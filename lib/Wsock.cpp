/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../header/wrap.h"

void Listen(int fd, int backlog){
    char *ptr;
    
    /* can override 2nd argument with environment variable */
    if ( ( ptr == getenv("LISTENQ")) != NULL )
        backlog = atoi(ptr);
    
    if ( listen(fd, backlog) < 0 )
        err_sys("listen error!!!");
}


