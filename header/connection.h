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

#include "fileserver.h"
#include "fileHandle.h"
#include "Packet.h"
#include "Session.h"
#include "ssl.h"

// Separator for commands
#define SEPARATOR " "

/*
 * this class handle one connection from client to server.
 * 
 */

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
    serverconnection(int filedescriptor, fssl * sslcon, unsigned int connId, std::string defaultDir, std::string hostId, bool iSSL, unsigned short commandOffset = 1);
    virtual ~serverconnection();    
    
    std::string                 commandParser(std::string command);
    std::vector<std::string>    extractParameters(std::string command);
    
    void                        classify_connection();
    void                        getAllData();
    bool                        authConnection(const std::vector<USER> & listUser); 
    void                        respondToQuery();
    void                        respondAuthen();
    void                        TLS_handshark();
    unsigned int                getConnectionId();
    int                         getFD();
    
    bool                        getCloseRequestStatus();
    void                        setCloseRequestStatus(bool status);
    bool                        get_TLShandshark_state();
    void                        set_TLShandshark_state(bool state);
    bool                        get_authen_state();
    void                        set_authen_state(bool state);
    void                        set_isMainConnection(bool state);
    bool                        get_isMainConnection();
    void                        set_isFileConnection(bool state);
    bool                        get_isFileConnection();
    
private:
    int                         fd; // Filedescriptor per each threaded object
    SSL*                        ssl;
    filehandle*                 fo; // For browsing, writing and reading
    Session * session;
    
    std::vector<std::string>    directories;
    std::vector<std::string>    files;
    unsigned int                connectionId;
    std::string                 dir;
    std::string                 hostAddress;
    std::string                 parameter;
    
    struct timeval              timeout;
    
    unsigned short              commandOffset;
    unsigned long               receivedPart;
    
    bool                        isMainSocket;
    bool                        isFileSocket;
    bool                        closureRequested;
    bool                        uploadCommand;
    bool                        downloadCommand;
    bool                        isSSL;
    bool                        ConfirmedState;
    bool                        TLSHandsharkState;
    
    /*
     * @response  data response to client
     * @length size of data
     * @return void
     */
    void                        sendToClient(char* response, unsigned long length);
    
    /*
     * @response 
     */
    void                        sendToClient(std::string response);
    
    bool                        commandEquals(std::string a, std::string b);
    
    std::string                 filterOutBlanks(std::string inString);
    
    static void                 getAllParametersAfter(std::vector<std::string> parameterVector, unsigned int currentParameter, std::string& theRest);
    
    void                        respondClassifyConnectionDone(bool state);
};

#endif /* CONNECTION_H */

