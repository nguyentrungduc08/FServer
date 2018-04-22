/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ssl.h
 * Author: nguyen trung duc
 *
 * Created on January 10, 2018, 12:03 PM
 */

#ifndef SSL_H
#define SSL_H

#include "fileserver.h"

class  fssl{
public:
    fssl();
    ~fssl();
    
    void        create_context();
    void        configure_context(std::string cerfile, std::string keyfile);
    SSL_CTX*    get_ctx();
    void        set_verify_client(bool is_verify);
    
private:
    SSL_CTX*    ctx;
    bool        verify_client;
};


#endif /* SSL_H */

