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
    //global APIs
    int             start_Server();
    int             init_Sockets(int port);
    int             handle_New_Connection();
    void            set_NonBlocking(int &sock);
    int             get_Num_User_Active();
    
    //APIs handle file connection.
    void            handle_File_Connection(Connection* & conn);
    int             check_File_Transaction(std::string _receiver);
    void            build_Select_list_For_File_Connection();
    void            thread_File_Connecion_Handle();
    void            read_Data_File_Connections();
    
    //APIs handle main connection.
    void            handle_Main_Connection(Connection* & conn);
    void            thread_Main_Connecion_Handle();
    void            build_Select_list_For_Main_Connection();
    void            read_Data_Main_Connections();
    void            update_List_Users_Active_Online(std::string );
    void            update_List_Users_Active_Offline(std::string );
    
    //APIs using in main thread.
    void            build_Select_List_For_Connections();    
    void            free_All_Connections();
    void            free_All_File_Connections();
    void            free_All_Main_Connections();
    void            free_All_File_Transaction();
    void            read_Data_Main_Socket();   
    
    //testing apis
    int             start();
    void            build_Select_List();
    void            read_Sockets();

    fd_set                  _connectionsSet;
    fd_set                  _mainConnSet;
    fd_set                  _fileConnSet;
    int                     _mainSocket; // The main listening socket file descriptor
    int                     _highestFdConnSet;
    int                     _highestFdMainSet;
    int                     _highestFdFileSet;
    struct timeval          _serverTimeout;
    bool                    _shutdown; //to set server on/off
    unsigned int            _maxConnectionsInQuery; // number of connections in query
    unsigned int            _connId;

    
    int                     highSock;
    unsigned int            connId; 
    sockaddr_in             addr; // set server information
    struct sockaddr_storage addrStorage; //get info of connection
    socklen_t               addrLength;
    sockaddr_in             cli;
    socklen_t               cli_size;
    unsigned short          commandOffset;
    fd_set                  _workingSet; // set of socket file descriptors we want to wake up for, using select()
    std::string             dir; //path of directory working 
    fssl*                   sslConn;
    Database*               _database;
    
    std::vector<Connection*>                _connections; // Manage the connected sockets / connections in a list with an iterator
    std::vector<Connection*>                _mainConnections;
    std::vector<Connection*>                _fileConnections;
    std::vector<USER>                       _listUser;
    std::vector<FILE_TRANSACTION*>          _listFileTransaction;
    std::vector<TOKEN>                      _listSession;
};

#endif /* SERVERCORE_H */

