/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   database.cpp
 * Author: hydra
 * 
 * Created on February 5, 2018, 11:26 AM
 */

#include "../header/database.h"


database::database() {
    std::cout << "MySQL client version: " << mysql_get_client_info() << std::endl;
    
    this->ServerDatabase = mysql_init(NULL);
    
    if (this->ServerDatabase == NULL){
        std::cerr <<"ERROR init connection database: " << mysql_errno(this->ServerDatabase) << std::endl;
        return;
    }
    
    if (mysql_real_connect(this->ServerDatabase, "localhost", "testuser", "testuser", "FILE", 0, NULL, 0) == NULL ){
        std::cerr <<"ERROR connect to database " << mysql_errno(this->ServerDatabase) << std::endl;
        mysql_close(this->ServerDatabase);
        return;
    } else {
        std::cout <<"Conection success" << std::endl;
    }
}

database::database(const database& orig) {
}

database::~database() {
}

