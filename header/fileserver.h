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
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <mutex>

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

#define SERVER_CAPACITY             10000

#define TRUE                        1
#define FALSE                       0

#define USE_SSL                     1
#define NO_SSL                      2

#define CMD_AUTHEN_LOGIN            1
#define CMD_AUTHEN_SUCCESS          2
#define CMD_AUTHEN_FAILURE          121
    
#define CMD_ERROR                   500

#define CMD_ADD_CONTACT             3
#define CMD_ADD_SUCCESS             4

#define CMD_UPLOAD_FILE             5
#define CMD_UPLOAD_READY            6
#define CMD_UPLOAD_FINISH           65
#define CMD_SAVE_FILE_FINISH        56
#define CMD_MSG_FILE                7
#define CMD_DOWNLOAD_FILE           8
#define CMD_DOWNLOAD_FINISH         9
#define CMD_DOWNLOAD_FIN_SEND       27
#define CMD_DOWNLOAD_READY_SEND     28
#define CMD_DOWNLOAD_READY_SAVE     29

#define CMD_SHARE_FILE              10
#define CMD_SHARE_WAITING           11
#define CMD_RECEIVE_FILE            12
#define CMD_GET_FILE                13
#define CMD_SHARE_CONFIRM           14
#define CMF_SHARE_FINISH            15

#define CMD_IS_MAIN_CONNECTION      88
#define CMD_IS_FILE_CONNECTION      99
#define CMD_CLASSIFY_DONE           89
#define CMD_CLASSIFY_FAILURE        98
    
#define MAXCONN                     1000
#define	BUFFSIZE                    4096	/* buffer size for reads and writes */

#define PING                        31
#define PONG                        32

#define PACKET                      std::vector<char>
#define TOKEN                       std::pair<int,Session*>                  

#define rep(i,n)                    for(int i =      0; i  < (int)n; ++i)
#define Ford(i,a,b)                 for(int i = (int)a; i >= (int)b; --i)
#define For(i,a,b)                  for(int i = (int)a; i <= (int)b; ++i)
#define pb                          push_back
#define X                           first
#define Y                           second


struct USER{
    int             id;
    std::string     username;
    std::string     password;
    int             status;
    bool            _state = false;
};

struct FILE_TRANSACTION {
    unsigned int    _transsactionId;
    int             _senderID;
    int             _receiverID;
    std::string     _sender;
    std::string     _receiver;
    std::string     _url;
    std::string     _checksum;
    long long       _filesize;
    bool            _status = false;
};


//#ifndef _GLOBAL_VARIABLE
//#define _GLOBAL_VARIABLE
//        
//std::mutex              SERVER_MUTEX;
//
//#endif

#endif /* FILESERVER_H */

