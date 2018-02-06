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
    std::cout << "Create SQL object, MySQL version: " << mysql_get_client_info() << std::endl;
}

bool database::doConnection(std::string username, std::string password, std::string database){
    this->ServerDatabase = mysql_init(NULL);
    
    if (this->ServerDatabase == NULL){
        this->finish_with_error();
        return false;
    }
    
    //if (mysql_real_connect(this->ServerDatabase, "localhost", "testuser", "testuser", "FILE", 0, NULL, 0) == NULL ){
    if (mysql_real_connect(this->ServerDatabase, "localhost", username.c_str() , password.c_str() , database.c_str(), 0, NULL, 0) == NULL ){
        this->finish_with_error();
        mysql_close(this->ServerDatabase);
        return false;
    } else {
        std::cout << "Conection success  to database " << database << std::endl;
    }
    return true;
}

database::database(const database& orig) {
}

database::~database() {
    mysql_close(this->ServerDatabase);
}

void database::finish_with_error()
{
    std::cerr << "ERROR server database: " << mysql_error(this->ServerDatabase) << std::endl;
    mysql_close(this->ServerDatabase);
    exit(EXIT_FAILURE);        
}

std::vector<USER> database::getListUser(){
    std::vector<USER> listuser;
    if ( mysql_query(this->ServerDatabase, "SELECT * FROM USERS") ) {
        this->finish_with_error();
    }
    MYSQL_RES * result = mysql_store_result(this->ServerDatabase);
  
    if (result == NULL) {
        this->finish_with_error();
    }
    
    int num_fields = mysql_num_fields(result);
    
    //std::cout << "log: num of fields " << num_fields << std::endl;
    
    MYSQL_ROW row;
    
    while ((row = mysql_fetch_row(result))) 
    { 
        //for(int i = 0; i < num_fields; i++)  
        //    printf("%s ", row[i] ? row[i] : "NULL"); 
        //printf("\n"); 
        //std::cout <<"Id: " << row[0] << " username: " << row[1] <<" password: " << row[2] << " status: " << row[7] << std::endl;    
        USER user;
        user.id = atoi(row[0]);
        user.username = row[1];
        user.password = row[2];
        user.status = atoi(row[7]);
        listuser.pb(user);
    }
    //std::cout << "finish get list user!!!" << std::endl;
    mysql_free_result(result);
    
    return listuser;
}
