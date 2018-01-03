/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   fileHandle.h
 * Author: nguyen trung duc
 *
 * Created on December 28, 2017, 11:33 AM
 */

#ifndef FILEHANDLE_H
#define FILEHANDLE_H


#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <list>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <locale.h>
#include <pwd.h>
#include <grp.h>
#include <stdint.h>
#include <unistd.h>


// Buffer size
#define BUFFER_SIZE 409600

// This contains the designation for the server root directory
#define SERVERROOTPATHSTRING "<root>/"

/* The strict parameter distincts the access rights:
 * strict = true:
 *  Only file and directory names in the current working dir are permitted as parameter,
 *  especially references over several directories like ../../filename are prohibited
 *  to ensure we do not drop under the server root directory by user command,
 *  used with client side commands
 * strict = false:
 *  References over several directories like ../../filename are allowed as parameters,
 *  only used at server side
 */
class filehandle {
    
public:
    
    filehandle(std::string dir);
    virtual ~filehandle();
    
    int readFile(std::string fileName);
    char* readFileBlock(unsigned long &sizeInBytes);
    
    int writeFileAtOnce(std::string fileName, char* content);
    
    int beginWriteFile(std::string fileName);
    int writeFileBlock(std::string content);
    int closeWriteFile();
    bool changeDir(std::string newPath, bool strict = true);
    std::string getCurrentWorkingDir(bool showRootPath = true);
    bool createFile(std::string &fileName, bool strict = true);
    bool createDirectory(std::string &dirName, bool strict = true);
    bool deleteDirectory(std::string dirName, bool cancel = false, std::string pathToDir = "");
    bool deleteFile(std::string fileName, bool strict = true);
    void browse(std::string dir, std::vector<std::string> &directories, std::vector<std::string> &files, bool strict = true);
    bool dirCanBeOpenend(std::string dir); //check this folder can be opened.
    std::string getParentDir();
    unsigned long getDirSize(std::string dirName);
    std::vector<std::string> getStats(std::string fileName, struct stat Status);
    void clearListOfDeletedFiles();
    void clearListOfDeletedDirectories();
    std::vector<std::string> getListOfDeletedFiles();
    std::vector<std::string> getListOfDeletedDirectories();
    bool dirIsBelowServerRoot(std::string dirName);
private:
    
    std::vector<std::string> deletedDirectories;
    std::vector<std::string> deletedFiles;
    void getValidDir(std::string &dirName);
    void getValidFile(std::string &fileName);
    void stripServerRootString(std::string &dirOrFileName);
    std::ofstream currentOpenFile; //stream to read data from socket
    std::ifstream currentOpenReadFile; //stream to send data to socket
    std::list<std::string> completePath; // The path from server root dir upwards to the current working dir, each list element containing one dir
    static void IntToString(int i, std::string &res);
};


#endif /* FILEHANDLE_H */

