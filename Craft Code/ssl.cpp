/*
         if (bytes < 0){
            switch (SSL_get_error(this->ssl, Sta)){
                case SSL_ERROR_NONE:
                    Sta = 0;
                    std::cout << "#log conn: SSL_read SSL_ERROR_NONE" << std::endl;
                    break;
                case SSL_ERROR_WANT_WRITE:
                    FD_SET(this->fd, &writeFdSet);
                    Sta = 1;
                    std::cout << "#log conn: SSL_read SSL_ERROR_WANT_WRITE" << std::endl;
                    break;
                case SSL_ERROR_WANT_READ:
                    FD_SET(this->fd, &readFdSet);
                    Sta = 1;
                    std::cout << "#log conn: SSL_read SSL_ERROR_WANT_READ" << std::endl;
                    break;
                case SSL_ERROR_ZERO_RETURN:
                    std::cout << "#log conn: SSL_ERROR_ZERO_RETURN" << std::endl;
                    Sta = -1;
                    break;
                case SSL_ERROR_SYSCALL:     
                    std::cout << "#log conn: SSL_read Peer closed connection during SSL handshake,status: " << status << std::endl;
                    Sta = -1;
                    break;
                default:
                    std::cout << "#log conn: SSL_read Unexpected error during SSL handshake,status: " << status << std::endl;
                    Sta = -1;
                    break;
            }
        }
 
 */
