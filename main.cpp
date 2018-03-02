/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: nguyen trung duc
 *
 * Created on December 27, 2017, 2:35 PM
 */

#include "header/fileserver.h"
#include "header/servercore.h"
#include "header/connection.h"
#include "header/fileHandle.h"
#include "header/md5.h"
#include "header/Packet.h"
#include "header/database.h"
#include "header/Session.h"

using namespace std;

void testFunction();

/*
 * 
 */
int main(int argc, char** argv) {

    //testFunction();
    rep(a,8)
        cout << a << " ";
    cout << endl;

    unsigned short commandOffset = 1; // For telnet, we need 3 because of the enter control sequence at the end of command (+2 characters)
    unsigned int port = 8888; // Port to listen on (>1024 for no root permissions required)
    std::string dir = "./"; // Default dir 
    if (argc < 2) {
        std::cout << "Usage: fileserver <dir> <port> [telnetmode=no], using default dir '" << dir << "' , port " << port << std::endl;
    } else {
        switch (argc) {
            case 4: // full parameter to seting server 
                commandOffset = 3; // If any 3rd parameter is given, the server is started for use with telnet as client
            case 3: // convert to int port listening 
                port = atoi(argv[2]); // Cast str to int, set port
            case 2:
                FileHandle* db = new FileHandle(dir);
                // Test if dir exists
                if (db->dirCanBeOpenend(argv[1])) {
                    dir = argv[1]; // set default server directory
                    db->changeDir(dir, false); // Assume the server side is allowed to change any directory as server root (thus the false for no strict mode)
                    cout << "@dir opened" << endl;
                } else {
                    std::cout << "Invalid path specified ('" << argv[1] << "'), falling back to '" << dir << "'" << std::endl;
                }
                break;
        }
    }

    cout << "create server" << endl;
    servercore* myServer = new servercore(port, dir, commandOffset); // create server 

    // @TODO: some sort of server shutdown command??
    delete myServer;
    
    return (EXIT_SUCCESS);
}

void testFunction() {
    Packet *pk = new Packet();
    
    pk->appendData((int) CMD_DOWNLOAD_FINISH);
    pk->appendData("whoami");
    pk->appendData("hello");
    
    
    cout << pk->getCMDHeader() << endl;
    while (pk->IsAvailableData()){
        cout << "Content: " << pk->getContent() << endl;
    }
    
    Session * ss = new Session();
    ss->buildSession(2,"192.1.1.1");
    
    delete ss;
    /*
    database *data = new database();
    
    vector<USER> listuser; 
    listuser = data->getListUser();
    
    cout << "size of list user: "<< listuser.size() << endl;
    */        
    cout << "md5 of 'grape': " << md5("testuser") << endl;

    delete pk;
    //delete data;

    return;
}