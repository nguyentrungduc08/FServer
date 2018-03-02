/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Session.cpp
 * Author: nguyen trung duc
 * 
 * Created on February 7, 2018, 2:10 PM
 */

#include "../header/Session.h"

Session::Session() {
}

Session::Session(const Session& orig) {
}

Session::~Session() {
}

void 
Session::setCurrentTime(){
    time_t      rawtime;
    struct tm*  timeinfo;
    char        buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%d-%m-%Y %I:%M:%S",timeinfo);
    std::string str(buffer);
    this->currentTime = str;
}

void 
Session::buildSession(int conid, std::string ipAddr){
    this->setCurrentTime();
    std::stringstream ss;
    ss << conid;
    std::string inputString = ss.str() +  ipAddr + this->currentTime;    
    this->sessionCode = md5(inputString);
    //std::cout <<"input : " << conid << " " << ipAddr << " " << this->currentTime << " output: " << this->sessionCode << std::endl; 
}

std::string 
Session::getSession(){
    return this->sessionCode;
}

std::string 
Session::getCurrentTime(){
    return this->currentTime;
}

bool        
Session::is_Session_Valid(){
    return true;
}