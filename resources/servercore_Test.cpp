/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../header/servercore.h"

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
