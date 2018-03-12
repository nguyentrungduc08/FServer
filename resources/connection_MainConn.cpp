/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../header/connection.h"
#include "../header/fileserver.h"
#include "../header/ssl.h"

bool 
Connection::handle_CMD_AUTHEN_LOGIN(const  std::vector<USER> & listUser) {
    
    std::cout << "#log conn: authen connection!!!!" << std::endl;
    char            buffer[BUFFER_SIZE];
    int             bytes = -1;
    int             cmd;
    int             Sta = -1;
    std::string     status;
    fd_set          writeFdSet;
    fd_set          readFdSet;

    bzero(buffer, sizeof buffer);
    
    if (isSSL){ 
        bytes = SSL_read(this->_ssl, buffer, sizeof(buffer));
    } else {
        bytes = recv(this->_socketFd, buffer, sizeof(buffer), 0);
    }
    
    std::cout << "#log conn: size of data ssl read " << bytes << std::endl;
    if (bytes > 0){
        Packet *pk = new Packet(std::string(buffer,bytes));
        
        cmd = pk->getCMDHeader();
        std::string username, password;
        
        if (cmd == CMD_AUTHEN_LOGIN && !this->_ConfirmedState){
            username = pk->getContent();
            password = pk->getContent();
        } else {
            this->_ConfirmedState = false;
            return false;
        }
        
        std::cout << "#log conn: User request username: " <<username << " password: " << password << std::endl;
        
        delete pk;
        
        rep(i,listUser.size()){
            if ( listUser.at(i).username == username && listUser.at(i).password == password ){
                std::cout << "#log conn: debug status login ok" << std::endl;
                this->_username = username;
                return true;
            } 
        }
        
        std::cout << "#log conn: debug status login fail" << std::endl;
        return false;
    }
    return false;
} 

void 
Connection::respond_CMD_AUTHEN(){
    if (this->_ConfirmedState && !this->closureRequested){
        
        this->session->buildSession(this->connectionId, this->hostAddress);
        std::string ses = this->session->getSession();
        Packet *pk = new Packet();
        pk->appendData(CMD_AUTHEN_SUCCESS);
        pk->appendData(ses);
        std::cout << "#log conn: " << ses << std::endl;
        SSL_write(this->_ssl, &pk->getData()[0], pk->getData().size() );
        delete pk;
    } else {
        Packet *pk = new Packet();
        pk->appendData(CMD_AUTHEN_FAIL);
        SSL_write(this->_ssl, &pk->getData()[0], pk->getData().size() );
        delete pk;
    } 
}