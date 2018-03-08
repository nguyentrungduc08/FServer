/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../header/servercore.h"

servercore::servercore(uint port, std::string dir, unsigned short commandOffset) 
                        : dir(dir), commandOffset(commandOffset), shutdown(false), connId(0) 
{
    this->connections.clear();
    this->listUser.clear();
    this->database = new Database();
    
    if ( !this->database->doConnection("testuser","testuser","FILE") ){
        std::cerr << "ERROR connecting to database!!!!" << std::endl;
        exit(EXIT_FAILURE);
    } else{
        this->listUser = this->database->getListUser();
        rep(i,this->listUser.size()){
            std::cout << "Id: " << this->listUser.at(i).id << " Username: " << this->listUser.at(i).username << " Password: " << this->listUser.at(i).password << std::endl;
        }
        std::cout << "@log servercore: load list user successfull." << std::endl;
    }
    
    if (USE_SSL){
        std::cout << "@log servercore: begin load certificate" << std::endl;
        this->sslConn = new fssl();  //create and load some lib
        this->sslConn->create_context(); //
        this->sslConn->configure_context("CA/server.crt", "CA/server.key.pem");
        std::cout << "@log servercore: load certificate finished" << std::endl;
    } 
    
    if ( chdir( dir.c_str() ) ) //change working directory
        std::cerr << "@log servercore: Directory could not be changed to '" << dir << "'!" << std::endl;
   
    this->init_Sockets(port); // create socket to listening and set socket attribute
    this->start();
}

/* 
 * Free up used memory by cleaning up all the object variables;
 */ 
servercore::~servercore() 
{
    std::cout << "@log servercore: Server shutdown" << std::endl;
    close(this->Mainsocket); 
    this->free_All_Connections(); // Deletes all connection objects and frees their memory
    delete this->sslConn;
    delete this->database;
}

// Builds the list of sockets to keep track on and removes the closed ones
// @TODO: Crash if data is incoming over a closed socket connection???
void 
servercore::build_Select_List() 
{
    FD_ZERO(&(this->working_set));
    FD_SET(this->Mainsocket, &(this->working_set));
    
    std::vector<Connection*>::iterator iter = this->connections.begin();
    
    while( iter != this->connections.end() ) {
        // This connection was closed, flag is set -> remove its corresponding object and free the memory
        if ( (*iter)->get_Close_Request_Status() ) { 
            std::cout << "@log servercore: Connection with Id " << (*iter)->getConnectionId() << " closed! " << std::endl;
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
void 
servercore::free_All_Connections() 
{
    std::vector<Connection*>::iterator iter = this->connections.begin();
    while( iter != this->connections.end() ) {
        delete (*(iter++)); // Clean up, issue destructor implicitly
    }
    this->connections.clear(); // Delete all deleted connections also from our vector
    
}

// Accepts new connections and stores the connection object with fd in a vector
int 
servercore::handle_New_Connection() 
{
    int             fd; // Socket file descriptor for incoming connections
    char            ipstr[INET6_ADDRSTRLEN];
    int             port;
    std::string     hostId          = "";
    int             reuseAllowed    = 1;
    this->cli_size                  = sizeof(this->cli);
    fd                              = accept(this->Mainsocket, (struct sockaddr*) &this->cli, &this->cli_size);
    
    if (fd < 0) {
        std::cerr << "@log servercore: Error while accepting client" << std::endl;
        return (EXIT_FAILURE);
    }

    // Gets the socket fd flags and add the non-blocking flag to the fd
    this->set_NonBlocking(fd);    
    
    // Something (?) went wrong, new connection could not be handled
    if (fd == -1) {
        std::cerr << "@log servercore: Something went wrong, new connection could not be handled (Maybe server too busy, too many connections?)" << std::endl;
        try {
            close(fd);
        } catch (std::exception e) {
            std::cerr << e.what() << std::endl;
        }
        return (EXIT_FAILURE); // Return at this point
    }
    
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuseAllowed, sizeof(reuseAllowed)) < 0) { //  enable reuse of socket, even when it is still occupied
        std::cerr << "@log servercore: setsockopt() failed" << std::endl;
        close (fd);
        return EXIT_FAILURE;
    }

    // Get the client IP address
    this->addrLength = sizeof this->addrStorage;
    getpeername(fd, (struct sockaddr*) &this->addrStorage, &(this->addrLength));
    
    if (this->addr.sin_family == AF_INET) {
        struct sockaddr_in* fd = (struct sockaddr_in*) &(this->addrStorage);
        port = ntohs(fd->sin_port);
        inet_ntop(AF_INET, &fd->sin_addr, ipstr, sizeof ipstr);
        hostId = (std::string)ipstr;
    }

    printf("@log servercore: Connection accepted: FD=%d - Slot=%d - Id=%d \n", fd, (this->connections.size()+1), ++(this->connId));
    
    //update highSock 
    this->highSock = (fd > this->highSock) ? fd:this->highSock;
    
    // The new connection 
    Connection* conn;
    
    if (USE_SSL){
        conn = new Connection(fd, this->sslConn ,this->connId, this->dir, hostId, true, this->commandOffset); 
        std::cout << "@log servercore: use SSL model " << std::endl;
    }
    else{ 
        conn = new Connection(fd, this->sslConn, this->connId, this->dir, hostId, false, this->commandOffset); 
        std::cout << "@log servercore: use non-SSL model " << std::endl;
    }    
    
    conn->TLS_handshark();
    this->connections.push_back(conn);
    return (EXIT_SUCCESS);
}

void 
servercore::handle_Main_Connection(Connection* & conn)
{
    Session*        ses;
    unsigned int    id;
    TOKEN           token;
    if ( !conn->get_authen_state() ) {
        //if not authen connection 
        if ( conn->authConnection(this->listUser) ) {
            //if check auth success
            conn->set_authen_state(true);
            id      = conn->get_Connection_Id();
            ses     = conn->get_Session();
            token   = std::make_pair(id,ses);
            this->_listSession.pb(token);
        } else {
            //if check auth fail
            conn->set_Close_Request_Status(true);
        }   
        conn->respondAuthen();
        conn->set_Close_Request_Status(true); //close connection after response success login
    } else {
        //if this connection authenticated -> handle data commining
        std::cout << "@log servercore: main connection establish $$$$$" << std::endl;
        //conn->getAllData();
        //this->connections.at(index)->respondToQuery();
    }
}
    
void 
servercore::handle_File_Connection(Connection* & conn)
{
    std::cout << "@log servercore: handle file connection!!!" << std::endl;
    if (!conn->get_isUploadConnection())
        conn->handle_uploadRequest(this->_listSession);
    else {
        //conn->respondToQuery();
        conn->wirte_Data();
        if (conn->get_Data_Write_Done()){
            conn->handle_CMD_MSG_FILE();
        }
    }
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
// Something is happening (=data ready to read) at a socket, either accept a new connection or handle the incoming data over an already opened socket
void 
servercore::read_Sockets() 
{
    // accept connection. <TCP handshark + TLS handshark>
    if (FD_ISSET(this->Mainsocket,&(this->working_set))) {
        std::cout << "@log servercore: new connection " << std::endl;
        if (this->handle_New_Connection() == EXIT_FAILURE ){
            std::cerr << "@log servercore: error handle new connection!!!!" << std::endl;
            return;
        }  
    }
    
    // Now check connectlist for available data
    // Run through our sockets and check to see if anything happened with them
    for (unsigned int index = 0; index < this->connections.size(); ++index) {
        if (FD_ISSET(this->connections.at(index)->getFD(), &(this->working_set))) {
            std::cout << "@log servercore: handle comming data to exist socket " << this->connections.at(index)->getFD() << std::endl;
            
            //handle new connection to mainsocket, maybe is mainconnection or fileconnection
            //need to classify is mainconnection or fileconnection 
            if ( !this->connections.at(index)->get_isMainConnection() && !this->connections.at(index)->get_isFileConnection())
            {
                std::cout << "@log servercore: handle new connection" << std::endl;
                this->connections.at(index)->classify_connection();
                continue;
            }
            
            //data comming to socket main connection
            //handle cmd 
            if ( this->connections.at(index)->get_isMainConnection() ){
                std::cout << "@log servercore: handle main connection" << std::endl;
                this->handle_Main_Connection(this->connections.at(index));
                continue;
            }
            
            //data comming to file socket.
            //handle read/write file
            if ( this->connections.at(index)->get_isFileConnection() ){
                std::cout << "@log servercore: handle file connection" << std::endl;
                this->handle_File_Connection(this->connections.at(index));
                continue;
            }

        }
    }
}
  
/*
 * Server entry point and main loop accepting and handling connections
 */ 
int 
servercore::start() 
{ 
    int             readworking_set = -1; // Number of sockets ready for reading
    struct timeval  timeout, working_timeout;
    timeout.tv_sec  = 3; // Timeout = 3 sec
    timeout.tv_usec = 0;

    while (!this->shutdown) {
        std::cout << "@log servercore: waiting connection form client....." << std::endl;

        this->build_Select_List(); // Clear out data handled in the previous iteration, clear closed sockets
        // Multiplexes between the existing connections regarding to data waiting to be processed on that connection (that's actually what select does)
        working_timeout = timeout;
        
        readworking_set = select(this->highSock+1, &(this->working_set), NULL , NULL , &working_timeout);

        if (readworking_set < 0) {
            std::cerr << "@log servercore: Error calling select" << std::endl;
            return (EXIT_FAILURE);
        }
            
        this->read_Sockets(); // Handle the sockets (accept new connections or handle incoming data or do nothing [if no data])
    }
    return (EXIT_SUCCESS);
}

/* 
 * Sets the given socket to non-blocking mode
 * @sock parameter reference to set socket non-blocking
 */
void 
servercore::set_NonBlocking(int &sock) 
{
    int opts = fcntl(sock,F_GETFL, 0);
    if (opts < 0) {
        std::cerr << "@log servercore: Error getting socket flags" << std::endl;
        return;
    }
    opts = (opts | O_NONBLOCK);
    if (fcntl(sock,F_SETFL,opts) < 0) {
        std::cerr << "@log servercore: Error setting socket to non-blocking" << std::endl;
        return;
    }
}

/* 
 * Initialization of sockets / socket list with options and error checking
 * @port create socket to listening 
 */ 
int 
servercore::init_Sockets(int port) 
{
    int reuseAllowed            = 1; 
    this->maxConnectionsInQuery = 500; //set maximum connect to server simultaneously
    this->addr.sin_family       = AF_INET; // PF_INET;
    this->addr.sin_port         = htons(port); 
    this->addr.sin_addr.s_addr  = INADDR_ANY; // Server can be connected to from any host
    // PF_INET: domain, Internet; SOCK_STREAM: datastream, TCP / SOCK_DGRAM = UDP => WARNING, this can change the byte order!; for 3rd parameter==0: TCP preferred
    
    this->Mainsocket = socket(PF_INET, SOCK_STREAM, 0); 

    if (this->Mainsocket == -1) {
        std::cerr << "@log servercore: socket() failed" << std::endl;
        return FALSE;
    }
    else if (setsockopt(this->Mainsocket, SOL_SOCKET, SO_REUSEADDR, &reuseAllowed, sizeof(reuseAllowed)) < 0) { //  enable reuse of socket, even when it is still occupied
        std::cerr << "@log servercore: setsockopt() failed" << std::endl;
        close (this->Mainsocket);
        return FALSE;
    }
    
    this->set_NonBlocking(this->Mainsocket);
    
    if (bind(this->Mainsocket, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        std::cerr << ("@log servercore: bind() failed (do you have the apropriate rights? is the port unused?)") << std::endl;
        close (this->Mainsocket);
        return FALSE;
    } // 2nd parameter (backlog): number of connections in query, can be also set SOMAXCONN
    else if (listen(this->Mainsocket, this->maxConnectionsInQuery) == -1) {
        std::cerr << ("@log servercore: listen () failed") << std::endl;
        close (this->Mainsocket);
        return FALSE;
    }
    this->highSock = this->Mainsocket; // This is the first (and the main listening) socket
    std::cout << "@log servercore: Server started and listening at port " << port << ", default server directory '" << this->dir << "'" << ((this->commandOffset == 3) ? ", for use with telnet" : "")  << std::endl;
    
    return TRUE;
}
