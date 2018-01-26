/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   fileserver.h
 * Author: nguyen trung duc
 *
 * Created on December 28, 2017, 2:55 PM
 */

#ifndef FILESERVER_H
#define FILESERVER_H

#include <cstdlib>
#include <iostream>


#define USE_SSL                 1
#define NO_SSL                  2

#define CMD_AUTHEN_LOGIN        801
#define CMD_AUTHEN_SUCCESS      802
#define CMD_ADD_CONTACT         803
#define CMD_ADD_SUCCESS         804
#define CMD_GET_CONTACTS        805
#define CMD_CONTACTS_DATA       806 
#define CMD_UPLOAD_FILE         807
#define CMD_UPLOAD_READY        808
#define CMD_MSG_FILE            809
#define CMD_DOWNLOAD_FILE       800
#define CMD_DOWNLOAD_FINISH     888


#endif /* FILESERVER_H */

