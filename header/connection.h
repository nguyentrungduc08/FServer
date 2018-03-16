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
    
    Connection(int filedescriptor, fssl * sslcon, unsigned int connId, std::string defaultDir, std::string hostId, bool iSSL, unsigned short _commandOffset = 1);
    virtual ~Connection();    
    
    //global APIs
    void                        TLS_handshark();
    void                        Respond_CMD_ERROR();
    int                         get_CMD_HEADER();
    void                        classify_connection();
    void                        respond_Classify_Connection_Done(bool state);
    unsigned int                getConnectionId();
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
    bool                        get_Is_Classified();
    void                        set_Is_Classified_State(bool _state);
    std::string                 get_Username_Of_Connection();
    int                         get_Usser_Id_Of_Connection();
    unsigned int                get_Connection_Id();
    Session*                    get_Session();
    
    void                        respond_CMD_HEADER(int _cmd);
    void                        push_CounPING()     {++this->_countPING;}
    void                        reset_CounPING()    {this->_countPING = 0;}
    bool                        timeout_PING()      {return this->_countPING > 2 ? true:false;}
    
    //APIs handle main connection.
    bool                        handle_CMD_AUTHEN_LOGIN(const std::vector<USER> & listUser); 
    void                        respond_CMD_AUTHEN();
    void                        respond_PONG();
    void                        send_Download_CMD_MSG_FILE(FILE_TRANSACTION*);
    FILE_TRANSACTION*           handle_Upload_CMD_MSG_FILE();
    
    
    //APIs handle file connection.
    void                        handle_CMD_UPLOAD_FILE(std::vector<TOKEN> _listToken);
    void                        handle_CMD_DOWNLOAD_FILE(std::vector<TOKEN> _listToken);    
    void                        respond_CMD_UPLOAD_READY();
    bool                        get_Data_Write_Done_State();
    void                        set_Data_Write_Done_State(bool _state);
    void                        wirte_Data();
    void                        send_Data(long long _dataSize);
    void                        Respond_CMD_SAVE_FILE_FINISH();
    bool                        check_Respond_CMD_UPLOAD_FINISH();
    
    //testing apis
    void                        getAllData();
    
private:
    int                         _socketFd; // Filedescriptor per each threaded object
    SSL*                        _ssl;
    FileHandle*                 fo; // For browsing, writing and reading
    Session*                    session;
    std::string                 dir;
    std::string                 hostAddress;
    int                         _countPING;
    std::string                 _parameter;
    std::string                 _username;
    int                         _userID;
    struct timeval              _timeout;
    unsigned short              _commandOffset;
    unsigned long               _receivedPart;
    unsigned int                _connectionId;
    bool                        _isMainSocket;
    bool                        _isFileSocket;
    bool                        _closureRequested;
    bool                        _isSSL;
    bool                        _ConfirmedState; //this is main connection and gained permission
    bool                        _TLSHandsharkState;
    bool                        _isUploadConnection;
    bool                        _isDownloadConnection;
    bool                        _dataWriteDoneState;
    bool                        _isClassified;
    
    //testing apis
    void                        sendToClient(char* response, unsigned long length);   
    void                        sendToClient(std::string response);   
    bool                        commandEquals(std::string a, std::string b);    
    std::string                 filterOutBlanks(std::string inString);  
    static void                 getAllParametersAfter(std::vector<std::string> parameterVector, unsigned int currentParameter, std::string& theRest); 
};

#endif /* CONNECTION_H */

