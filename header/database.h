/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   database.h
 * Author: hydra
 *
 * Created on February 5, 2018, 11:26 AM
 */

#ifndef DATABASE_H
#define DATABASE_H


#include "fileserver.h"
#include <mysql/mysql.h>
#include <mysql/my_command.h>



class database {
public:
    database();
    database(const database& orig);
    
    bool doConnection(std::string username, std::string password, std::string database);
    std::vector<USER> getListUser();
    virtual ~database();
    
private:
    MYSQL *ServerDatabase;
    
    void finish_with_error();
};

#endif /* DATABASE_H */

