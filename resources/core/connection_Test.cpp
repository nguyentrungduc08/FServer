/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../../header/connection.h"
#include "../../header/fileserver.h"
#include "../../header/ssl.h"

// Sends the given string to the client using the current connection
void 
Connection::sendToClient(std::string response) {
    // Now we're sending the response
    unsigned int bytesSend = 0;
    while (bytesSend < response.length()) {
        int ret;
        if (!this->_isSSL)
            ret = send(this->_socketFd, response.c_str()+bytesSend, response.length()-bytesSend, 0);
        else
            ret = SSL_write(this->_ssl, response.c_str()+bytesSend, response.length()-bytesSend);
        if (ret <= 0) {
            return;
        }
        bytesSend += ret;
    }
}

/*
 * @response  data response to client
 * @length size of data
 * @return void
 */
void 
Connection::sendToClient(char* response, unsigned long length) {
    // Now we're sending the response
    unsigned int bytesSend = 0;
    while (bytesSend < length) {
        int ret;
        if (!this->_isSSL)
            ret = send(this->_socketFd, response+bytesSend, length-bytesSend, 0);
        else
            ret = SSL_write(this->_ssl, response + bytesSend, length-bytesSend);
        if (ret <= 0) {
            return;
        }
        bytesSend += ret;
    }
}