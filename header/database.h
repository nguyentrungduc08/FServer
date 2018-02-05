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
    virtual ~database();
private:
    
    MYSQL *ServerDatabase;
};

#endif /* DATABASE_H */

