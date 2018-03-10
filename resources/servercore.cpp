/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../header/servercore.h"

servercore::servercore(uint port, std::string dir, unsigned short commandOffset) 
                        : dir(dir), commandOffset(commandOffset), _shutdown(false), connId(0) 
{
    this->_serverTimeout.tv_sec     = 3;
    this->_serverTimeout.tv_usec    = 0;

    this->_connections.clear();
    this->_listUser.clear();
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
    this->start();
}

/* 
 * Free up used memory by cleaning up all the object variables;
 */ 
servercore::~servercore() 
{
    std::cout << "@log servercore: Server shutdown" << std::endl;
    close(this->_mainSocket); 
    this->free_All_Connections(); // Deletes all connection objects and frees their memory
    delete this->sslConn;
    delete this->_database;
}


void            
servercore::build_Select_list_For_Main_Connection()
{
    FD_ZERO(&(this->_mainConnSet));
    this->_highestFdMainSet = 0;
    std::vector<Connection*>::iterator _iter = this->_mainConnections.begin();

    while (_iter != this->_mainConnections.end()){
        if ( (*_iter)->get_Close_Request_Status() ){
            std::cout << "@log servercore: Connection with Id " << (*_iter)->getConnectionId() << " closed! " << std::endl;
            delete (*_iter);
            this->_mainConnections.erase(_iter);
            if ( this->_mainConnections.empty() || _iter == this->_mainConnections.end()){
                return;
            }
        } else {
            int _currentFD = (*_iter)->getFD();
            if (_currentFD != 0) {
                FD_SET(_currentFD, &(this->_mainConnSet)); 
                if (_currentFD > this->_highestFdMainSet)
                    this->_highestFdMainSet = _currentFD; 
            }
            ++_iter;
        }
    }
}

void            
servercore::build_Select_list_For_File_Connection()
{
    FD_ZERO(&(this->_fileConnSet));
    this->_highestFdFileSet = 0;
    std::vector<Connection*>::iterator _iter = this->_fileConnections.begin();

    while (_iter != this->_fileConnections.end()){
        if ( (*_iter)->get_Close_Request_Status() ){
            std::cout << "@log servercore: Connection with Id " << (*_iter)->getConnectionId() << " closed! " << std::endl;
            delete (*_iter);
            this->_fileConnections.erase(_iter);
            if ( this->_fileConnections.empty() || _iter == this->_fileConnections.end()){
                return;
            }
        } else {
            int _currentFD = (*_iter)->getFD();
            if (_currentFD != 0) {
                FD_SET(_currentFD, &(this->_fileConnSet));
                if (_currentFD > this->_highestFdFileSet)
                    this->_highestFdFileSet = _currentFD;
            }
            ++_iter;
        }
    }
}


// @NOTE: donot delete memory of connection push to list main or file connecions.
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
            std::cout << "@log servercore: Connection with Id " << (*_iter)->getConnectionId() << " closed! " << std::endl;
            delete (*_iter);
            this->_connections.erase(_iter);
            if ( this->_connections.empty() || _iter == this->_connections.end()){
                return;
            }
        } else if ((*_iter)->get_Is_Classified()){
            std::cout << "@log servercore: Connection with Id " << (*_iter)->getConnectionId() << " classified and move to another list" << std::endl;
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

// Builds the list of sockets to keep track on and removes the closed ones
// @TODO: Crash if data is incoming over a closed socket connection???
void 
servercore::build_Select_List() 
{
    FD_ZERO(&(this->_workingSet));
    FD_SET(this->_mainSocket, &(this->_workingSet));
    
    std::vector<Connection*>::iterator _iter = this->_connections.begin();
    
    while( _iter != this->_connections.end() ) {
        // This connection was closed, flag is set -> remove its corresponding object and free the memory
        if ( (*_iter)->get_Close_Request_Status() ) { 
            std::cout << "@log servercore: Connection with Id " << (*_iter)->getConnectionId() << " closed! " << std::endl;
            delete (*_iter); // Clean up
            this->_connections.erase(_iter); // Delete it from our vector
            if ( this->_connections.empty() || (_iter == this->_connections.end()) )
                return; // Don't increment the iterator when there is nothing to iterate over - avoids crash
        } else {
            int currentFD = (*_iter)->getFD();
            if (currentFD != 0) {
                FD_SET(currentFD, &(this->_workingSet)); // Adds the socket file descriptor to the monitoring for select
                if (currentFD > this->highSock)
                    this->highSock = currentFD; // We need the highest socket for select
            }
            ++_iter; // Increment iterator
        }
        //++iter; // Increment iterator
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

// Accepts new connections and stores the connection object with fd in a vector
int 
servercore::handle_New_Connection()
{
    int             _fd; // Socket file descriptor for incoming connections
    char            ipstr[INET6_ADDRSTRLEN];
    int             port;
    std::string     hostId          = "";
    int             reuseAllowed    = 1;
    this->cli_size                  = sizeof(this->cli);
    _fd                             = accept(this->_mainSocket, (struct sockaddr*) &this->cli, &this->cli_size);
    
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
    
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &reuseAllowed, sizeof(reuseAllowed)) < 0) { //  enable reuse of socket, even when it is still occupied
        std::cerr << "@log servercore: setsockopt() failed" << std::endl;
        close (_fd);
        return EXIT_FAILURE;
    }

    // Get the client IP address
    this->addrLength = sizeof this->addrStorage;
    getpeername(_fd, (struct sockaddr*) &this->addrStorage, &(this->addrLength));
    
    if (this->addr.sin_family == AF_INET) {
        struct sockaddr_in* _fd = (struct sockaddr_in*) &(this->addrStorage);
        port = ntohs(_fd->sin_port);
        inet_ntop(AF_INET, &_fd->sin_addr, ipstr, sizeof ipstr);
        hostId = (std::string)ipstr;
    }

    printf("@log servercore: Connection accepted: FD=%d - Slot=%d - Id=%d \n", _fd, (this->_connections.size()+1), ++(this->connId));
    
    //update highSock 
    this->highSock = (_fd > this->highSock) ? _fd:this->highSock;
    
    // The new connection 
    Connection* conn;
    
    if (USE_SSL){
        conn = new Connection(_fd, this->sslConn ,this->connId, this->dir, hostId, true, this->commandOffset); 
        std::cout << "@log servercore: use SSL model " << std::endl;
    }
    else{ 
        conn = new Connection(_fd, this->sslConn, this->connId, this->dir, hostId, false, this->commandOffset); 
        std::cout << "@log servercore: use non-SSL model " << std::endl;
    }    
    
    conn->TLS_handshark();
    this->_connections.push_back(conn);
    return (EXIT_SUCCESS);
}

void 
servercore::handle_Main_Connection(Connection* & _conn)
{
    Session*        _ses;
    unsigned int    _id;
    TOKEN           _token;
    std::string     _usernameOfConnection;
    int             _idFileTransaction;
    if ( !_conn->get_authen_state() ) {
        //if not authen connection 
        if ( _conn->authConnection(this->_listUser) ) {
            //if check auth success
            _conn->set_authen_state(true);
            _id                     = _conn->get_Connection_Id();
            _ses                    = _conn->get_Session();
            _usernameOfConnection   = _conn->get_Username_Of_Connection();
            _token                  = std::make_pair(_id,_ses);
            this->_listSession.pb(_token);
            _idFileTransaction      = this->check_File_Transaction(_usernameOfConnection);
            if (_idFileTransaction < 0){
                
            } 
        }    
        _conn->respondAuthen();
        _conn->set_Close_Request_Status(true); //close connection after response success login
    } else {
        //if this connection authenticated -> handle data commining
        std::cout << "@log servercore: main connection establish $$$$$" << std::endl;
        //conn->getAllData();
        //this->connections.at(index)->respondToQuery();
    }
}

int
servercore::check_File_Transaction(std::string _receiver)
{
    rep(index,_listFileTransaction.size()){
        if (_listFileTransaction.at(index)->_receiver == _receiver)
            return index;
    }
    return -1;
}

void 
servercore::handle_File_Connection(Connection* & _conn)
{
    std::cout << "@log servercore: handle file connection!!!" << std::endl;
    if (!_conn->get_isUploadConnection())
        _conn->handle_uploadRequest(this->_listSession);
    else {
        //conn->respondToQuery();
        _conn->wirte_Data();
        if (_conn->get_Data_Write_Done()){
            FILE_TRANSACTION * _ft;
            _ft = _conn->handle_CMD_MSG_FILE();
            if (_ft != NULL){
                std::cout << "@log servercore: add file transsaction completed" << std::endl;
                this->_listFileTransaction.emplace_back(_ft);
            }
        }
    }
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
// Something is happening (=data ready to read) at a socket, either accept a new connection or handle the incoming data over an already opened socket
void 
servercore::read_Sockets() 
{
    // accept connection. <TCP handshark + TLS handshark>
    if (FD_ISSET(this->_mainSocket,&(this->_workingSet))) {
        std::cout << "@log servercore: new connection " << std::endl;
        if (this->handle_New_Connection() == EXIT_FAILURE ){
            std::cerr << "@log servercore: error handle new connection!!!!" << std::endl;
            return;
        }  
    }
    
    // Now check connectlist for available data
    // Run through our sockets and check to see if anything happened with them
    for (unsigned int index = 0; index < this->_connections.size(); ++index) {
        if (FD_ISSET(this->_connections.at(index)->getFD(), &(this->_workingSet))) {
            std::cout << "@log servercore: handle comming data to exist socket " << this->_connections.at(index)->getFD() << std::endl;
            
            //handle new connection to _mainSocket, maybe is mainconnection or fileconnection
            //need to classify is mainconnection or fileconnection 
            if ( !this->_connections.at(index)->get_isMainConnection() && !this->_connections.at(index)->get_isFileConnection())
            {
                std::cout << "@log servercore: handle new connection" << std::endl;
                this->_connections.at(index)->classify_connection();
                continue;
            }
            
            //data comming to socket main connection
            //handle cmd 
            if ( this->_connections.at(index)->get_isMainConnection() ){
                std::cout << "@log servercore: handle main connection" << std::endl;
                this->handle_Main_Connection(this->_connections.at(index));
                continue;
            }
            
            //data comming to file socket.
            //handle read/write file
            if ( this->_connections.at(index)->get_isFileConnection() ){
                std::cout << "@log servercore: handle file connection" << std::endl;
                this->handle_File_Connection(this->_connections.at(index));
                continue;
            }

        }
    }
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
        if (FD_ISSET(this->_connections.at(_index)->getFD(), &(this->_workingSet))) {
            std::cout << "@log servercore: handle comming data to exist socket " << this->_connections.at(_index)->getFD() << std::endl;
            
            if ( !this->_connections.at(_index)->get_isMainConnection() && !this->_connections.at(_index)->get_isFileConnection())
            {
                std::cout << "@log servercore: handle new connection" << std::endl;
                this->_connections.at(_index)->classify_connection();
            }
            
            if ( this->_connections.at(_index)->get_isMainConnection() ){
                std::cout << "@log servercore: push this connecion to MAIN connections list" << std::endl;
                this->_mainConnections.emplace_back(this->_connections.at(_index));
            }
            
            if ( this->_connections.at(_index)->get_isFileConnection() ){
                std::cout << "@log servercore: push this connecion to FILE connections list" << std::endl;
                this->_fileConnections.emplace_back(this->_connections.at(_index));
            }

        }
    }
}

void            
servercore::thread_Main_Connecion_Handle()
{
    return;
}   
    
void            
servercore::thread_File_Connecion_Handle()
{
    return;
}
  
/*
 * Server entry point and main loop accepting and handling connections
 */ 

int 
servercore::start() 
{ 
    int             _readWorkingSet = -1; // Number of sockets ready for reading
    struct timeval  _timeout, _time;
    _timeout.tv_sec     = 3; // Timeout = 3 sec
    _timeout.tv_usec    = 0;

    while (!this->_shutdown) {
        std::cout << "@log servercore: waiting connection form client....." << std::endl;

        this->build_Select_List(); // Clear out data handled in the previous iteration, clear closed sockets
        // Multiplexes between the existing connections regarding to data waiting to be processed on that connection (that's actually what select does)
        _time = _timeout;
        
        _readWorkingSet = select(this->highSock+1, &(this->_workingSet), NULL , NULL , &_time);

        if (_readWorkingSet < 0) {
            std::cerr << "@log servercore: Error calling select" << std::endl;
            return (EXIT_FAILURE);
        }
            
        this->read_Sockets(); // Handle the sockets (accept new connections or handle incoming data or do nothing [if no data])
    }
    return (EXIT_SUCCESS);
}


int             
servercore::start_Server()
{
    int                 _rc;
    struct timeval      _time;



    while (!this->_shutdown) {
        std::cout << "@log servercore: waiting connections form client....." << std::endl;

        this->build_Select_List_For_Connections();

        _time   = this->_serverTimeout;

        _rc     = select(this->_highestFdConnSet+1, &(this->_connectionsSet), NULL, NULL, &_time);

        if (_rc < 0){
            std::cerr << "@log servercore: Error calling select()" << std::endl;
            return (EXIT_FAILURE);
        }

        //this->handle_Connection();    
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
    this->_maxConnectionsInQuery = 500; //set maximum connect to server simultaneously
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
