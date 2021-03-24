#include "NetworkClientBase.hpp"

// Client side
NetworkClientBase::NetworkClientBase() :
    _service{ std::make_shared<boost::asio::io_service>() },
    _socket{ std::make_shared<boost::asio::ip::tcp::socket>( *_service ) },
    _strand{ std::make_shared<boost::asio::io_service::strand>( *_service ) },
    _pingTimer{ std::make_shared<boost::asio::deadline_timer>( *_service ) },
    _initMessageTimer{ std::make_shared<boost::asio::deadline_timer>( *_service ) },
    _managementTimer{ std::make_shared<boost::asio::deadline_timer>( *_service ) },
    _clientID{ NetConst::ClientID::ClientSide }
{}

// Server side
NetworkClientBase::NetworkClientBase( std::shared_ptr<boost::asio::io_service> service, int clientID ) :
    _service{ service },
    _socket{ std::make_shared<boost::asio::ip::tcp::socket>( *_service ) },
    _strand{ std::make_shared<boost::asio::io_service::strand>( *_service ) },
    _pingTimer{ std::make_shared<boost::asio::deadline_timer>( *_service ) },
    _initMessageTimer{ std::make_shared<boost::asio::deadline_timer>( *_service ) },
    _managementTimer{ std::make_shared<boost::asio::deadline_timer>( *_service ) },
    _clientID{ clientID }
{}

NetworkClientBase::~NetworkClientBase() {}

std::shared_ptr<boost::asio::io_service> NetworkClientBase::_getService() {
    return _service;
}

std::shared_ptr<boost::asio::ip::tcp::socket> NetworkClientBase::_getSocket() {
    return _socket;
}

void NetworkClientBase::_startTransfer() {
    _ipAddress = _loadIpAddress();

    BOOST_LOG(lg) << Converters::netTransferClientIDToString( _clientID ) << ": " << "Connection established!" << " IP: " << Converters::convertIpAddress( _ipAddress );

    _connected = true;
    _reconnectRespiteTime = boost::posix_time::microsec_clock::local_time(); // Initialize respite time
    _startConnectionTime = boost::posix_time::microsec_clock::local_time();

    _readFromSocket();
    _writeToSocket();
    _initialLoop();
}

void NetworkClientBase::_disconnectClient() {
    if (_connected) {
        _connected = false;
        _outputQueue.clear(); // Clear output messages queue
        _socket->close(); // Close socket
        _ipAddress = 0;
        _startConnectionTime = boost::posix_time::microsec_clock::local_time();
        _clientDisconnected(); // Signal that client is disconnected
        connectionDuration.store( 0 );

        BOOST_LOG(lg) << Converters::netTransferClientIDToString( _clientID ) << ": " << "Disconnected!";
    }
}

void NetworkClientBase::_startTimers() {
    _pingTimer.reset( new boost::asio::deadline_timer( *_service ));
    _initMessageTimer.reset( new boost::asio::deadline_timer( *_service ));
    _managementTimer.reset( new boost::asio::deadline_timer( *_service ));
    _tickPingLoop( _pingInterval );
    _tickInitMsgLoop( _initLoopInterval );
    _tickManageLoop( _manageLoopFrqency );
}

void NetworkClientBase::_stopTimers() {
    _pingTimer.reset();
    _initMessageTimer.reset();
    _managementTimer.reset();
}

// Check respite timer
bool NetworkClientBase::_isReconnectRespiteExpired() {
    if (( boost::posix_time::microsec_clock::local_time() - _reconnectRespiteTime ).total_milliseconds() > _reconnectTimeout ) {
        _reconnectRespiteTime = boost::posix_time::microsec_clock::local_time();
        return true;
    }
    return false;
}

long long NetworkClientBase::getConnectionDuration() {
    return connectionDuration.load();
}

int NetworkClientBase::_getClientID() {
    return _clientID;
}

unsigned int NetworkClientBase::_getIpAddress() {
    return _ipAddress;
}

bool NetworkClientBase::_isConnected() {
    return _connected;
}

unsigned long NetworkClientBase::_loadIpAddress() {
    boost::system::error_code err;
    boost::asio::ip::tcp::endpoint endpoint = _socket->local_endpoint( err );

    if ( !err )
        return endpoint.address().to_v4().to_ulong();

    return 0;
}

void NetworkClientBase::_tickPingLoop( unsigned int timer ) {
    if ( _pingTimer ) {
        _pingTimer->expires_from_now( boost::posix_time::millisec( timer ));
        _pingTimer->async_wait( bind( &NetworkClientBase::_pingHandler, shared_from_this(), std::placeholders::_1 ));
    }
}

void NetworkClientBase::_tickInitMsgLoop( unsigned int timer ) {
    if ( _initMessageTimer ) {
        _initMessageTimer->expires_from_now( boost::posix_time::millisec( timer ));
        _initMessageTimer->async_wait( bind( &NetworkClientBase::_initMsgHandler, shared_from_this(), std::placeholders::_1 ));
    }
}

void NetworkClientBase::_tickManageLoop( unsigned int timer ) {
    if ( _managementTimer ) {
        _managementTimer->expires_from_now( boost::posix_time::millisec( timer ));
        _managementTimer->async_wait( bind( &NetworkClientBase::_manageLoopHandler, shared_from_this(), std::placeholders::_1 ));
    }
}

void NetworkClientBase::_pingHandler( const boost::system::error_code &err ) {
    if ( err ) {
        BOOST_LOG(lg) << Converters::netTransferClientIDToString( _clientID ) << ": " << "Ping timer error! " << err.message();
        return;
    }

    _sendMessage( NetworkMessage( MessageHeader( NetConst::MessageType::Ping ), 0 )); // Send ping
    _tickPingLoop( _pingInterval );
}

void NetworkClientBase::_initMsgHandler( const boost::system::error_code &err ) {
    if ( err ) {
        BOOST_LOG(lg) << Converters::netTransferClientIDToString( _clientID ) << ": " << "Initial message timer error! " << err.message();
        return;
    }

    _initialLoop();
    _tickInitMsgLoop( _initLoopInterval );
}

void NetworkClientBase::_manageLoopHandler( const boost::system::error_code &err ) {
    if ( err ) {
        BOOST_LOG(lg) << Converters::netTransferClientIDToString( _clientID ) << ": " << "Management loop timer error! " << err.message();
        return;
    }

    _managementLoop();
    _tickManageLoop( _manageLoopFrqency );
}

// Read starting point
void NetworkClientBase::_readFromSocket() {
    if ( !_connected ) return;

    std::shared_ptr<std::vector<uint8_t>> header{ std::make_shared<std::vector<uint8_t>>( MessageHeader::_rawDataSize )}; // Initialize new buffer for header
    boost::asio::async_read( *_socket, boost::asio::buffer( *header ), bind( &NetworkClientBase::_handleReadHeader, shared_from_this(), std::placeholders::_1, std::placeholders::_2, header ));
}

// Handle message header
void NetworkClientBase::_handleReadHeader( const boost::system::error_code &err, size_t bytesTrans, std::shared_ptr<std::vector<uint8_t>> inbHeader ) {
    if ( !_connected ) return;

    if ( err || !bytesTrans ) {
        BOOST_LOG(lg) << Converters::netTransferClientIDToString( _clientID ) << ": " << "Handle read error!" << " Bytes transferred = " << bytesTrans;
        return;
    }

    try {
        MessageHeader messageHeader{ inbHeader }; // Create message header from received data

        _reconnectRespiteTime = boost::posix_time::microsec_clock::local_time(); // Set time of succesfull data transfer operation
        connectionDuration.store(( _reconnectRespiteTime - _startConnectionTime ).total_seconds() );
        
        NetworkMessage netMsg( messageHeader, _clientID ); // Initialize new network message
        // Initialize new buffer for data that contain message tail size
        std::shared_ptr<std::vector<uint8_t>> tailSize = std::make_shared<std::vector<uint8_t>>( MessageTailSize::_rawDataSize );

        // Read message tail size
        boost::asio::async_read( *_socket, boost::asio::buffer( *tailSize ), bind( &NetworkClientBase::_handleReadSize, shared_from_this(), std::placeholders::_1, std::placeholders::_2, netMsg, tailSize ));

    } catch ( std::runtime_error err ) {
        BOOST_LOG(lg) << Converters::netTransferClientIDToString( _clientID ) << ": " << err.what();
        return;
    }
}

// Handle inbound data size
void NetworkClientBase::_handleReadSize(const boost::system::error_code &err, size_t bytesTrans, NetworkMessage netMsg, std::shared_ptr<std::vector<uint8_t>> inbTailSize) {
    if ( !_connected ) return;

    if ( err ) {
        BOOST_LOG(lg) << Converters::netTransferClientIDToString(_clientID) << ": " << "Size read error! " << "Bytes transferred = " << bytesTrans;
        return;
    }

    try {
        MessageTailSize tailSize{ inbTailSize };

        // If message have attached tail - read it, otherwise message ready for processing
        if ( !tailSize._isMsgOver() ) {
            // Initialize new buffer for message tail data
            std::shared_ptr<std::vector<uint8_t>> data{ std::make_shared<std::vector<uint8_t>>( tailSize._getDataSize() ) };
            boost::asio::async_read( *_socket, boost::asio::buffer( *data ), bind( &NetworkClientBase::_handleReadTail, shared_from_this(), std::placeholders::_1, std::placeholders::_2, netMsg, inbTailSize, data )); // Read data
        } else {
            _processMessage( netMsg ); // Process received message
            _readFromSocket(); // Read next message
        }

    } catch ( std::runtime_error err ) {
        BOOST_LOG(lg) << Converters::netTransferClientIDToString(_clientID) << ": " << err.what();
        return;
    }
}

// Handle inbound data
void NetworkClientBase::_handleReadTail( const boost::system::error_code &err, size_t bytesTrans, NetworkMessage netMsg, std::shared_ptr<std::vector<uint8_t>> inbTailSize, std::shared_ptr<std::vector<uint8_t> > inbData ) {
    if ( !_connected ) return;

    if ( err ) {
        BOOST_LOG(lg) << Converters::netTransferClientIDToString(_clientID) << ": " << "Tail read error! " << "Message = " << netMsg._getMessageHeader()._getMessageType() << " Bytes transferred = " << bytesTrans;
        return;
    }

    netMsg._addMessageTail( inbData, inbTailSize ); // Add inbound data to final message
    // Initialize new buffer for data that contain next message tail size
    std::shared_ptr<std::vector<uint8_t>> tailSize{ std::make_shared<std::vector<uint8_t>>( MessageTailSize::_rawDataSize )};

    // Read next message tail size
    boost::asio::async_read( *_socket, boost::asio::buffer(*tailSize), bind(&NetworkClientBase::_handleReadSize, shared_from_this(), std::placeholders::_1, std::placeholders::_2, netMsg, tailSize) );
}

// Send outbound data through strand that synchronize messages order
void NetworkClientBase::_sendMessage( NetworkMessage outbMsg ) {
    if ( !_connected ) return;


    _strand->post( std::bind( &NetworkClientBase::_handlerFillBuffer, shared_from_this(), outbMsg )); // Strand for guaranteed synchronous execution of methods
}

// Add new message to queue
void NetworkClientBase::_handlerFillBuffer( NetworkMessage outbMsg ) {
    if ( !_connected ) return;

    // Fill buffer only if there is no same type messages ( thats made to protect queue from overflow when application transfer data by slow networks )
    if ( std::count_if( _outputQueue.begin(), _outputQueue.end(), [outbMsg]( NetworkMessage msg ) {
            return msg._getMessageHeader()._getMessageType() == outbMsg._getMessageHeader()._getMessageType();
        })) return;

    _outputQueue.push_back( outbMsg ); // Add message to queue

    if ( _outputQueue.size() == 1 ) // If queue was empty before new message was added, restart writing recursion
        _writeToSocket();
}

void NetworkClientBase::_writeToSocket() {
    if ( !_connected ) return;

    if ( !_outputQueue.empty() ) {
        NetworkMessage outbMsg = _outputQueue.front();
        boost::asio::async_write( *_socket, outbMsg._getMsgForTransfer(), _strand->wrap( bind( &NetworkClientBase::_handleWriteToSocket, shared_from_this(), std::placeholders::_1, std::placeholders::_2, outbMsg )));
    }
}

void NetworkClientBase::_handleWriteToSocket( const boost::system::error_code &err, size_t bytesTrans, NetworkMessage outbMsg ) {
    if ( !_connected ) return;

    if ( err ) {
        BOOST_LOG(lg) << Converters::netTransferClientIDToString( _clientID ) << ": " << "Writing interrupted! " << "Bytes transferred = " << bytesTrans;
        return;
    }

    _reconnectRespiteTime = boost::posix_time::microsec_clock::local_time(); // Mark time of successfull transfer operation
    connectionDuration.store(( _reconnectRespiteTime - _startConnectionTime ).total_seconds() );

    if ( !_outputQueue.empty() ) {
        _confirmTransfer( _outputQueue.front() ); // Signal that message was transferred
        _outputQueue.pop_front();
        _writeToSocket(); // Write next message
    }
}
