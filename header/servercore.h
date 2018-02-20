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

#include "fileserver.h"
#include "connection.h"
#include "ssl.h"
#include "database.h"

class servercore {
public:
    
    /*
     * constructor to create server
     * @port: port to listent service 
     * @dir: set directory working
     * @commandOffset: ...
     */
    servercore(uint port,std::string dir, unsigned short commandOffset = 1);
    
    ~servercore();

private:
    
    /*
     * Server entry point and main loop accepting and handling connections
     */ 
    int start();
    
    /* 
     * Initialization of sockets / socket list with options and error checking
     * @port create socket to listening 
     */ 
    int initSockets(int port);
    
    /* 
     * Sets the given socket to non-blocking mode
     * @sock parameter reference to set socket non-blocking
     */
    void setNonBlocking(int &sock);
    
    /*
     * build list socket to handle new connection or data from clients
     */
    void buildSelectList();
    
    /*
     * read data from socket
     */
    void readSockets();
    
    /*
     * Accepts new connections and stores the connection object with fd in a vector 
     */
    int handleNewConnection();
    
    /*
     * free all memories 
     */
    void freeAllConnections();
    

    unsigned int maxConnectionsInQuery; // number of connections in query
    int Mainsocket; // The main listening socket file descriptor
    int highSock;
    unsigned int connId; 
    bool shutdown; //to set server on/off
    sockaddr_in addr; // set server information
    struct sockaddr_storage addrStorage; //get info of connection
    socklen_t addrLength;
    sockaddr_in cli;
    socklen_t cli_size;
    unsigned short commandOffset;
    fd_set working_set; // set of socket file descriptors we want to wake up for, using select()
    std::string dir; //path of directory working 

    std::vector<serverconnection*> connections;// Manage the connected sockets / connections in a list with an iterator
    std::vector<USER> listUser;
    std::vector<serverconnection*> mainConnections;
    std::vector<serverconnection*> fileConnections;
     
    fssl * sslConn;
    database *DataBase;
};

#endif /* SERVERCORE_H */

