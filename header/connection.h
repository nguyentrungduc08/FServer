/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   connection.h
 * Author: nguyen trung duc
 *
 * Created on December 28, 2017, 11:33 AM
 */


/*
 * this class to handle connections form client to server
 * 
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include "../header/fileHandle.h"
#include <vector>
#include <cstdlib>
#include <string>
#include <iostream>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include <algorithm> // for transform command

// Separator for commands
#define SEPARATOR " "

class serverconnection {
public:
    
    /*
     * constructor to handle connection from client.
     * @filedescriptor fd of socket to get data from client
     * @connid id of connection
     * @defaulDir direction working 
     * @hostId id of host
     * @commandOffset 
     */
    serverconnection(int filedescriptor, unsigned int connId, std::string defaultDir, std::string hostId, unsigned short commandOffset = 1);
    
    std::string commandParser(std::string command);
    std::vector<std::string> extractParameters(std::string command);
    virtual ~serverconnection();
    void run();
    void respondToQuery();
    int getFD();
    bool getCloseRequestStatus();
    unsigned int getConnectionId();

private:
    int fd; // Filedescriptor per each threaded object
    int fdflags;
    bool closureRequested;
    std::vector<std::string> directories;
    std::vector<std::string> files;
    unsigned int connectionId;
    std::string dir;
    std::string hostAddress;
    bool uploadCommand;
    bool downloadCommand;
    std::string parameter;
    fileoperator* fo; // For browsing, writing and reading
    void sendToClient(char* response, unsigned long length);
    void sendToClient(std::string response);
    bool commandEquals(std::string a, std::string b);
    std::string filterOutBlanks(std::string inString);
    static void getAllParametersAfter(std::vector<std::string> parameterVector, unsigned int currentParameter, std::string& theRest);
    unsigned short commandOffset;
    unsigned long receivedPart;
};



#endif /* CONNECTION_H */

