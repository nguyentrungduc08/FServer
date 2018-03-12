/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../header/connection.h"
#include "../header/fileserver.h"
#include "../header/ssl.h"

void                         
Connection::handle_uploadRequest(std::vector<TOKEN> _listToken)
{
    std::cout << "#log conn: handle upload request" << std::endl;
    
    char            buffer[BUFFER_SIZE];
    int             bytes = -1;
    struct timeval  time = this->timeout;
    fd_set          fdset;
    int             rc;
    int             cmd;
    Packet*         pk;
    std::string     token, _sender, _receiver, _urlFile, _fileName, _fileSize;
    std::string     res;
    bzero(buffer, sizeof(buffer));
    FD_ZERO(&fdset);
    FD_SET(this->fd, &fdset);

    rc = select(this->fd + 1, &fdset, NULL, NULL, &time);

    if (rc == 0){
        std::cout << "#log conn: timeout request upload" << std::endl;
        this->closureRequested = true;
        return;
    }

    bytes        = SSL_read(this->ssl, buffer, sizeof(buffer));
    pk           = new Packet(std::string(buffer, bytes));
    if (pk->IsAvailableData())
        cmd      = pk->getCMDHeader();
    if (pk->IsAvailableData())
        token    = pk->getContent();
    bool _checkToken = false;
    rep(i,_listToken.size()){
        if (_listToken.at(i).second->compare(token))
            _checkToken = true;
    }
    if (_checkToken){
        std::cout << "#log conn: check token ok: " << _listToken.size() << std::endl;
        if (cmd == CMD_UPLOAD_FILE) {
            if (pk->IsAvailableData())
                _fileName = pk->getContent();
            if (pk->IsAvailableData())
                _fileSize = pk->getContent();
            this->fo->set_File_Size(_fileSize);
            std::cout << "#log conn: token: " << token << "\nfilename: " << _fileName  << "\nfileSize: " << _fileSize << " " << this->fo->get_File_Size() << std::endl;
            this->_isUploadConnection   = true; // upload hit!
            this->receivedPart          = 0;
            this->parameter             = _fileName;
            std::cout << "Preparing upload of file '" << this->parameter << "'" << std::endl;
            res = (this->fo->beginWriteFile(this->parameter) ? "Preparing for upload failed" : "Preparing for upload successful");
            std::cout <<"#log conn: " << res << std::endl; 
            this->response_uploadRequest();
            return;
        } if (cmd == CMD_MSG_FILE) {
            if (pk->IsAvailableData())
                _sender     = pk->getContent();
            if (pk->IsAvailableData())
                _receiver   = pk->getContent();
        } 
    }else {    
        this->closureRequested = true;
        delete  pk;
        return;
    }
}

void                        
Connection::response_uploadRequest()
{
    std::cout << "#log conn: response upload request" << std::endl;
    Packet*         pk;

    pk = new Packet();
    pk->appendData(CMD_UPLOAD_READY);
    pk->appendData(this->parameter);

    SSL_write(this->ssl,  &pk->getData()[0], pk->getData().size());

    delete pk;
}

bool
Connection::get_Data_Write_Done_State(){
    return this->_dataWriteDoneState;
}

void                        
Connection::wirte_Data(){
    char            buffer[BUFFER_SIZE];
    int             bytes;
    std::string     data;
    long long       _totalData      = this->fo->get_File_Size();
    long long       _recievedData   = this->fo->get_Data_Received();
    
    if (_totalData == _recievedData){
        this->_dataWriteDoneState = true;
        return;
    }
    else {
        if (_recievedData + sizeof(buffer) <= _totalData)
        {
            bytes   = SSL_read(this->ssl, buffer, sizeof(buffer));
            if (bytes > 0){
                data = std::string(buffer, bytes);
                std::cout << "#log conn: Write block" << std::endl;
                // Previous (upload) command continuation, store incoming data to the file
                std::cout << "#log conn: Part" << ++(this->receivedPart) << ": " << bytes << std::endl;
                this->fo->writeFileBlock(data);
            } else {
                //this->closureRequested = true;
                std::cerr << "#log conn: 1 read zero data" << std::endl;
            }
            return;
        } else {
            if ((_totalData - _recievedData < sizeof(buffer)) && (_totalData > _recievedData))  
            {
                bytes   = SSL_read(this->ssl, buffer, (_totalData - _recievedData));
                if (bytes > 0){
                    data = std::string(buffer, bytes);
                    std::cout << "#log conn: Write block" << std::endl;
                    // Previous (upload) command continuation, store incoming data to the file
                    std::cout << "#log conn: Part" << ++(this->receivedPart) << ": " << bytes << std::endl;
                    this->fo->writeFileBlock(data);
                } else {
                    //this->closureRequested = true;
                    std::cerr << "#log conn: 2 read zero data" << std::endl;
                }
            }
            return;
        }
    }
}
