/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Packet.cpp
 * Author: hydra
 * 
 * Created on February 1, 2018, 5:32 PM
 */

#include "../header/Packet.h"

Packet::Packet()
{
    this->data.clear();
}

Packet::Packet(const PACKET & pk){
    this->data.clear();
    this->data = pk;
}

Packet::Packet(std::string str){
    this->data.clear();
    std::copy(str.begin(), str.end(), std::back_inserter(this->data) );
}

Packet::~Packet(){
    this->data.clear();
}

PACKET Packet::buildIntField(int cmd){
    PACKET vc (4);
    /*
    bytes[0] = (n >> 24) & 0xFF;
    bytes[1] = (n >> 16) & 0xFF;
    bytes[2] = (n >> 8) & 0xFF;
    bytes[3] = n & 0xFF;
    */
    vc[0] = (cmd >> 24) & 0xFF;
    vc[1] = (cmd >> 16) & 0xFF;
    vc[2] = (cmd >> 8 ) & 0xFF;
    vc[3] =  cmd   		& 0xFF;
    return vc;
}

PACKET Packet::buildStringField(std::string sField){
    int leng = sField.length();

    PACKET sData;
    sData.clear();

    PACKET vtlength;
    vtlength = this->buildIntField(leng);

    sData.insert(sData.end(), vtlength.begin(), vtlength.end());
    sData.insert(sData.end(), sField.begin(), sField.end());

    return sData;
}

PACKET Packet::getData(){
    return this->data;
}

bool Packet::appendData(int cmd){
    PACKET pk;
    pk.clear();
    pk = buildIntField(cmd);

    this->data.insert(this->data.end(), pk.begin(), pk.end());

    return true;
}

bool Packet::appendData(std::string s){
    PACKET pk;
    pk.clear();
    pk = buildStringField(s);

    this->data.insert(this->data.begin(), pk.begin(), pk.end());
    return true;
}


int Packet::getCMDHeader(){
    int cmd =0;
    //cmd+= this->data[0] << 24 + this->data[1] << 16 + this->data[2] << 8 + this->data[3];
    rep(i,4)
        cmd+= this->data[i] << (8*(4-i-1));
    return cmd;
}
