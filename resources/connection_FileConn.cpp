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
    int             _bytes = -1, _rc, _cmd;
    struct timeval  _time = this->_timeout;
    fd_set          _fdset;
    Packet*         _pk;
    std::string     _tokenString, _sender, _receiver, _urlFile, _fileName, _fileSize, _result;

    bzero(_buffer, sizeof(_buffer));

    _bytes        = SSL_read(this->_ssl, _buffer, sizeof(_buffer));
    _pk           = new Packet(std::string(_buffer, _bytes));

    if (_pk->IsAvailableData())
        _tokenString    = _pk->getContent();

    bool _checkToken = false;
    rep(i,_listToken.size())
    { 
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
    }else {    
        std::cout << "#log conn: token invalid!!!! " << _listToken.size() << std::endl;
        this->_closureRequested = true;
    }
    
    delete _pk;
    return;
}

/*
 *@TODO create thread to sent file data to client  
 * 
 */
void
Connection::handle_CMD_DOWNLOAD_FILE(std::vector<TOKEN> _listToken)
{
    Packet*         _pk;
    char            _buffer[BUFFER_SIZE];
    struct timeval  _time = this->_timeout;
    fd_set          _fdset;
    int             _bytes;
    std::string     _result, _tokenString, _fileName;
    bzero(_buffer, sizeof(_buffer));

    _bytes        = SSL_read(this->_ssl, _buffer, sizeof(_buffer));
    _pk           = new Packet(std::string(_buffer, _bytes));

    if (_pk->IsAvailableData()) 
        _tokenString    = _pk->getContent();

    bool _checkToken = false;
    rep(i,_listToken.size())
    { 
        if (_listToken.at(i).second->getSession() == _tokenString)
            _checkToken = true;
    }
    
    if (_checkToken){
        if (_pk->IsAvailableData())
            _fileName = _pk->getContent();
        this->_isDownloadConnection     = true; // upload hit!
        this->_parameter                = _fileName;
        std::cout << "Preparing download of file '" << this->_parameter << "'" << std::endl;
        _result = (this->fo->readFile(this->_parameter) ? "Preparing for download failed" : "Preparing for download successful");
    }
}

void                        
Connection::respond_CMD_UPLOAD_READY()
{
    std::cout << "#log conn: response upload request" << std::endl;
    Packet*         _pk;
    _pk = new Packet();
    _pk->appendData(CMD_UPLOAD_READY);
    _pk->appendData(this->_parameter);
    SSL_write(this->_ssl,  &_pk->getData()[0], _pk->getData().size());
    delete _pk;
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
        if (_recievedData + sizeof(buffer) <= _totalData) {
            _bytes   = SSL_read(this->_ssl, buffer, sizeof(buffer));
            if (_bytes > 0){
                _data = std::string(buffer, _bytes);
                std::cout << "#log conn: Part" << ++(this->_receivedPart) << ": " << _bytes << std::endl;
                this->fo->writeFileBlock(_data);
            } else {
                std::cerr << "#log conn: 1 read zero data" << std::endl;
            }
        } else {
            if ((_totalData - _recievedData < sizeof(buffer)) && (_totalData > _recievedData))  
            {
                _bytes   = SSL_read(this->_ssl, buffer, (_totalData - _recievedData));
                if (_bytes > 0){
                    _data = std::string(buffer, _bytes);
                    std::cout << "#log conn: Part" << ++(this->_receivedPart) << ": " << _bytes << std::endl;
                    this->fo->writeFileBlock(_data);
                } else {
                    std::cerr << "#log conn: 2 read zero data" << std::endl;
                }
            }
        }
        return;
    }
}

bool
Connection::check_Respond_CMD_UPLOAD_FINISH()
{
    std::cout << "Log Connection: check_Respond_CMD_UPLOAD_FINISH" << std::endl;
    int _cmd = this->get_CMD_HEADER();
    if (_cmd == CMD_UPLOAD_FINISH)
        return true;
    else
        return false;
}

void                       
Connection::Respond_CMD_SAVE_FILE_FINISH()
{
    //send CMD_SAVE_FILE_FINISH
    Packet*     _pk;
    _pk = new Packet();
    _pk->appendData(CMD_SAVE_FILE_FINISH);
    SSL_write(this->_ssl,  &_pk->getData()[0], _pk->getData().size());
    delete _pk;
    return;
}