#ifndef NETWORKSERVERBASE_HPP
#define NETWORKSERVERBASE_HPP

#include <network/client/ServerSideClient.hpp>

class NetworkServerBase : public std::enable_shared_from_this<NetworkServerBase> {
public:
    NetworkServerBase();
    ~NetworkServerBase();

    void startServer();
    void stopServer();
    void stopClient( int clientID );

    // Send message to all clients
    void sendBroadMessage( unsigned int messageType, std::vector<std::shared_ptr<std::vector<uint8_t>>> rawData );
    void sendBroadMessage( unsigned int messageType, std::shared_ptr<std::vector<uint8_t>> rawData );
    void sendBroadMessage( unsigned int messageType );
    void sendBroadMessage( unsigned int messageType, unsigned long long dataSize, uint8_t *rawData );
    template<typename T> void sendBroadMessage( unsigned int messageType, T value ) {
        NetworkMessage netMsg( MessageHeader( messageType ), value, NetConst::ClientID::ServerSide );
        _service->post( std::bind( &NetworkServerBase::_sendMessage, shared_from_this(), netMsg ));
    }

    // Send message to selected client
    void sendUniMessage( int clientID, unsigned int messageType, std::vector<std::shared_ptr<std::vector<uint8_t>>> rawData );
    void sendUniMessage( int clientID, unsigned int messageType, std::shared_ptr<std::vector<uint8_t>> rawData );
    void sendUniMessage( int clientID, unsigned int messageType );
    void sendUniMessage( int clientID, unsigned int messageType, unsigned long long dataSize, uint8_t *rawData );
    template<typename T> void sendUniMessage( int clientID, unsigned int messageType, T value ) {
        NetworkMessage netMsg( MessageHeader( messageType ), value, clientID );
        _service->post( std::bind( &NetworkServerBase::_sendMessage, shared_from_this(), netMsg ));
    }

    // Set message to selected clients list
    template <template <class ...> class Container, class Alloc>
    void sendMultiMessage( Container<int, Alloc> clientIDs, unsigned int messageType, std::vector<std::shared_ptr<std::vector<uint8_t>>> rawData ) {
        NetworkMessage netMsg( MessageHeader( messageType ), rawData, NetConst::ClientID::ServerSide );
        _service->post( std::bind( &NetworkServerBase::_multiMessage<Container, Alloc>, shared_from_this(), clientIDs, netMsg ));
    }

    template <template <class ...> class Container, class Alloc>
    void sendMultiMessage( Container<int, Alloc> clientIDs, unsigned int messageType, std::shared_ptr<std::vector<uint8_t>> rawData ) {
        NetworkMessage netMsg( MessageHeader( messageType ), rawData, NetConst::ClientID::ServerSide );
        _service->post( std::bind( &NetworkServerBase::_multiMessage<Container, Alloc>, shared_from_this(), clientIDs, netMsg ));
    }

    template <template <class ...> class Container, class Alloc>
    void sendMultiMessage( Container<int, Alloc> clientIDs, unsigned int messageType ) {
        NetworkMessage netMsg( MessageHeader( messageType ), NetConst::ClientID::ServerSide );
        _service->post( std::bind( &NetworkServerBase::_multiMessage<Container, Alloc>, shared_from_this(), clientIDs, netMsg ));
    }

    template <template <class ...> class Container, class Alloc>
    void sendMultiMessage( Container<int, Alloc> clientIDs, unsigned int messageType, unsigned long long dataSize, uint8_t *rawData ) {
        NetworkMessage netMsg( MessageHeader( messageType ), dataSize, rawData, NetConst::ClientID::ServerSide );
        _service->post( std::bind( &NetworkServerBase::_multiMessage<Container, Alloc>, shared_from_this(), clientIDs, netMsg ));
    }

    template <template <class ...> class Container, class Alloc, typename T>
    void sendMultiMessage( Container<int, Alloc> clientIDs, unsigned int messageType, T value ) {
        NetworkMessage netMsg( MessageHeader( messageType ), value, NetConst::ClientID::ServerSide );
        _service->post( std::bind( &NetworkServerBase::_multiMessage<Container, Alloc>, shared_from_this(), clientIDs, netMsg ));
    }

    // Callback functions
    void initMessage( std::shared_ptr<ServerSideClient> client );
    void confirmTransfer( std::shared_ptr<ServerSideClient> client, NetworkMessage netMsg );
    void processMessage( std::shared_ptr<ServerSideClient> client, NetworkMessage netMsg );
    void clientDisconnected( std::shared_ptr<ServerSideClient> client );

    void setPort( unsigned short port );
    unsigned long long getClientsCount();

protected:
    std::shared_ptr<boost::asio::io_service> _getService();

private:
    boost::log::sources::logger_mt& lg = boostLogger::get();
    std::shared_ptr<boost::asio::io_service> _service{ std::make_shared<boost::asio::io_service>() };
    std::shared_ptr<boost::asio::io_service::work> _work{};
    std::shared_ptr<boost::asio::ip::tcp::acceptor> _acceptor{};
    std::shared_ptr<boost::asio::deadline_timer> _initMessageTimer{}; // Initial message resend timer

    std::mutex mtx;
    std::thread serverThread;

    std::atomic<unsigned short> port{ NetConst::Defaults::ServersPort };
    std::list<std::shared_ptr<ServerSideClient>> _clientsList; // List of connected clients
    
    std::atomic<unsigned long long> clientsCount{};

    unsigned int _initLoopInterval{ NetConst::Defaults::ServersInitMsgInterval };

    void _sendMessage( NetworkMessage netMsg );

    template <template <class ...> class Container, class Alloc>
    void _multiMessage( Container<int, Alloc> clientIDs, NetworkMessage netMsg ) {
        for ( int &id : clientIDs ) {
            netMsg._setClientID( id );
            _service->post( std::bind( &NetworkServerBase::_sendMessage, shared_from_this(), netMsg ));
        }
    }

    void _removeClientFromList( std::shared_ptr<ServerSideClient> client );

    void _stopClient( int clientID );
    void _startService(); // Starting point for boost io_service
    void _stopServer();
    void _startListen(); // Server's starting point
    std::shared_ptr<ServerSideClient> _makeNewClient();
    void handleAccept( std::shared_ptr<ServerSideClient> client, const boost::system::error_code& err );

    void _startTimers(); // Start server timers
    void _stopTimers(); // Stop server timers
    void _tickInitMsgLoop( unsigned int timer );
    void _initMsgHandler( const boost::system::error_code &err );
    
    virtual void _initServerLoop() = 0; // Servers initialization messages loop
    virtual void _initMessage( std::shared_ptr<ServerSideClient> client ) = 0;
    virtual void _confirmTransfer( std::shared_ptr<ServerSideClient> client, NetworkMessage netMsg ) = 0;
    virtual void _processMessage( std::shared_ptr<ServerSideClient> client, NetworkMessage netMsg ) = 0;
    virtual void _clientConnected( std::shared_ptr<ServerSideClient> client ) = 0;
    virtual void _clientDisconnected( std::shared_ptr<ServerSideClient> client ) = 0;
};

#endif // NETWORKSERVERBASE_HPP
