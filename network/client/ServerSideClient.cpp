#include "ServerSideClient.hpp"
#include <network/server/NetworkServerBase.hpp>

ServerSideClient::ServerSideClient( std::weak_ptr<NetworkServerBase> server, std::shared_ptr<boost::asio::io_service> service, int clientID ) :
     NetworkClientBase( service,  clientID ), _server{ server } {
}

ServerSideClient::~ServerSideClient() {}

int ServerSideClient::_getClientID() {
    return NetworkClientBase::_getClientID();
}

void ServerSideClient::_startClient() {
    _startTransfer();
    _startTimers();
}

void ServerSideClient::_stopClient() {
    BOOST_LOG(lg) << Converters::netTransferClientIDToString( _getClientID() ) << ": " << "Stopping...";
    
    _stopTimers();
    _disconnectClient();
}

void ServerSideClient::_sendMessage( NetworkMessage netMsg ) {
    NetworkClientBase::_sendMessage( netMsg );
}

std::shared_ptr<boost::asio::ip::tcp::socket> ServerSideClient::_getSocket() {
    return NetworkClientBase::_getSocket();
}

void ServerSideClient::_managementLoop() {
    // Stop client if deadline timer has expired
    if ( _isReconnectRespiteExpired() ) {
        BOOST_LOG(lg) << Converters::netTransferClientIDToString( _getClientID() ) << ": " << "Reconnection respite expired!";
        _stopClient();
    }
}

void ServerSideClient::_initialLoop() {
    if ( auto server = _server.lock() )
        server->initMessage( shared_from_base<ServerSideClient>() );
}

void ServerSideClient::_confirmTransfer( NetworkMessage netMsg ) {
    if ( auto server = _server.lock() )
        server->confirmTransfer( shared_from_base<ServerSideClient>(), netMsg );
}

void ServerSideClient::_processMessage( NetworkMessage netMsg ) {
    if ( auto server = _server.lock() )
        server->processMessage( shared_from_base<ServerSideClient>(), netMsg );
}

void ServerSideClient::_clientDisconnected() {
    if ( auto server = _server.lock() )
        server->clientDisconnected( shared_from_base<ServerSideClient>() );
}
