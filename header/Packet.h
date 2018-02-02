/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Packet.h
 * Author: hydra
 *
 * Created on February 1, 2018, 5:31 PM
 */

#ifndef PACKET_H
#define PACKET_H

#include "fileserver.h"

class Packet
{
public:
    Packet();
    Packet(const PACKET & pk);
    Packet(std::string s);
    ~Packet();

    bool appendData(int cmd);
    bool appendData(std::string s);
    std::string readHeader();
    std::string readData();
    PACKET getData();
    
    int getCMDHeader();
    
private:
    PACKET data;

    PACKET buildIntField(int cmd);
    PACKET buildStringField(std::string sdata);
};

#endif /* PACKET_H */

