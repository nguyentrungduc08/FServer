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

class Connection {
public:
    
    Connection(int filedescriptor, fssl * sslcon, unsigned int connId, std::string defaultDir, std::string hostId, bool iSSL, unsigned short commandOffset = 1);
    virtual ~Connection();    
    
    std::string                 commandParser(std::string command);
    std::vector<std::string>    extractParameters(std::string command);
    
    void                        classify_connection();
    void                        getAllData();
    bool                        authConnection(const std::vector<USER> & listUser); 
    
    void                        respondAuthen();
    void                        TLS_handshark();
    unsigned int                getConnectionId();
    void                        handle_uploadRequest(std::vector<TOKEN> _listToken);
    void                        response_uploadRequest();
    void                        wirte_Data();
    int                         getFD();
    bool                        get_Close_Request_Status();
    void                        set_Close_Request_Status(bool status);
    bool                        get_TLShandshark_state();
    void                        set_TLShandshark_state(bool state);
    bool                        get_authen_state();
    void                        set_authen_state(bool state);
    void                        set_isMainConnection(bool state);
    bool                        get_isMainConnection();
    void                        set_isFileConnection(bool state);
    bool                        get_isFileConnection();
    bool                        get_isUploadConnection();
    bool                        get_isDownloadConnection();
    bool                        get_Data_Write_Done_State();

    bool                        get_Is_Classified();
    void                        set_Is_Classified_State(bool _state);
    std::string                 get_Username_Of_Connection();
    int                         get_Usser_Id_Of_Connection();
    unsigned int                get_Connection_Id();
    Session*                    get_Session();
    FILE_TRANSACTION*           handle_CMD_MSG_FILE(); 
    
private:
    int                         fd; // Filedescriptor per each threaded object
    SSL*                        ssl;
    FileHandle*                 fo; // For browsing, writing and reading
    Session*                    session;
    std::vector<std::string>    directories;
    std::vector<std::string>    files;
    unsigned int                connectionId;
    std::string                 dir;
    std::string                 hostAddress;
    std::string                 parameter;
    std::string                 _username;
    int                         _userID;
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
    
    bool                        _isUploadConnection;
    bool                        _isDownloadConnection;
    bool                        _dataWriteDoneState;
    bool                        _isClassified;
    
    int                         get_CMD_HEADER();
    
    void                        sendToClient(char* response, unsigned long length);   
    void                        sendToClient(std::string response);
    bool                        commandEquals(std::string a, std::string b);    
    std::string                 filterOutBlanks(std::string inString);  
    static void                 getAllParametersAfter(std::vector<std::string> parameterVector, unsigned int currentParameter, std::string& theRest); 
    void                        respondClassifyConnectionDone(bool state);
    
    void                        Respond_CMD_SAVE_FILE_FINISH();
};

#endif /* CONNECTION_H */

