/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../../header/logger.h"

void Logger_Message(std::string msg)
{
    std:: cout << msg << "\n";
}

void Logger_Message_Mutex(std::mutex &_myMutex, std::string msg)
{
    std::lock_guard<std::mutex> _guard(_myMutex);
    std:: cout << msg << "\n";
}