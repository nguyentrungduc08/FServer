/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../../header/connection.h"
#include "../../header/fileserver.h"
#include "../../header/ssl.h"

bool 
Connection::handle_CMD_AUTHEN_LOGIN(const  std::vector<USER> & listUser) 
{   
    std::cout   << "#log conn: authen connection!!!!" 
                << std::endl;
    
    char            _buffer[BUFFER_SIZE];
    int             _bytes = -1;
    int             _cmd;
    int             _Sta = -1;
    std::string     _status;
    fd_set          _writeFdSet;
    fd_set          _readFdSet;

    bzero(_buffer, sizeof _buffer);
    
    if (this->_isSSL){ 
        _bytes = SSL_read(this->_ssl, _buffer, sizeof(_buffer));
    } else {
        _bytes = recv(this->_socketFd,_buffer, sizeof(_buffer), 0);
    }
    
    std::cout   << "#log conn: size of data ssl read " << _bytes 
                << std::endl;
    
    if (_bytes > 0){
        Packet *_pk = new Packet(std::string(_buffer,_bytes));
        
        _cmd = _pk->getCMDHeader();
        std::string _username, _password;
        
        if (_cmd == CMD_AUTHEN_LOGIN && !this->_ConfirmedState){
            _username = _pk->getContent();
            _password = _pk->getContent();
        } else {
            this->_ConfirmedState = false;
            return false;
        }
        
        std::cout   << "#log conn: User request username: " << _username 
                    << " password: "                        << _password 
                    << std::endl;
        
        delete _pk;
        
        rep(i,listUser.size()){
            if ( listUser.at(i).username == _username && listUser.at(i).password == _password ){
                std::cout << "#log conn: debug status login ok" << std::endl;
                this->_username = _username;
                return true;
            } 
        }
        
        std::cout   << "#log conn: debug status login fail" 
                    << std::endl;
        
        return false;
    }
    return false;
} 

FILE_TRANSACTION* 
Connection::handle_Upload_CMD_MSG_FILE()
{
    char                _buffer[BUFFER_SIZE];
    int                 _bytes, _cmd;
    Packet*             _pk;
    std::string         _sender, _receiver, _urlFile, _filesize;
    FILE_TRANSACTION*   _ft;
    
    _bytes   = SSL_read(this->_ssl, _buffer, sizeof(_buffer));
    
    if (_bytes > 0){
        _pk = new Packet(std::string(_buffer,_bytes));
        if (_pk->IsAvailableData())
            _sender     = _pk->getContent();
        if (_pk->IsAvailableData())
            _receiver   = _pk->getContent();
        if (_pk->IsAvailableData())
            _urlFile    = _pk->getContent();
        if (_pk->IsAvailableData())
            _filesize   = _pk->getContent();
        
        std::cout   << "#log conn: msg\n-cmd: "  << CMD_MSG_FILE << 
                       "\n-sender: "             << _sender      << 
                       "\n-receiver: "           << _receiver    << 
                       "\n-urlfile: "            << _urlFile     <<
                       "\n-file size: "          << _filesize    << std::endl; 
        
        _ft = new FILE_TRANSACTION;
        
        _ft->_sender    = _sender;
        _ft->_receiver  = _receiver;
        _ft->_url       = _urlFile;
        _ft->_filesize  = std::stoi(_filesize);
        _ft->_status    = false;
        delete _pk;
        return _ft;
    } 
    
    return NULL;
}


void                        
Connection::send_Download_CMD_MSG_FILE(FILE_TRANSACTION *_fileTransaction)
{
    Packet      *_pk = new Packet();
    fd_set      _FDSet;
    
    _pk->appendData(CMD_MSG_FILE);
    _pk->appendData(_fileTransaction->_sender);
    _pk->appendData(_fileTransaction->_receiver);
    _pk->appendData(_fileTransaction->_url);
    _pk->appendData(std::to_string(_fileTransaction->_filesize));
    
    std::cout << "#log conn: msg send to client"    <<
                 "\n-cmd: "                         << CMD_MSG_FILE                 << 
                 "\n-sender: "                      << _fileTransaction->_sender    << 
                 "\n-receiver: "                    << _fileTransaction->_receiver  << 
                 "\n-urlfile: "                     << _fileTransaction->_url       <<
                 "\n-file size: "                   << _fileTransaction->_filesize  << std::endl; 
    
    FD_ZERO(&_FDSet);
    FD_SET(this->_socketFd,&_FDSet);
    
    int _rc = select(_socketFd+1, NULL, &_FDSet, NULL, NULL); // wait until can sendable
    
    if (_rc > 0){
        SSL_write(this->_ssl, &_pk->getData()[0], _pk->getData().size());
        _fileTransaction->_status = 1;
    }
    
    delete _pk;
    return;
}


void                        
Connection::respond_PONG()
{
    Packet *_pk = new Packet();
    _pk->appendData(PONG);
    SSL_write(this->_ssl, &_pk->getData()[0], _pk->getData().size() );
    delete _pk;
    return;
}

void 
Connection::respond_CMD_AUTHEN(){
    if (this->_ConfirmedState && !this->_closureRequested){
        
        this->session->buildSession(this->_connectionId, this->hostAddress);
        std::string ses = this->session->getSession();
        Packet *pk = new Packet();
        pk->appendData(CMD_AUTHEN_SUCCESS);
        pk->appendData(ses);
        
        std::cout   << "#log conn: "    << ses 
                    << std::endl;
        
        SSL_write(this->_ssl, &pk->getData()[0], pk->getData().size() );
        delete pk;
    } else {
        Packet *pk = new Packet();
        pk->appendData(CMD_AUTHEN_FAILURE);
        SSL_write(this->_ssl, &pk->getData()[0], pk->getData().size() );
        delete pk;
    } 
}