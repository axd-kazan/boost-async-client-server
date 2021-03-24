#ifndef SERVERSIDECLIENT_HPP
#define SERVERSIDECLIENT_HPP

#include "NetworkClientBase.hpp"

class NetworkServerBase;

class ServerSideClient : public NetworkClientBase {
public:
    explicit ServerSideClient( std::weak_ptr<NetworkServerBase> server, std::shared_ptr<boost::asio::io_service> service, int clientID );
    ~ServerSideClient();

    int _getClientID();
    long _getConnectionDuration();

    void _startClient();
    void _stopClient();

    void _sendMessage( NetworkMessage outbMsg );

    std::shared_ptr<boost::asio::ip::tcp::socket> _getSocket();

private:
    std::weak_ptr<NetworkServerBase> _server;

    boost::log::sources::logger_mt& lg = boostLogger::get();

    void _managementLoop() override;
    virtual void _initialLoop() override;
    virtual void _confirmTransfer( NetworkMessage netMsg ) override;
    virtual void _processMessage( NetworkMessage netMsg ) override;
    virtual void _clientDisconnected() override;
};

#endif /* SERVERSIDECLIENT_HPP */

