/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../header/connection.h"
#include "../header/fileserver.h"
#include "../header/ssl.h"

void                         
Connection::handle_CMD_UPLOAD_FILE(std::vector<TOKEN> _listToken)
{
    std::cout << "#log conn: handle upload request handle_CMD_UPLOAD_FILE" << std::endl;
    
    char            _buffer[BUFFER_SIZE];
    int             _bytes = -1;
    struct timeval  _time = this->timeout;
    fd_set          _fdset;
    int             _rc;
    int             _cmd;
    Packet*         _pk;
    std::string     _tokenString, _sender, _receiver, _urlFile, _fileName, _fileSize;
    std::string     _result;

    bzero(_buffer, sizeof(_buffer));
    // FD_ZERO(&_fdset);
    // FD_SET(this->_socketFd, &_fdset);

    // _rc = select(this->_socketFd + 1, &_fdset, NULL, NULL, &_time);

    // if (_rc == 0){
    //     std::cout << "#log conn: timeout request upload" << std::endl;
    //     this->closureRequested = true;
    //     return;
    // }

    _bytes        = SSL_read(this->_ssl, _buffer, sizeof(_buffer));
    std::cout << "#log conn: handle upload request read data " << _bytes <<  std::endl;
    _pk           = new Packet(std::string(_buffer, _bytes));

    if (_pk->IsAvailableData())
        _tokenString    = _pk->getContent();

    std::cout << "#log conn: handle upload request token - " << _tokenString <<" list size - " << _listToken.size() <<  std::endl;
    bool _checkToken = false;
    //rep(i,_listToken.size())
    for(int i = 0; i < _listToken.size(); ++i)
    {
        //std::cout <<"#log conn: token system: " << _listToken.at(i).second->getSession() << " - " << _listToken.at(i).second->getSession().size() << std::endl;
        //std::cout <<"#log conn: token received: " << _tokenString << " - " << _tokenString.size() << std::endl;  
        if (_listToken.at(i).second->getSession() == _tokenString)
            _checkToken = true;
    }

    if (_checkToken){
        std::cout << "#log conn: check token ok: " << _listToken.size() << std::endl;
        
        if (_pk->IsAvailableData())
            _fileName = _pk->getContent();
        if (_pk->IsAvailableData())
            _fileSize = _pk->getContent();
        this->fo->set_File_Size(_fileSize);
        std::cout << "#log conn: token: " << _tokenString << "\nfilename: " << _fileName  << "\nfileSize: " << _fileSize << " " << this->fo->get_File_Size() << std::endl;
        this->_isUploadConnection    = true; // upload hit!
        this->_receivedPart          = 0;
        this->_parameter             = _fileName;
        std::cout << "Preparing upload of file '" << this->_parameter << "'" << std::endl;
        _result = (this->fo->beginWriteFile(this->_parameter) ? "Preparing for upload failed" : "Preparing for upload successful");
        std::cout <<"#log conn: " << _result << std::endl; 
        this->respond_CMD_UPLOAD_READY();
        delete _pk;
        return;
    }else {    
        std::cout << "#log conn: token invalid!!!! " << _listToken.size() << std::endl;
        this->closureRequested = true;
        delete  _pk;
        return;
    }
}

void                        
Connection::respond_CMD_UPLOAD_READY()
{
    std::cout << "#log conn: response upload request" << std::endl;
    Packet*         pk;

    pk = new Packet();
    pk->appendData(CMD_UPLOAD_READY);
    pk->appendData(this->_parameter);

    SSL_write(this->_ssl,  &pk->getData()[0], pk->getData().size());

    delete pk;
}

bool
Connection::get_Data_Write_Done_State(){
    return this->_dataWriteDoneState;
}

void                        
Connection::set_Data_Write_Done_State(bool _state){
    this->_dataWriteDoneState = _state;
}

void                        
Connection::wirte_Data(){
    char            buffer[BUFFER_SIZE];
    int             _bytes;
    std::string     _data;
    long long       _totalData      = this->fo->get_File_Size();
    long long       _recievedData   = this->fo->get_Data_Received();
    
    if (_totalData == _recievedData){
        this->_dataWriteDoneState = true;
        return;
    }
    else {
        if (_recievedData + sizeof(buffer) <= _totalData)
        {
            _bytes   = SSL_read(this->_ssl, buffer, sizeof(buffer));
            if (_bytes > 0){
                _data = std::string(buffer, _bytes);
                std::cout << "#log conn: Write block" << std::endl;
                // Previous (upload) command continuation, store incoming data to the file
                std::cout << "#log conn: Part" << ++(this->_receivedPart) << ": " << _bytes << std::endl;
                this->fo->writeFileBlock(_data);
            } else {
                //this->closureRequested = true;
                std::cerr << "#log conn: 1 read zero data" << std::endl;
            }
            return;
        } else {
            if ((_totalData - _recievedData < sizeof(buffer)) && (_totalData > _recievedData))  
            {
                _bytes   = SSL_read(this->_ssl, buffer, (_totalData - _recievedData));
                if (_bytes > 0){
                    _data = std::string(buffer, _bytes);
                    std::cout << "#log conn: Write block" << std::endl;
                    // Previous (upload) command continuation, store incoming data to the file
                    std::cout << "#log conn: Part" << ++(this->_receivedPart) << ": " << _bytes << std::endl;
                    this->fo->writeFileBlock(_data);
                } else {
                    //this->closureRequested = true;
                    std::cerr << "#log conn: 2 read zero data" << std::endl;
                }
            }
            return;
        }
    }
}

void                       
Connection::Respond_CMD_SAVE_FILE_FINISH()
{
    //send CMD_SAVE_FILE_FINISH
    Packet*     _pk;
    
    _pk = new Packet();
    _pk->appendData(CMD_UPLOAD_FINISH);
    
    SSL_write(this->_ssl,  &_pk->getData()[0], _pk->getData().size());
    
    delete _pk;
    return;
}