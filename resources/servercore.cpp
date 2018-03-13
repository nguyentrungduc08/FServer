/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../header/servercore.h"

servercore::servercore(uint port, std::string dir, unsigned short commandOffset) 
                        : dir(dir), commandOffset(commandOffset), _shutdown(false), _connId(0) 
{
    this->_serverTimeout.tv_sec     = 2;
    this->_serverTimeout.tv_usec    = 0;
    this->_maxConnectionsInQuery    = 500; //set maximum connect to server simultaneously
    
    this->_listUser.clear();
    this->_connections.clear();
    this->_mainConnections.clear();
    this->_fileConnections.clear();
    this->_database = new Database();
    
    if ( !this->_database->doConnection("testuser","testuser","FILE") ){
        std::cerr << "ERROR connecting to database!!!!" << std::endl;
        exit(EXIT_FAILURE);
    } else{
        this->_listUser = this->_database->getListUser();
        rep(i,this->_listUser.size()){
            std::cout << "Id: " << this->_listUser.at(i).id << " Username: " << this->_listUser.at(i).username << " Password: " << this->_listUser.at(i).password << std::endl;
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
    //this->start();
    this->start_Server();
}

/* 
 * Free up used memory by cleaning up all the object variables;
 */ 
servercore::~servercore() 
{
    std::cout << "@log servercore: Server shutdown" << std::endl;
    close(this->_mainSocket); 
    // Deletes all connection objects and frees their memory
    this->free_All_Connections();
    this->free_All_File_Connections();
    this->free_All_Main_Connections();
    this->free_All_File_Transaction();
    
    delete this->sslConn;
    delete this->_database;
}



// @NOTE: do not delete memory of connection push to list main or file connecions.
// connnection add state is classified
void 
servercore::build_Select_List_For_Connections()
{
    FD_ZERO(&(this->_connectionsSet));
    FD_SET(this->_mainSocket, &(this->_connectionsSet));
    this->_highestFdConnSet = this->_mainSocket;
    std::vector<Connection*>::iterator _iter = this->_connections.begin();

    while (_iter != this->_connections.end()){
        if ( (*_iter)->get_Close_Request_Status() ){
            std::cout << "@log servercore: For_Connections Connection with Id " << (*_iter)->getConnectionId() << " closed! " << std::endl;
            delete (*_iter);
            this->_connections.erase(_iter);
            if ( this->_connections.empty() || _iter == this->_connections.end()){
                return;
            }
        } else if ((*_iter)->get_Is_Classified()){
            std::cout << "@log servercore: For_Connections connection with Id " << (*_iter)->getConnectionId() << " classified and move to another list" << std::endl;
            this->_connections.erase(_iter);
            if ( this->_connections.empty() || _iter == this->_connections.end()){
                return;
            }
        } else {
            int _currentFD = (*_iter)->getFD();
            if (_currentFD != 0) {
                FD_SET(_currentFD, &(this->_connectionsSet)); // Adds the socket file descriptor to the monitoring for select
                if (_currentFD > this->_highestFdConnSet)
                    this->_highestFdConnSet = _currentFD; // We need the highest socket for select
            }
            ++_iter;
        }
    }
}

// Clean up everything
void 
servercore::free_All_Connections() 
{
    std::vector<Connection*>::iterator _iter = this->_connections.begin();
    while( _iter != this->_connections.end() ) {
        delete (*(_iter++)); // Clean up, issue destructor implicitly
    }
    this->_connections.clear(); // Delete all deleted connections also from our vector
    
}

void            
servercore::free_All_File_Connections()
{
    std::vector<Connection*>::iterator _iter = this->_fileConnections.begin();
    while( _iter != this->_fileConnections.end() ) {
        delete (*(_iter++)); 
    }
    this->_fileConnections.clear(); 
}
    
void            
servercore::free_All_Main_Connections()
{
    std::vector<Connection*>::iterator _iter = this->_mainConnections.begin();
    while( _iter != this->_mainConnections.end() ) {
        delete (*(_iter++));
    }
    this->_mainConnections.clear(); 
}

void            
servercore::free_All_File_Transaction()
{
    std::vector<FILE_TRANSACTION*>::iterator _iter = this->_listFileTransaction.begin();
    while (_iter != this->_listFileTransaction.end()){
        delete (*(_iter++));
    }
    this->_listFileTransaction.clear();
}

int             
servercore::get_Num_User_Active()
{
    int _numUserAcvite = 0;
    rep(_index,this->_listUser.size()){
        if (this->_listUser.at(_index)._state ){
            ++_numUserAcvite;
        }
    }
    return _numUserAcvite;
}

// Accepts new connections and stores the connection object with fd in a vector
int 
servercore::handle_New_Connection()
{
    int             _fd; // Socket file descriptor for incoming connections
    char            _ipstr[INET6_ADDRSTRLEN];
    int             _port;
    std::string     _hostId          = "";
    int             _reuseAllowed    = 1;
    this->cli_size                   = sizeof(this->cli);
    _fd                              = accept(this->_mainSocket, (struct sockaddr*) &this->cli, &this->cli_size);
    
    if (_fd < 0) {
        std::cerr << "@log servercore: Error while accepting client" << std::endl;
        return (EXIT_FAILURE);
    }

    // Gets the socket fd flags and add the non-blocking flag to the fd
    this->set_NonBlocking(_fd);    
    
    // Something (?) went wrong, new connection could not be handled
    if (_fd == -1) {
        std::cerr << "@log servercore: Something went wrong, new connection could not be handled (Maybe server too busy, too many connections?)" << std::endl;
        try {
            close(_fd);
        } catch (std::exception e) {
            std::cerr << e.what() << std::endl;
        }
        return (EXIT_FAILURE); // Return at this point
    }
    
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &_reuseAllowed, sizeof(_reuseAllowed)) < 0) { //  enable reuse of socket, even when it is still occupied
        std::cerr << "@log servercore: setsockopt() failed" << std::endl;
        close (_fd);
        return EXIT_FAILURE;
    }

    // Get the client IP address
    this->addrLength = sizeof(this->addrStorage);
    getpeername(_fd, (struct sockaddr*) &this->addrStorage, &(this->addrLength));
    
    if (this->addr.sin_family == AF_INET) {
        struct sockaddr_in* _fds = (struct sockaddr_in*) &(this->addrStorage);
        _port = ntohs(_fds->sin_port);
        inet_ntop(AF_INET, &_fds->sin_addr, _ipstr, sizeof _ipstr);
        _hostId = (std::string)_ipstr;
    }

    printf("@log servercore: Connection accepted: FD=%d - Port=%d - Slot=%d - Id=%d \n", _fd, _port, (this->_connections.size()+1), ++(this->_connId));
    
    //update highSock 
    this->highSock = (_fd > this->highSock) ? _fd:this->highSock;
    
    // The new connection 
    Connection* conn;
    
    if (USE_SSL){
        conn = new Connection(_fd, this->sslConn ,this->_connId, this->dir, _hostId, true, this->commandOffset); 
        std::cout << "@log servercore: use SSL model " << std::endl;
    }
    else{ 
        conn = new Connection(_fd, this->sslConn, this->_connId, this->dir, _hostId, false, this->commandOffset); 
        std::cout << "@log servercore: use non-SSL model " << std::endl;
    }    
    
    conn->TLS_handshark();
    //this->_connections.push_back(conn);
    this->_connections.emplace_back(conn);
    return (EXIT_SUCCESS);
}

int
servercore::check_File_Transaction(std::string _receiver)
{
    rep(index,_listFileTransaction.size()){
        if (_listFileTransaction.at(index)->_receiver == _receiver)
            return index;
    }
    return (-1);
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
void            
servercore::read_Data_Main_Socket()
{
    //accept new connection and push this connecion to list connection pending to handle
    //this connection not yet classify 
    if (FD_ISSET(this->_mainSocket, &(this->_connectionsSet))){
        std::cout << "@log servercore: new connection comming" << std::endl;
        if (this->handle_New_Connection() == EXIT_FAILURE ){
            std::cerr << "@log servercore: error handle new connection!!!!" << std::endl;
            return;
        } 
    }
    //classify connections in _connecions list.
    //remove and push it to list main connecion or file connection
    for (unsigned int _index = 0; _index < this->_connections.size(); ++_index) {
        if (FD_ISSET(this->_connections.at(_index)->getFD(), &(this->_connectionsSet))) {
            std::cout << "@log servercore: handle comming data to exist socket " << this->_connections.at(_index)->getFD() << std::endl;
            
            if ( !this->_connections.at(_index)->get_Is_Classified()){
                std::cout << "@log servercore: handle new connection" << std::endl;
                this->_connections.at(_index)->classify_connection();
            }
            
            if ( this->_connections.at(_index)->get_isMainConnection() && this->_connections.at(_index)->get_Is_Classified() ){
                std::cout << "@log servercore: push this connecion to MAIN connections list" << std::endl;
                this->_mainConnections.emplace_back(this->_connections.at(_index));
            }
            
            if ( this->_connections.at(_index)->get_isFileConnection() && this->_connections.at(_index)->get_Is_Classified() ){
                std::cout << "@log servercore: push this connecion to FILE connections list" << std::endl;
                this->_fileConnections.emplace_back(this->_connections.at(_index));
            }
        }
    }
}

int             
servercore::start_Server()
{
    int                 _num_Fd_Incomming;
    struct timeval      _time;

    std::thread         _threadMain(&servercore::thread_Main_Connecion_Handle, this);
    std::thread         _threadFile(&servercore::thread_File_Connecion_Handle, this);

    while (!this->_shutdown) {
        std::cout << "@log servercore: waiting connections form client....." << std::endl;
        //build list set connection for new connection 
        this->build_Select_List_For_Connections();

        _time                 = this->_serverTimeout;

        _num_Fd_Incomming     = select(this->_highestFdConnSet+1, &(this->_connectionsSet), NULL, NULL, &_time);

        if (_num_Fd_Incomming < 0){
            std::cerr << "@log servercore: Error calling select()" << std::endl;
            return (EXIT_FAILURE);
        }

        this->read_Data_Main_Socket();    
    }

    if (_threadMain.joinable()){
        _threadMain.join();
    }

    if (_threadFile.joinable()){
        _threadFile.join();
    }
    return (EXIT_SUCCESS);
}

/* 
 * Sets the given socket to non-blocking mode
 * @sock parameter reference to set socket non-blocking
 */
void 
servercore::set_NonBlocking(int &_sock) 
{
    int _opts = fcntl(_sock,F_GETFL, 0);
    if (_opts < 0) {
        std::cerr << "@log servercore: Error getting socket flags" << std::endl;
        return;
    }
    _opts = (_opts | O_NONBLOCK);
    if (fcntl(_sock,F_SETFL,_opts) < 0) {
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
    this->addr.sin_family       = AF_INET; // PF_INET;
    this->addr.sin_port         = htons(port); 
    this->addr.sin_addr.s_addr  = INADDR_ANY; // Server can be connected to from any host
    // PF_INET: domain, Internet; SOCK_STREAM: datastream, TCP / SOCK_DGRAM = UDP => WARNING, this can change the byte order!; for 3rd parameter==0: TCP preferred
    
    this->_mainSocket = socket(PF_INET, SOCK_STREAM, 0); 

    if (this->_mainSocket == -1) {
        std::cerr << "@log servercore: socket() failed" << std::endl;
        return FALSE;
    }
    else if (setsockopt(this->_mainSocket, SOL_SOCKET, SO_REUSEADDR, &reuseAllowed, sizeof(reuseAllowed)) < 0) { //  enable reuse of socket, even when it is still occupied
        std::cerr << "@log servercore: setsockopt() failed" << std::endl;
        close (this->_mainSocket);
        return FALSE;
    }
    
    this->set_NonBlocking(this->_mainSocket);
    
    if (bind(this->_mainSocket, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        std::cerr << ("@log servercore: bind() failed (do you have the apropriate rights? is the port unused?)") << std::endl;
        close (this->_mainSocket);
        return FALSE;
    } // 2nd parameter (backlog): number of connections in query, can be also set SOMAXCONN
    else if (listen(this->_mainSocket, this->_maxConnectionsInQuery) == -1) {
        std::cerr << ("@log servercore: listen () failed") << std::endl;
        close (this->_mainSocket);
        return FALSE;
    }
    this->highSock = this->_mainSocket; // This is the first (and the main listening) socket
    std::cout << "@log servercore: Server started and listening at port " << port << ", default server directory '" << this->dir << "'" << ((this->commandOffset == 3) ? ", for use with telnet" : "")  << std::endl;
    
    return TRUE;
}
