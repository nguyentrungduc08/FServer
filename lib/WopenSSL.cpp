/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../header/wrap.h"

int     
SF_SSL_READ(int _fd, SSL* _ssl, char* _buffer , int _sizebuf)
{
    int         _bytes, rc;
    fd_set      _fdSet;
    
    FD_ZERO(&_fdSet);
    FD_SET(_fd, &_fdSet);
    
    rc = select(_fd + 1, &_fdSet, NULL, NULL, NULL);
    
    _bytes = SSL_read(_ssl, _buffer, _sizebuf);
    
    return _bytes;
}

int     
SF_SSL_WRITE(int _fd, SSL* _ssl, char* _buffer , int _sizebuf)
{
    int         _bytes, rc;
    fd_set      _fdSet;
    
    FD_ZERO(&_fdSet);
    FD_SET(_fd, &_fdSet);
    
    rc = select(_fd + 1, NULL, &_fdSet, NULL, NULL);
    
    _bytes = SSL_write(_ssl, _buffer, _sizebuf);
    
    return _bytes;
}