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
    int             start();
    int             init_Sockets(int port);
    int             handle_New_Connection();
    void            set_NonBlocking(int &sock);
    void            build_Select_List();
    void            read_Sockets();
    void            free_All_Connections();
    void            handle_Main_Connection(Connection* & conn);
    void            handle_File_Connection(Connection* & conn);
    
    unsigned int            maxConnectionsInQuery; // number of connections in query
    int                     Mainsocket; // The main listening socket file descriptor
    int                     highSock;
    unsigned int            connId; 
    bool                    shutdown; //to set server on/off
    sockaddr_in             addr; // set server information
    struct sockaddr_storage addrStorage; //get info of connection
    socklen_t               addrLength;
    sockaddr_in             cli;
    socklen_t               cli_size;
    unsigned short          commandOffset;
    fd_set                  working_set; // set of socket file descriptors we want to wake up for, using select()
    std::string             dir; //path of directory working 
    fssl*                   sslConn;
    Database*               database;
    
    std::vector<Connection*>  connections;// Manage the connected sockets / connections in a list with an iterator
    std::vector<USER>         listUser;
    std::vector<Connection*>  mainConnections;
    std::vector<Connection*>  fileConnections;
};

#endif /* SERVERCORE_H */

