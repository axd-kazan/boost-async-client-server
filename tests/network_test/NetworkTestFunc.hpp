#ifndef NETWORKTEST_HPP
#define NETWORKTEST_HPP

#include <network/message/NetworkMessage.hpp>

class NetworkTestFunc {
public:
    bool _isTestFinishedServer() const;
    bool _isTestFinishedClient();
    bool _receiveMessage( NetworkMessage netMsg );
    void _sendMessage( NetworkMessage netMsg );

    int _getClientID() const;
    void _setClientID( int clientID );

    void _resetCounters();

private:
    int clientID{};

    int send21{}, send22{}, send23{}, send31{}, send32{}, send33{}, send41{}, send42{}, send43{}, send51{}, send52{}, send53{}, send61{}, send62{}, send63{};
    int recv21{}, recv22{}, recv23{}, recv31{}, recv32{}, recv33{}, recv41{}, recv42{}, recv43{}, recv51{}, recv52{}, recv53{}, recv61{}, recv62{}, recv63{};
};

#endif // NETWORKTEST_HPP
