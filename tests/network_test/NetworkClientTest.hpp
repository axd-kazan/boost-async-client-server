#ifndef NETWORKCLIENTTEST_HPP
#define NETWORKCLIENTTEST_HPP

#include <network/client/ClientSideBase.hpp>
#include "NetworkTestFunc.hpp"

class NetworkClientTest : public ClientSideBase {
public:
    NetworkClientTest();
    ~NetworkClientTest();

    bool isTestFinished();
    void resetCounters();

private:
    boost::log::sources::logger_mt& lg = boostLogger::get();

    NetworkTestFunc _netTest;
    std::atomic<bool> testFinished{};    

    void _initialLoop() override;
    void _confirmTransfer( NetworkMessage netMsg ) override;
    void _processMessage( NetworkMessage netMsg ) override;
    void _clientDisconnected() override;

};

#endif /* NETWORKCLIENTTEST_HPP */

