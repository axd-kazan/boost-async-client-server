#include "NetworkServerBase.hpp"

NetworkServerBase::NetworkServerBase() :
    _work{ std::make_shared<boost::asio::io_service::work>( *_service ) }
{}

NetworkServerBase::~NetworkServerBase() {}

void NetworkServerBase::startServer() {
    serverThread = std::thread( std::bind( &NetworkServerBase::_startService, shared_from_this() ));
}

void NetworkServerBase::stopServer() {
    BOOST_LOG(lg) << "Server: " << "Stopping...";
    _service->post( std::bind( &NetworkServerBase::_stopServer, shared_from_this() ));
    serverThread.join();
}

void NetworkServerBase::stopClient( int clientID ) {
    _service->post( std::bind( &NetworkServerBase::_stopClient, shared_from_this(), clientID ));
}

void NetworkServerBase::_stopClient( int clientID ) {
    std::list<std::shared_ptr<ServerSideClient>>::iterator client  = std::find_if( _clientsList.begin(), _clientsList.end(), [clientID]( const std::shared_ptr<ServerSideClient> &client ) {
        return client->_getClientID() == clientID; });

    if ( client != _clientsList.end() )
        (*client)->_stopClient();
}

void NetworkServerBase::_stopServer() {
    _stopTimers();
    _acceptor.reset();

    for ( std::shared_ptr<ServerSideClient> &client : _clientsList )
        client->_stopClient();
    _clientsList.clear();
    clientsCount.store( 0 );

    _work.reset();
}

// Send message to all clients
void NetworkServerBase::sendBroadMessage( unsigned int messageType, std::vector<std::shared_ptr<std::vector<uint8_t>>> rawData ) {
    NetworkMessage netMsg( MessageHeader( messageType ), rawData, NetConst::ClientID::ServerSide );
    _service->post( std::bind( &NetworkServerBase::_sendMessage, shared_from_this(), netMsg ));
}

void NetworkServerBase::sendBroadMessage( unsigned int messageType, std::shared_ptr<std::vector<uint8_t>> rawData ) {
    NetworkMessage netMsg( MessageHeader( messageType ), rawData, NetConst::ClientID::ServerSide );
    _service->post( std::bind( &NetworkServerBase::_sendMessage, shared_from_this(), netMsg ));
}

void NetworkServerBase::sendBroadMessage( unsigned int messageType ) {
    NetworkMessage netMsg( MessageHeader( messageType ), NetConst::ClientID::ServerSide );
    _service->post( std::bind( &NetworkServerBase::_sendMessage, shared_from_this(), netMsg ));
}

void NetworkServerBase::sendBroadMessage( unsigned int messageType, unsigned long long dataSize, uint8_t *rawData ) {
    NetworkMessage netMsg( MessageHeader( messageType ), dataSize, rawData, NetConst::ClientID::ServerSide );
    _service->post( std::bind( &NetworkServerBase::_sendMessage, shared_from_this(), netMsg ));
}

// Send message to selected client
void NetworkServerBase::sendUniMessage( int clientID, unsigned int messageType, std::vector<std::shared_ptr<std::vector<uint8_t>>> rawData ) {
    NetworkMessage netMsg( MessageHeader( messageType ), rawData, clientID );
    _service->post( std::bind( &NetworkServerBase::_sendMessage, shared_from_this(), netMsg ));
}

void NetworkServerBase::sendUniMessage( int clientID, unsigned int messageType, std::shared_ptr<std::vector<uint8_t>> rawData ) {
    NetworkMessage netMsg( MessageHeader( messageType ), rawData, clientID );
    _service->post( std::bind( &NetworkServerBase::_sendMessage, shared_from_this(), netMsg ));
}

void NetworkServerBase::sendUniMessage( int clientID, unsigned int messageType ) {
    NetworkMessage netMsg( MessageHeader( messageType ), clientID );
    _service->post( std::bind( &NetworkServerBase::_sendMessage, shared_from_this(), netMsg ));
}

void NetworkServerBase::sendUniMessage( int clientID, unsigned int messageType, unsigned long long dataSize, uint8_t *rawData ) {
    NetworkMessage netMsg( MessageHeader( messageType ), dataSize, rawData, clientID );
    _service->post( std::bind( &NetworkServerBase::_sendMessage, shared_from_this(), netMsg ));
}

void NetworkServerBase::_sendMessage(NetworkMessage netMsg) {
    // Send message to all clients
    if ( netMsg._getClientID() == NetConst::ClientID::ServerSide ) {
        for ( std::list<std::shared_ptr<ServerSideClient>>::iterator it = _clientsList.begin(); it != _clientsList.end(); ++it )
            (*it)->_sendMessage( netMsg );
    }

    // Send message to defined client
    if (netMsg._getClientID() > 0) {
        for ( std::list<std::shared_ptr<ServerSideClient>>::iterator it = _clientsList.begin(); it != _clientsList.end(); ++it )
            if ( (*it)->_getClientID() == netMsg._getClientID() ) {
                (*it)->_sendMessage( netMsg );
                break;
            }
    }
}

void NetworkServerBase::_removeClientFromList( std::shared_ptr<ServerSideClient> client ) {
    _clientsList.remove( client );
    clientsCount.store( _clientsList.size() );
}

void NetworkServerBase::initMessage( std::shared_ptr<ServerSideClient> client ) {
    _service->post( std::bind( &NetworkServerBase::_initMessage, shared_from_this(), client ));
}

void NetworkServerBase::confirmTransfer(std::shared_ptr<ServerSideClient> client, NetworkMessage netMsg) {
    _service->post( std::bind( &NetworkServerBase::_confirmTransfer, shared_from_this(), client, netMsg ));
}

void NetworkServerBase::processMessage( std::shared_ptr<ServerSideClient> client, NetworkMessage netMsg ) {
    _service->post( std::bind( &NetworkServerBase::_processMessage, shared_from_this(), client, netMsg ));
}

void NetworkServerBase::clientDisconnected(std::shared_ptr<ServerSideClient> client) {
    _service->post( std::bind( &NetworkServerBase::_removeClientFromList, shared_from_this(), client ));
    _service->post( std::bind( &NetworkServerBase::_clientDisconnected, shared_from_this(), client ));
}

void NetworkServerBase::_startListen() {
    mtx.lock();
    _acceptor = std::make_shared<boost::asio::ip::tcp::acceptor>( *_service, boost::asio::ip::tcp::endpoint{ boost::asio::ip::tcp::v4(), port.load() } );
    mtx.unlock();

    std::shared_ptr<ServerSideClient> newClient = _makeNewClient();
    _acceptor->async_accept( *newClient->_getSocket(), std::bind( &NetworkServerBase::handleAccept, shared_from_this(), newClient, std::placeholders::_1 ));
    BOOST_LOG(lg) << "Server: Listening...";

    _startTimers();
}

void NetworkServerBase::_startService() {
    _service->post( std::bind( &NetworkServerBase::_startListen, shared_from_this() ));
    _service->run();
}

std::shared_ptr<ServerSideClient> NetworkServerBase::_makeNewClient() {
    static int clientIDCounter{};
    if ( clientIDCounter < 0 ) clientIDCounter = 0;
    clientIDCounter++;

    return std::make_shared<ServerSideClient>( shared_from_this(), _service, clientIDCounter );
}

void NetworkServerBase::handleAccept( std::shared_ptr<ServerSideClient> client, const boost::system::error_code& err ) {
    if ( err ) {
        BOOST_LOG(lg) << "Server: Handle accept error! " << err.message();
    } else {
        BOOST_LOG(lg) << "Server: Handle new connection " << "ClientID = " << client->_getClientID();

        client->_startClient();
        
        _clientsList.push_back( client ); // Add client to server's clients list
        clientsCount.store( _clientsList.size() );
        _clientConnected( client );
    }

    if ( _acceptor && _acceptor->is_open() ) {
        std::shared_ptr<ServerSideClient> newClient = _makeNewClient();
        _acceptor->async_accept( *newClient->_getSocket(), bind( &NetworkServerBase::handleAccept, shared_from_this(), newClient, std::placeholders::_1 ));
    }
}

void NetworkServerBase::setPort( unsigned short port ) {
    this->port.store( port );
    BOOST_LOG(lg) << "Server: Set port " << port;
}

unsigned long long NetworkServerBase::getClientsCount() {
    return clientsCount.load();
}

std::shared_ptr<boost::asio::io_service> NetworkServerBase::_getService() {
    return _service;
}

void NetworkServerBase::_startTimers() {
    _initMessageTimer.reset( new boost::asio::deadline_timer( *_service ));
    _tickInitMsgLoop( _initLoopInterval );
}

void NetworkServerBase::_stopTimers() {
    _initMessageTimer.reset();
}

void NetworkServerBase::_tickInitMsgLoop( unsigned int timer ) {
    if ( _initMessageTimer ) {
        _initMessageTimer->expires_from_now( boost::posix_time::millisec( timer ));
        _initMessageTimer->async_wait( bind( &NetworkServerBase::_initMsgHandler, shared_from_this(), std::placeholders::_1 ));
    }
}

void NetworkServerBase::_initMsgHandler( const boost::system::error_code &err ) {
    if ( err ) {
        BOOST_LOG(lg) << "Server: Initial message timer error! " << err.message();
        return;
    }

    _initServerLoop();
    _tickInitMsgLoop( _initLoopInterval );
}
