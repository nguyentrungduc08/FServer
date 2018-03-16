/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   wrapsock.h
 * Author: hydra
 *
 * Created on February 21, 2018, 11:13 AM
 */

#ifndef WRAPSOCK_H
#define WRAPSOCK_H

#include "fileserver.h"

void Listen(int fd, int backlog);


void	 err_dump(const char *, ...);
void	 err_msg(const char *, ...);
void	 err_quit(const char *, ...);
void	 err_ret(const char *, ...);
void	 err_sys(const char *, ...);

int      SF_SSL_READ(int, SSL*, char*, int );
int      SF_SSL_WRITE(int, SSL*, char*, int );

#endif /* WRAPSOCK_H */

