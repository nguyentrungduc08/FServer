/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Packet.cpp
 * Author: nguyen trung duc
 * 
 * Created on February 1, 2018, 5:32 PM
 */

#include "../../header/Packet.h"

Packet::Packet()
{
    this->data.clear();
}

Packet::Packet(const PACKET & pk){
    this->data.clear();
    this->data = pk;
}

Packet::Packet(const std::string & str){
    this->data.clear();
    std::copy(str.begin(), str.end(), std::back_inserter(this->data) );
}


Packet::~Packet(){
    this->data.clear();
}

PACKET Packet::buildIntField(int cmd){
    PACKET vc = { (char) 0xFF & (cmd >> 24), (char) 0xFF & (cmd >> 16), (char) 0xFF & (cmd >> 8 ), (char) 0xFF &  cmd };
   
    /*
    vc.push_back( (char) 0xFF & (cmd >> 24) );
    vc.push_back( (char) 0xFF & (cmd >> 16) );
    vc.push_back( (char) 0xFF & (cmd >> 8 ) );
    vc.push_back( (char) 0xFF &  cmd        );
    */
   
    return vc;
}

PACKET 
Packet::buildStringField(std::string sField){
    int leng = sField.length();

    PACKET sData;
    sData.clear();

    PACKET vtlength;
    vtlength = this->buildIntField(leng);

    std::copy(vtlength.begin(), vtlength.end(), std::back_inserter(sData));
    std::copy(sField.begin(), sField.end(), std::back_inserter(sData));
    return sData;
}

PACKET 
Packet::getData(){
    return this->data;
}

bool 
Packet::appendData(int cmd)
{
    std::cout   << "%Log build paccket: buil Int" 
                << std::endl;
    
    PACKET pk;
    pk.clear();
    pk = buildIntField(cmd);

    std::copy(pk.begin(), pk.end(), std::back_inserter(this->data));

    return true;
}

bool 
Packet::appendData(std::string s)
{
    std::cout   << "%Log build paccket: buil string" 
                << std::endl;
    
    PACKET pk;
    pk.clear();
    pk = buildStringField(s);

    std::copy(pk.begin(), pk.end(), std::back_inserter(this->data) );
    return true;
}


int 
Packet::getCMDHeader(){
    int cmd = 0;
    //cmd+= (this->data[0] << 24) + (this->data[1] << 16) + (this->data[2] << 8) + (this->data[3]);
    for(int i = 0; i < 4; ++i)
        cmd+= ((int)this->data[i] << ( 24 - i * 8 ) );
    
    this->data.erase(this->data.begin(), this->data.begin() + 4);
    return cmd;
}


bool 
Packet::IsAvailableData()
{
    return (this->data.size() > 0) ? true : false;
}

std::string 
Packet::getContent(){
    int len = this->getCMDHeader();
    
    std::string res (this->data.begin(), this->data.begin() + len);

    this->data.erase(this->data.begin(), this->data.begin() + len);

    return res;
}

std::string 
Packet::getData_stdString(){
    std::string da(this->data.begin(), this->data.end());
    return da;
}