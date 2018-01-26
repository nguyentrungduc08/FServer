/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../header/servercore.h"


servercore::servercore(uint port, std::string dir, unsigned short commandOffset) : dir(dir), commandOffset(commandOffset), shutdown(false), connId(0) {
    if ( chdir( dir.c_str() ) ) //change working directory
        std::cerr << "Directory could not be changed to '" << dir << "'!" << std::endl;
    
    if (USE_SSL){
        std::cout << "begin load certificate" << std::endl;
        this->sslComm = new fssl();  //create and load some lib
        this->sslComm->create_context(); //
        this->sslComm->configure_context("CA/server.crt", "CA/server.key.pem");
        std::cout << "load certificate finished" << std::endl;
    }
    
    this->initSockets(port); // create socket to listening and set socket attribute
    this->start();
}

/* 
 * Free up used memory by cleaning up all the object variables;
*/ 
servercore::~servercore() {
    std::cout << "Server shutdown" << std::endl;
    close(this->s);
    this->freeAllConnections(); // Deletes all connection objects and frees their memory
    delete sslComm;
}

// Builds the list of sockets to keep track on and removes the closed ones
// @TODO: Crash if data is incoming over a closed socket connection???
void servercore::buildSelectList() {
    FD_ZERO(&(this->working_set));
    FD_SET(this->s, &(this->working_set));
    
    std::vector<serverconnection*>::iterator iter = this->connections.begin();
    
    while( iter != this->connections.end() ) {
        // This connection was closed, flag is set -> remove its corresponding object and free the memory
        if ( (*iter)->getCloseRequestStatus() == true ) { 
            std::cout << "Connection with Id " << (*iter)->getConnectionId() << " closed! " << std::endl;
            delete (*iter); // Clean up
            this->connections.erase(iter); // Delete it from our vector
            if ( this->connections.empty() || (iter == this->connections.end()) )
                return; // Don't increment the iterator when there is nothing to iterate over - avoids crash
        } else {
            int currentFD = (*iter)->getFD();
            if (currentFD != 0) {
                FD_SET(currentFD, &(this->working_set)); // Adds the socket file descriptor to the monitoring for select
                if (currentFD > this->highSock)
                    this->highSock = currentFD; // We need the highest socket for select
            }
            ++iter; // Increment iterator
        }
        //++iter; // Increment iterator
    }
}

// Clean up everything
void servercore::freeAllConnections() {
    std::vector<serverconnection*>::iterator iter = this->connections.begin();
    while( iter != this->connections.end() ) {
        delete (*(iter++)); // Clean up, issue destructor implicitly
    }
    this->connections.clear(); // Delete all deleted connections also from our vector
}

// Accepts new connections and stores the connection object with fd in a vector
int servercore::handleNewConnection() {
    int fd; // Socket file descriptor for incoming connections
    int reuseAllowed = 1;
    
    this->cli_size = sizeof(this->cli);
    
    fd = accept(this->s, (struct sockaddr*) &cli, &cli_size);
    
    if (fd < 0) {
        std::cerr << "Error while accepting client" << std::endl;
        return (EXIT_FAILURE);
    }

    // Gets the socket fd flags and add the non-blocking flag to the fd
    this->setNonBlocking(fd);    
    
    // Something (?) went wrong, new connection could not be handled
    if (fd == -1) {
        std::cerr << "Something went wrong, new connection could not be handled (Maybe server too busy, too many connections?)" << std::endl;
        try {
            close(fd);
        } catch (std::exception e) {
            std::cerr << e.what() << std::endl;
        }
        return (EXIT_FAILURE); // Return at this point
    }
    
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuseAllowed, sizeof(reuseAllowed)) < 0) { //  enable reuse of socket, even when it is still occupied
        std::cerr << "setsockopt() failed" << std::endl;
        close (fd);
        return EXIT_FAILURE;
    }

    // Get the client IP address
    char ipstr[INET6_ADDRSTRLEN];
    int port;
    this->addrLength = sizeof this->addrStorage;
    getpeername(fd, (struct sockaddr*) &this->addrStorage, &(this->addrLength));
    std::string hostId = "";
    if (this->addr.sin_family == AF_INET) {
        struct sockaddr_in* fd = (struct sockaddr_in*) &(this->addrStorage);
        port = ntohs(fd->sin_port);
        inet_ntop(AF_INET, &fd->sin_addr, ipstr, sizeof ipstr);
        hostId = (std::string)ipstr;
    }

    printf("Connection accepted: FD=%d - Slot=%d - Id=%d \n", fd, (this->connections.size()+1), ++(this->connId));
    // The new connection (object)
    serverconnection* conn = new serverconnection(fd, this->sslComm ,this->connId, this->dir, hostId, true, this->commandOffset); // The connection vector

    if ( conn->authConnection()){
        // Authen success  
        // Add it to our list for better management / convenience
        this->connections.push_back(conn);
    } else{
        // Authen fail
        printf("Connection dropped: FD=%d - Slot=%d - Id=%d (authentication failure)\n", fd, (this->connections.size()+1), this->connId);
        return (EXIT_FAILURE);
    } 
    return (EXIT_SUCCESS);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
// Something is happening (=data ready to read) at a socket, either accept a new connection or handle the incoming data over an already opened socket
void servercore::readSockets() {
    // OK, now working_set will be set with whatever socket(s) are ready for reading. First check our "listening" socket, and then check the sockets in connectlist
    // If a client is trying to connect() to our listening socket, select() will consider that as the socket being 'readable' and thus, if the listening socket is part of the fd_set, accept a new connection
    if (FD_ISSET(this->s,&(this->working_set))) {
//        this->handleNewConnection();
        if (this->handleNewConnection()) return; // Always check for errors
    }
    // Now check connectlist for available data
    // Run through our sockets and check to see if anything happened with them
    for (unsigned int listnum = 0; listnum < this->connections.size(); ++listnum) {
        if (FD_ISSET( this->connections.at(listnum)->getFD(), &(this->working_set) ) ) {
            this->connections.at(listnum)->respondToQuery(); // Now that data is available, deal with it!
        }
    }
}


int servercore::start() { 
    int readworking_set; // Number of sockets ready for reading
    // Wait for connections, main server loop
    while (!this->shutdown) {
        std::cout << "waiting connection form client....." << std::endl;

        this->buildSelectList(); // Clear out data handled in the previous iteration, clear closed sockets

        // Multiplexes between the existing connections regarding to data waiting to be processed on that connection (that's actually what select does)
        struct timeval timeout;
        timeout.tv_sec = 3; // Timeout = 3 sec
        timeout.tv_usec = 0;
        readworking_set = select(this->highSock+1, &(this->working_set), NULL , NULL , &timeout);

        if (readworking_set < 0) {
            std::cerr << "Error calling select" << std::endl;
            return (EXIT_FAILURE);
        }

        this->readSockets(); // Handle the sockets (accept new connections or handle incoming data or do nothing [if no data])
    }
    return (EXIT_SUCCESS);
}

void servercore::setNonBlocking(int &sock) {
    this->sflags = fcntl(sock, F_GETFL); // Get socket flags
    int opts = fcntl(sock,F_GETFL, 0);
    if (opts < 0) {
        std::cerr << "Error getting socket flags" << std::endl;
        return;
    }
    opts = (opts | O_NONBLOCK);
    if (fcntl(sock,F_SETFL,opts) < 0) {
        std::cerr << "Error setting socket to non-blocking" << std::endl;
        return;
    }
}

void servercore::initSockets(int port) {
    
    int reuseAllowed = 1; //set flag to set socket reusable 
    this->maxConnectionsInQuery = 500; //set maximum connect to server simultaneously
    this->addr.sin_family = AF_INET; // PF_INET;
    this->addr.sin_port = htons(port); 
    this->addr.sin_addr.s_addr = INADDR_ANY; // Server can be connected to from any host
    // PF_INET: domain, Internet; SOCK_STREAM: datastream, TCP / SOCK_DGRAM = UDP => WARNING, this can change the byte order!; for 3rd parameter==0: TCP preferred
    
    this->s = socket(PF_INET, SOCK_STREAM, 0); 
    //craete socket fail
    if (this->s == -1) {
        std::cerr << "socket() failed" << std::endl;
        return;
    }
    else if (setsockopt(this->s, SOL_SOCKET, SO_REUSEADDR, &reuseAllowed, sizeof(reuseAllowed)) < 0) { //  enable reuse of socket, even when it is still occupied
        std::cerr << "setsockopt() failed" << std::endl;
        close (this->s);
        return;
    }
    
    this->setNonBlocking(this->s);
    if (bind(this->s, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        std::cerr << ("bind() failed (do you have the apropriate rights? is the port unused?)") << std::endl;
        close (this->s);
        return;
    } // 2nd parameter (backlog): number of connections in query, can be also set SOMAXCONN
    else if (listen(this->s, this->maxConnectionsInQuery) == -1) {
        std::cerr << ("listen () failed") << std::endl;
        close (this->s);
        return;
    }
    this->highSock = this->s; // This is the first (and the main listening) socket
    std::cout << "Server started and listening at port " << port << ", default server directory '" << this->dir << "'" << ((this->commandOffset == 3) ? ", for use with telnet" : "")  << std::endl;
}
