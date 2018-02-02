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

#include <bits/stdc++.h>
#include <cstdlib>
#include <cstdio>
#include <sstream>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <resolv.h>

#include "openssl/ssl.h"
#include "openssl/err.h"
#include "openssl/md5.h"

#include <fstream>
#include <dirent.h>
#include <locale.h>
#include <pwd.h>
#include <grp.h>
#include <stdint.h>
#include <unistd.h>


#define TRUE 1
#define FALSE 0

#define USE_SSL                 1
#define NO_SSL                  2

#define CMD_AUTHEN_LOGIN        1
#define CMD_AUTHEN_SUCCESS      2

#define CMD_ADD_CONTACT         3
#define CMD_ADD_SUCCESS         4

#define CMD_UPLOAD_FILE         5
#define CMD_UPLOAD_READY        6
#define CMD_MSG_FILE            7
#define CMD_DOWNLOAD_FILE       7
#define CMD_DOWNLOAD_FINISH     8

#define CMD_SHARE_FILE          10
#define CMD_SHARE_WAITING       11
#define CMD_RECEIVE_FILE        12
#define CMD_GET_FILE            13
#define CMD_SHARE_CONFIRM       14
#define CMF_SHARE_FINISH        15

#define PACKET std::vector<unsigned char>

#define rep(i,n) for(int i = 0; i < (int) n; ++i)

struct USER{
    std::string username;
    std::string password;
};



#endif /* FILESERVER_H */

