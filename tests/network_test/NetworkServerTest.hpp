#ifndef NETWORKSERVERTEST_HPP
#define NETWORKSERVERTEST_HPP

#include <network/server/NetworkServerBase.hpp>
#include "NetworkTestFunc.hpp"

class NetworkServerTest : public NetworkServerBase {
public:
    NetworkServerTest();
    ~NetworkServerTest();

    bool isTestFinished();
    void stopAllClients();

private:
    boost::log::sources::logger_mt& lg = boostLogger::get();

    std::list<NetworkTestFunc> _netTests;
    std::atomic<bool> testFinished{};

    std::list<int> _clientsIDs;

    void _initServerLoop() override;
    void _initMessage( std::shared_ptr<ServerSideClient> client ) override;
    void _confirmTransfer( std::shared_ptr<ServerSideClient> client, NetworkMessage netMsg ) override;
    void _processMessage( std::shared_ptr<ServerSideClient> client, NetworkMessage netMsg ) override;
    void _clientConnected( std::shared_ptr<ServerSideClient> client ) override;
    void _clientDisconnected( std::shared_ptr<ServerSideClient> client ) override;
    
};

#endif // NETWORKSERVERTEST_HPP
