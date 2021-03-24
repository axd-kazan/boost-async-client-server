#include "ClientSideBase.hpp"

ClientSideBase::ClientSideBase() :
    _work{ std::make_shared<boost::asio::io_service::work>( *_getService() ) },
    _resolver{ std::make_shared<boost::asio::ip::tcp::resolver>( *_getService() ) }
{}

ClientSideBase::~ClientSideBase() {}

void ClientSideBase::startClient() {
    clientThread = std::thread( std::bind( &ClientSideBase::_startService, shared_from_base<ClientSideBase>() ));
}

void ClientSideBase::_startService() {
    BOOST_LOG(lg) << Converters::netTransferClientIDToString( _getClientID() ) << ": " << "Starting service...";

    connect();
    _getService()->run();
}

void ClientSideBase::stopClient() {
    BOOST_LOG(lg) << Converters::netTransferClientIDToString( _getClientID() ) << ": " << "Stopping...";

    _getService()->post(std::bind(&ClientSideBase::_stopClient, shared_from_base<ClientSideBase>()));
    clientThread.join();
}

void ClientSideBase::_stopClient() {
    _work.reset();
    _stopTimers();
    _disconnectClient();
}

void ClientSideBase::connect() {
    BOOST_LOG(lg) << Converters::netTransferClientIDToString(_getClientID()) << ": " << "Connecting...";
    _getService()->post( std::bind( &ClientSideBase::_startConnecting, shared_from_base<ClientSideBase>() ));
    _getService()->post( std::bind( &ClientSideBase::_startTimers, shared_from_base<ClientSideBase>() ));
}

void ClientSideBase::disconnect() {
    BOOST_LOG(lg) << Converters::netTransferClientIDToString(_getClientID()) << ": " << "Disconnecting...";
    _getService()->post(std::bind(&ClientSideBase::_stopTimers, shared_from_base<ClientSideBase>()));
    _getService()->post(std::bind(&ClientSideBase::_disconnectClient, shared_from_base<ClientSideBase>()));
}

void ClientSideBase::sendMessage( unsigned int messageType, std::vector<std::shared_ptr<std::vector<uint8_t>>> rawData ) {
    NetworkMessage netMsg( MessageHeader( messageType ), rawData, NetConst::ClientID::ClientSide );
    _getService()->post( std::bind( &ClientSideBase::_sendMessage, shared_from_base<ClientSideBase>(), netMsg ));
}

void ClientSideBase::sendMessage( unsigned int messageType, std::shared_ptr<std::vector<uint8_t>> rawData ) {
    NetworkMessage netMsg( MessageHeader( messageType ), rawData, NetConst::ClientID::ClientSide );
    _getService()->post( std::bind( &ClientSideBase::_sendMessage, shared_from_base<ClientSideBase>(), netMsg));
}

void ClientSideBase::sendMessage(unsigned int messageType) {
    NetworkMessage netMsg( MessageHeader( messageType ), NetConst::ClientID::ClientSide );
    _getService()->post( std::bind( &ClientSideBase::_sendMessage, shared_from_base<ClientSideBase>(), netMsg));
}

void ClientSideBase::sendMessage(unsigned int messageType, unsigned long long dataSize, uint8_t *rawData) {
    NetworkMessage netMsg( MessageHeader( messageType ), dataSize, rawData, NetConst::ClientID::ClientSide );
    _getService()->post( std::bind( &ClientSideBase::_sendMessage, shared_from_base<ClientSideBase>(), netMsg));
}

void ClientSideBase::setServerAddress( std::pair<std::string, std::string> serverAddress ) {
    std::lock_guard<std::mutex> guard( mtx );
    _serverAddress = serverAddress;

    BOOST_LOG(lg) << Converters::netTransferClientIDToString( _getClientID() ) << ": " << "Server address setted " << serverAddress.first << ":" << serverAddress.second;
}

// Connect to remote server
void ClientSideBase::_startConnecting() {
    if ( !_isConnected() ) {
        boost::system::error_code err; // Error variable for checking address type (IP or domain name)

        std::lock_guard<std::mutex> guard( mtx );

        _getSocket().reset( new boost::asio::ip::tcp::socket( *_getService() )); // Reinitialize socket

        boost::asio::ip::address servAddress = boost::asio::ip::address::from_string( _serverAddress.first, err );

        // IP address
        if ( !err )
            _endPoint = boost::asio::ip::tcp::endpoint( servAddress, static_cast<uint16_t>( stoi( _serverAddress.second )));
        // Domain name
        else {
            boost::asio::ip::tcp::resolver::query query( boost::asio::ip::tcp::v4(), _serverAddress.first, _serverAddress.second );
            _endPoint = _resolver->resolve( query )->endpoint();
        }

        BOOST_LOG(lg) << Converters::netTransferClientIDToString( _getClientID() ) << ": " << "Connecting to " << _serverAddress.first << ":" << _serverAddress.second << "...";

        // Connect to server        
        _getSocket()->async_connect( _endPoint, bind( &ClientSideBase::_connectionHandler, shared_from_base<ClientSideBase>(), std::placeholders::_1 ));
    }
}

void ClientSideBase::_connectionHandler( const boost::system::error_code& err) {
    if ( err ) {
        BOOST_LOG(lg) << Converters::netTransferClientIDToString( _getClientID() ) << ": " << "Connecting process interrupted!";
        return;
    }

    _startTransfer();
}

void ClientSideBase::_managementLoop() {
    // Try to reconnect if deadline timer has expired
    if ( _isReconnectRespiteExpired() ) {
        BOOST_LOG(lg) << Converters::netTransferClientIDToString( _getClientID() ) << ": " << "Reconnection respite expired!";

        _disconnectClient();
        _startConnecting();
    }
}
