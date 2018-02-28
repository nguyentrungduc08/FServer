/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Session.h
 * Author: nguyen trung duc
 *
 * Created on February 7, 2018, 2:10 PM
 */

#ifndef SESSION_H
#define SESSION_H

#include "fileserver.h"
#include "md5.h"

class Session {
public:
    Session();
    Session(const Session& orig);
    virtual ~Session();

    void        buildSession(int conid, std::string ipAddr);
    std::string getSession();
    std::string getCurrentTime();
    bool        is_SessionValib();
   
private:
    std::string sessionCode;
    std::string currentTime;
    
    void        setCurrentTime();
};

#endif /* SESSION_H */

