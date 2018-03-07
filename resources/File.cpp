/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   File.cpp
 * Author: hydra
 * 
 * Created on March 7, 2018, 3:58 PM
 */

#include "../header/File.h"

File::File()
{
    this->_fileSize     = -1;
    this->_filePatch    = "";
}

File::~File()
{
    if (this->_readStream.is_open())
        this->_readStream.close();
    if (this->_writeStream.is_open())
        this->_writeStream.close();
}

File::File(std::string _patch){
    this->_filePatch    = _patch;
    this->_fileSize     = -1;
}

bool
File::open_File_To_Read(std::string _fileName)
{
    // modes for binary file  |std::ios::ate
    this->_readStream.open(_fileName.c_str(), std::ios::in|std::ios::binary);
    if (this->_readStream.fail()) {
        std::cout << "Reading file '" << _fileName << "' failed!" << std::endl;
        return (false);
    }
    if (this->_readStream.is_open()) {
        this->_readStream.seekg(0, this->_readStream.beg);
        return (true);
    }
    std::cerr << "Unable to open file '" << _fileName << " '" << std::endl;
    return (false);
}

bool
File::open_File_To_Read()
{
    this->_readStream.open(this->_filePatch.c_str(), std::ios::in|std::ios::binary);
    if (this->_readStream.fail()) {
        std::cout << "Reading file '" << this->_filePatch << "' failed!" << std::endl;
        return (false);
    }
    if (this->_readStream.is_open()) {
        this->_readStream.seekg(0, this->_readStream.beg);
        return (true);
    }
    std::cerr << "Unable to open file '" << this->_filePatch << " '" << std::endl;
    return (false);
}

void
File::close_Read_Stream(){
    this->_readStream.close();
}

void
File::format_FileName(std::string& filename)
{
    //check filepatch format QML
    //if (filename.find("file://")){
        filename.erase(filename.begin(), filename.begin()+7);
        std::cout << "!Log file: file path formated: " << filename << std::endl;
    //}
    return;
}


std::string
File::get_File_Name(std::string filepath)
{
    int post = -1;
    //linux operator
    for(int i = sizeof(filepath) - 1; i >=0; --i){
        if (filepath[i] == '/')
        {
            post = i;
            break;
        }
    }
    if (post == -1)
        return std::string(filepath);
    else{
        //cout << post  <<" " << filepath << " " << sizeof(filepath)<< " " << filepath.length() - post << endl;
        std::string filename = filepath.substr(post+1, filepath.length() - post);
        return filename;
    }
}

void
File::read_File_Block(char* buffer, int sizeblock)
{
    this->_readStream.read(buffer, sizeblock);
}

/*
 *get size from of file
 */
long long
File::get_Size()
{
    std::streampos  _begin, _end;
    std::ifstream   _myfile(this->_filePatch.c_str(), std::ios::binary);
    _begin  = _myfile.tellg();
    _myfile.seekg (0, std::ios::end);
    _end    = _myfile.tellg();
    _myfile.close();
    return (_end - _begin);
}

void
File::set_File(std::string file)
{
    this->_filePatch = file;
    this->compute_Size();
}

void
File::compute_Size()
{
    std::streampos  _begin, _end;
    std::ifstream   _myfile(this->_filePatch.c_str(), std::ios::binary);
    _begin  = _myfile.tellg();
    _myfile.seekg (0, std::ios::end);
    _end    = _myfile.tellg();
    _myfile.close();
    this->_fileSize = _begin - _end;
}

std::string
File::get_Size_stdString()
{
    return std::to_string(this->_fileSize);
}

