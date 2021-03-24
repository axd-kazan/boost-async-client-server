#ifndef CLIENTSIDEBASE_HPP
#define CLIENTSIDEBASE_HPP

#include "NetworkClientBase.hpp"

class ClientSideBase : public NetworkClientBase {
public:
    explicit ClientSideBase();
    ~ClientSideBase();

    void startClient();
    void stopClient();

    void connect();
    void disconnect();

    void sendMessage( unsigned int messageType, std::vector<std::shared_ptr<std::vector<uint8_t>>> rawData );
    void sendMessage( unsigned int messageType, std::shared_ptr<std::vector<uint8_t>> rawData );
    void sendMessage( unsigned int messageType );
    void sendMessage( unsigned int messageType, unsigned long long dataSize, uint8_t *rawData );
    template<typename T> void sendMessage( unsigned int messageType, T value ) {
        NetworkMessage netMsg( MessageHeader( messageType ), value, NetConst::ClientID::ClientSide );
        _getService()->post( std::bind( &ClientSideBase::_sendMessage, shared_from_base<ClientSideBase>(), netMsg));
    }

    void setServerAddress( std::pair<std::string, std::string> serverAddress );

private:
    boost::log::sources::logger_mt& lg = boostLogger::get();
    std::shared_ptr<boost::asio::io_service::work> _work{}; // Used to control lifetime of io_service
    std::shared_ptr <boost::asio::ip::tcp::resolver> _resolver{}; // Used when server's address is domain name
    boost::asio::ip::tcp::endpoint _endPoint{};
    std::mutex mtx;
    std::thread clientThread;

    // Server address and port
    std::pair<std::string, std::string> _serverAddress{ "127.0.0.1", std::to_string( NetConst::Defaults::ServersPort ) };

    void _startService(); // Start asio io service
    void _startConnecting(); // Start connection process to server
    void _connectionHandler( const boost::system::error_code& err );
    void _stopClient();

    void _managementLoop() override; // Check if there is a data transferring process is in process, if transfer is freezed - start reconnection
    virtual void _initialLoop() override {};
    virtual void _confirmTransfer( NetworkMessage netMsg ) override {};
    virtual void _processMessage( NetworkMessage netMsg ) override {};
    virtual void _clientDisconnected() override {};
};

#endif /* CLIENTSIDEBASE_HPP */

