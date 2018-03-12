/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../header/connection.h"
#include "../header/fileserver.h"
#include "../header/ssl.h"

// Destructor, clean up all the mess
Connection::~Connection() {
    std::cout << "#log conn: Connection terminated to client (connection id " << this->connectionId << ")" << std::endl;
    delete this->fo;
    //delete this->session;
    close(this->_socketFd);
    SSL_free(this->_ssl);    
    //delete this->_ssl; 
    this->directories.clear();
    this->files.clear();
}

/*
 * constructor to handle connection from client.
 * @filedescriptor  fd of socket to get data from client
 * @connid          id of connection
 * @defaulDir       direction working 
 * @hostId          id of host
 * @commandOffset 
 */
Connection::Connection(int filedescriptor,fssl* sslcon, unsigned int connId, 
                                    std::string defaultDir, std::string hostId, bool iSSL, 
                                    unsigned short commandOffset) 
                                    : _socketFd(filedescriptor), connectionId(connId), dir(defaultDir), 
                                    hostAddress(hostId), isSSL(iSSL), commandOffset(commandOffset), 
                                    closureRequested(false), uploadCommand(false), downloadCommand(false),  
                                    _receivedPart(0), _parameter("") 
{
//    this->files = std::vector<std::string>();
    this->session                   = new Session();
    this->fo                        = new FileHandle(this->dir); // File and directory browser
    this->_TLSHandsharkState        = false;
    this->_ConfirmedState           = false;
    this->isMainSocket              = false;
    this->isFileSocket              = false;
    this->_isDownloadConnection     = false;
    this->_isUploadConnection       = false;
    this->_isClassified             = false;
   
    this->timeout.tv_sec            = 3;
    this->timeout.tv_usec           = 0;
    this->_dataWriteDoneState       = false;
    if (iSSL){
        this->_ssl           = SSL_new(sslcon->get_ctx());
        SSL_set_fd(this->_ssl, this->_socketFd);
    }
    
    std::cout << "#log conn: Connection to client '" << this->hostAddress << "' established" << std::endl;
}

// Check for matching (commands/strings) with compare method
bool 
Connection::commandEquals(std::string a, std::string b) {
    // Convert to lower case for case-insensitive checking
    std::transform(a.begin(), a.end(),a.begin(), tolower);
    int found = a.find(b);
    return (found!=std::string::npos);
}

void 
Connection::TLS_handshark() {
    //handle ssl_handshake non-blocking modle
    struct timeval tv, tvRestore;
    int status  = -1;
    tv.tv_sec   = 2;
    tv.tv_usec  = 0;
    tvRestore   = tv;
       
    fd_set writeFdSet;
    fd_set readFdSet;
             
    do {
        tv = tvRestore;
        FD_ZERO(&writeFdSet);
        FD_ZERO(&readFdSet);
        status = SSL_accept(this->_ssl);
        switch (SSL_get_error(this->_ssl, status)){
            case SSL_ERROR_NONE:
                status = 0;
                std::cout << "#log conn: SSL_ERROR_NONE" << std::endl;
                break;
            case SSL_ERROR_WANT_WRITE:
                FD_SET(this->_socketFd, &writeFdSet);
                status = 1;
                std::cout << "#log conn: SSL_ERROR_WANT_WRITE" << std::endl;
                break;
            case SSL_ERROR_WANT_READ:
                FD_SET(this->_socketFd, &readFdSet);
                status = 1;
                std::cout << "#log conn: SSL_ERROR_WANT_READ" << std::endl;
                break;
            case SSL_ERROR_ZERO_RETURN:
                std::cerr << "#log conn: SSL_ERROR_ZERO_RETURN" << std::endl;
                break;
            case SSL_ERROR_SYSCALL:     
                std::cout << "#log conn: Peer closed connection during SSL handshake,status: " << status << std::endl;
                status = -1;
                break;
            default:
                std::cout << "#log conn: Unexpected error during SSL handshake,status: " << status << std::endl;
                status = -1;
                break;
        }
        if (status == 1)
            {              
                // Must have at least one handle to wait for at this point.
                status = select(this->_socketFd + 1, &readFdSet, &writeFdSet, NULL, &tv);

                // 0 is timeout, so we're done.
                // -1 is error, so we're done.
                // Could be both handles set (same handle in both masks) so
                // set to 1.
                if (status >= 1)
                    {
                        status = 1;
                    }
                else // Timeout or failure
                    {
                        std::cout << "#log conn: SSL handshake - peer timeout or failure" << std::endl;
                        status = -1;
                    }
            }
                    
        } while (status == 1 && !SSL_is_init_finished(this->_ssl));
        std::cout << "#log conn: SSL handshark successed" << std::endl;
        //SSL_set_accept_state(this->_ssl);
}

void 
Connection::respond_Classify_Connection_Done(bool state){
    Packet *pk = new Packet();
    
    if (state){
        pk->appendData(CMD_CLASSIFY_DONE);
        SSL_write(this->_ssl, &pk->getData()[0], pk->getData().size());
    } else {
        pk->appendData(CMD_CLASSIFY_FAIL);
        SSL_write(this->_ssl, &pk->getData()[0], pk->getData().size());
    }
    delete pk;
}

void 
Connection::classify_connection(){
    std::cout << "#log conn: Classify connection." << std::endl;
    char        buffer[BUFFER_SIZE];
    int         _bytes = -1;
    Packet*     _pk;
    bzero(buffer, sizeof(buffer));
    
    _bytes = SSL_read(this->_ssl, buffer, sizeof(buffer));
    
    if (_bytes > 0){
        _pk = new Packet(std::string(buffer,_bytes));
        
        int _cmd = _pk->getCMDHeader();
        
        std::cout << "#log conn: recieved data " << _cmd << std::endl;
        
        if (_cmd == CMD_IS_MAIN_CONNECTION) {    
            std::cout << "#log conn: This is main connection." << std::endl;
            this->isMainSocket  = true;
            this->_isClassified = true;
            this->respond_Classify_Connection_Done(true);
            return;
        }
        
        if (_cmd == CMD_IS_FILE_CONNECTION) {    
            std::cout << "#log conn: This is file connection." << std::endl;
            this->isFileSocket  = true;
            this->_isClassified = true;
            this->respond_Classify_Connection_Done(true);
            return;
        }
        delete _pk;
    } 
    std::cout << "#log conn: " << _bytes << std::endl;
    if (!this->_isClassified)
        this->closureRequested  = true;
    this->respond_Classify_Connection_Done(false);
    return;
}

int
Connection::get_CMD_HEADER()
{
    Packet*         _pk;
    int             _num_Fd_Incomming, _bytes, _cmd;
    struct timeval  _time = this->timeout;
    fd_set          _fdset;
    char            buffer[5];
    
    FD_ZERO(&_fdset);
    FD_SET(this->_socketFd, &_fdset);

    _num_Fd_Incomming = select(this->_socketFd+1, &_fdset, NULL, NULL, &_time);

    std::cerr << "log before select " << SSL_get_fd(this->_ssl) << " " << _num_Fd_Incomming << std::endl;

    if (_num_Fd_Incomming == 0){
        std::cerr << "timeout login request connection!!!" << std::endl;
        exit(EXIT_FAILURE);
    }

    bzero(buffer, sizeof(buffer));

    _bytes   = SSL_read(this->_ssl, buffer, 4);
    _pk      = new Packet(std::string(buffer,_bytes));
    
    if (_pk->IsAvailableData())
        _cmd = _pk->getCMDHeader();
    
    std::cout << "Log Connection: size read header " << _bytes  << " - value: " << _cmd << std::endl;
    
    delete _pk;
    return _cmd;
}


FILE_TRANSACTION*           
Connection::handle_CMD_MSG_FILE(){
    char                _buffer[BUFFER_SIZE];
    int                 _bytes,_cmd;
    Packet*             _pk;
    std::string         _sender, _receiver, _urlFile,_filesize;
    FILE_TRANSACTION*   _ft;
    _bytes   = SSL_read(this->_ssl, _buffer, sizeof(_buffer));
    
    if (_bytes > 0){
        _pk = new Packet(std::string(_buffer,_bytes));
        if (_pk->IsAvailableData())
            _cmd        = _pk->getCMDHeader();
        if (_pk->IsAvailableData())
            _sender     = _pk->getContent();
        if (_pk->IsAvailableData())
            _receiver   = _pk->getContent();
        if (_pk->IsAvailableData())
            _urlFile    = _pk->getContent();
        if (_pk->IsAvailableData())
            _filesize   = _pk->getContent();
        std::cout <<"#log conn: msg\ncmd: " << _cmd << "\nsender: " << _sender << "\nreceiver: " << _receiver << "\nurlfile: " << _urlFile <<"\nfile size: " << _filesize << std::endl; 
        _ft = new FILE_TRANSACTION;
        _ft->_sender    = _sender;
        _ft->_receiver  = _receiver;
        _ft->_url       = _urlFile;
        _ft->_filesize  = this->fo->get_File_Size();
        delete _pk;
        return _ft;
    } 
    this->closureRequested = true;
    return NULL;
}

void                        
Connection::Respond_CMD_ERROR()
{
    //send CMD_ERROR
    Packet*     _pk;
    _pk = new Packet();

    _pk->appendData(CMD_ERROR);

    SSL_write(this->_ssl,  &_pk->getData()[0], _pk->getData().size());
    delete _pk;
    return; 
}

// Returns the file descriptor of the current connection
int 
Connection::getFD() {
    return this->_socketFd;
}

// Returns whether the connection was requested to be closed (by client)
bool 
Connection::get_Close_Request_Status() {
    return this->closureRequested;
}

void 
Connection::set_Close_Request_Status(bool status){
    this->closureRequested = status;
}

unsigned int 
Connection::getConnectionId() {
    return this->connectionId;
}

bool 
Connection::get_TLShandshark_state(){
    return this->_TLSHandsharkState;
}

void 
Connection::set_TLShandshark_state(bool state){
    this->_TLSHandsharkState = state;
}

bool 
Connection::get_authen_state(){
    return this->_ConfirmedState;
}

void 
Connection::set_authen_state(bool state){
    this->_ConfirmedState = state;
}

void 
Connection::set_isMainConnection(bool state){
    this->isMainSocket = state;
}

bool 
Connection::get_isMainConnection(){
    return this->isMainSocket;
}
    
void 
Connection::set_isFileConnection(bool state){
    this->isFileSocket = state;
}

bool 
Connection::get_isFileConnection(){
    return this->isFileSocket;
}


void 
Connection::getAllData(){
    char buf[BUFFSIZE];
    SSL_read(this->_ssl, buf, sizeof(buf));
    std::cout <<"#log conn: " << buf << std::endl;
    return;
}

bool
Connection::get_isUploadConnection(){
    return this->_isUploadConnection;
}
    
bool
Connection::get_isDownloadConnection(){
    return this->_isDownloadConnection;
}

std::string                 
Connection::get_Username_Of_Connection(){
    return this->_username;
}
    
int
Connection::get_Usser_Id_Of_Connection(){
    return this->_userID;
}

unsigned int                         
Connection::get_Connection_Id(){
    return this->connectionId;
}

Session*
Connection::get_Session(){
    return this->session;
}       
   
bool                        
Connection::get_Is_Classified()
{
    return this->_isClassified;
};
    
void                        
Connection::set_Is_Classified_State(bool _state)
{
    this->_isClassified = _state;
} 
