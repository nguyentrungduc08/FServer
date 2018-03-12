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
    std::vector<Connection*>::iterator _iter = this->_fileConnections.begin();

    while (_iter != this->_fileConnections.end()){
        if ( (*_iter)->get_Close_Request_Status() ){
            std::cout << "@log servercore: For_File_Connection Connection with Id " << (*_iter)->getConnectionId() << " closed! " << std::endl;
            delete (*_iter);
            this->_fileConnections.erase(_iter);
            if ( this->_fileConnections.empty() || _iter == this->_fileConnections.end()){
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
    for (unsigned int _index = 0; _index < this->_fileConnections.size(); ++_index) {
        if (FD_ISSET(this->_fileConnections.at(_index)->getFD(), &(this->_fileConnSet))) {
            std::cout << "@log servercore: read_Data_File_Connections " << this->_fileConnections.at(_index)->getFD() << std::endl;
            if ( this->_fileConnections.at(_index)->get_isFileConnection() ){
                std::cout << "@log servercore: handle data file connection" << std::endl;
                this->handle_File_Connection(this->_fileConnections.at(_index));
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
        if (_cmd == CMD_UPLOAD_FILE)
        _conn->handle_CMD_UPLOAD_FILE(this->_listSession);
    }
    else {
        //conn->respondToQuery();
        _conn->wirte_Data();
        if (_conn->get_Data_Write_Done_State()) {
            FILE_TRANSACTION * _ft;
            _ft = _conn->handle_CMD_MSG_FILE();
            if (_ft != NULL){
                std::cout << "@log servercore: add file transsaction completed" << std::endl;
                this->_listFileTransaction.emplace_back(_ft);
            }
        }
    }
    
}