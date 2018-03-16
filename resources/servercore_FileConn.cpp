/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../header/servercore.h"

void            
servercore::build_Select_list_For_File_Connection()
{
    FD_ZERO(&(this->_fileConnSet));
    this->_highestFdFileSet = 0;
    std::vector<Connection*>::iterator _iter = this->_listFileConnections.begin();

    while (_iter != this->_listFileConnections.end()){
        if ( (*_iter)->get_Close_Request_Status() ){
            std::cout << "@log servercore: For_File_Connection Connection with Id " << (*_iter)->getConnectionId() << " closed! " << std::endl;
            delete (*_iter);
            this->_listFileConnections.erase(_iter);
            if ( this->_listFileConnections.empty() || _iter == this->_listFileConnections.end()){
                return;
            }
        } else {
            int _currentFD = (*_iter)->getFD();
            if (_currentFD != 0) {
                FD_SET(_currentFD, &(this->_fileConnSet));
                if (_currentFD > this->_highestFdFileSet)
                    this->_highestFdFileSet = _currentFD;
            }
            ++_iter;
        }
    }
}


void            
servercore::read_Data_File_Connections()
{
    for (unsigned int _index = 0; _index < this->_listFileConnections.size(); ++_index) {
        if (FD_ISSET(this->_listFileConnections.at(_index)->getFD(), &(this->_fileConnSet))) {
            std::cout << "@log servercore: read_Data_File_Connections " << this->_listFileConnections.at(_index)->getFD() << std::endl;
            if ( this->_listFileConnections.at(_index)->get_isFileConnection() ){
                std::cout << "@log servercore: handle data file connection" << std::endl;
                this->handle_File_Connection(this->_listFileConnections.at(_index));
                continue;
            }
        }
    }
}

void            
servercore::thread_File_Connecion_Handle()
{
    int                 _num_Fd_Incomming;
    struct timeval      _time;
        
    while (!this->_shutdown) {
        std::cout << "@log servercore: File thread waiting connections form client....." << std::endl;
        this->build_Select_list_For_File_Connection();

        _time                 = this->_serverTimeout;
        _num_Fd_Incomming     = select(this->_highestFdFileSet+1, &(this->_fileConnSet), NULL, NULL, &_time);

        if (_num_Fd_Incomming < 0){
            std::cerr << "@log servercore: Error calling select()" << std::endl;
            return;
        }

        this->read_Data_File_Connections();
    }    
    return;
}

void 
servercore::handle_File_Connection(Connection* & _conn)
{
    std::cout << "@log servercore: handle file connection!!!" << std::endl;
    int _cmd;

    if (!_conn->get_isUploadConnection()){
        _cmd = _conn->get_CMD_HEADER();
        switch (_cmd){
            case CMD_UPLOAD_FILE: 
                _conn->handle_CMD_UPLOAD_FILE(this->_listSession);
                break;
            case CMD_DOWNLOAD_FILE:
                //std::cout << "@log servercore: this connection send download request!! " << std::endl;
                _conn->handle_CMD_DOWNLOAD_FILE(this->_listSession);
                break;
        }
    }
    else 
        if (_conn->get_isUploadConnection()){
            _conn->wirte_Data();
            if (_conn->get_Data_Write_Done_State()) {
                if (_conn->check_Respond_CMD_UPLOAD_FINISH()){
                    _conn->Respond_CMD_SAVE_FILE_FINISH();
                } else {
                    std::cerr << "@log servercore: client do not send CMD_UPLOAD_FINISH !!!!" << std::endl;
                }
                _conn->set_Close_Request_Status(true);      
            }
        }
    
}