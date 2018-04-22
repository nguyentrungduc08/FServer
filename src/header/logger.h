/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   logger.h
 * Author: hydra
 *
 * Created on March 27, 2018, 10:05 AM
 */

#ifndef LOGGER_H
#define LOGGER_H

#include "fileserver.h"

void Logger_Message(std::string msg);
void Logger_Message_Mutex(std::mutex &_myMutex, std::string msg);



#endif /* LOGGER_H */

