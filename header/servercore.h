/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   servercore.h
 * Author: nguyen trung duc
 *
 * Created on December 28, 2017, 11:33 AM
 */

#ifndef SERVERCORE_H
#define SERVERCORE_H

#include "../header/connection.h"
#include "../header/ssl.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class servercore {
public:
    
    //constructor to create server
    /*
     - port: port to listent service 
     - dir: set directory working
     - commandOffset: ...
     */
    servercore(uint port,std::string dir, unsigned short commandOffset = 1);
    ~servercore();

private:
    
    //start server 
    int start();
    
    //create socket to server listenning 
    void initSockets(int port);
    //set attribute non-blocking for socket
    void setNonBlocking(int &sock);
    //build list socket to handle new connection or data from clients
    void buildSelectList();
    //read data from socket
    void readSockets();
    
    /*
     * Accepts new connections and stores the connection object with fd in a vector 
     */
    int handleNewConnection();
    
    
    void freeAllConnections();
    unsigned int maxConnectionsInQuery; // number of connections in query
    int s; // The main listening socket file descriptor
    int sflags; // Socket flags
    std::vector<serverconnection*> connections;// Manage the connected sockets / connections in a list with an iterator
    int highSock; // Highest #'d file descriptor, needed for select()
    fd_set working_set; // set of socket file descriptors we want to wake up for, using select()
    std::string dir; //path of directory working 
    unsigned int connId; //
    bool shutdown; //to set serer on/off
    sockaddr_in addr; // set server information
    struct sockaddr_storage addrStorage; //get info of connection
    socklen_t addrLength;
    sockaddr_in cli;
    socklen_t cli_size;
    unsigned short commandOffset;
    fssl * sslComm;
};

#endif /* SERVERCORE_H */

