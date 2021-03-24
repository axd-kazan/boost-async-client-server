#ifndef NETMESSAGE_HPP
#define NETMESSAGE_HPP

#include <serialize/DataTypes.hpp>
#include <network/NetworkConst.hpp>
#include <boost/asio.hpp>
#include "MessageHeader.hpp"
#include "MessageTailSize.hpp"

class NetworkMessage {
public:
    explicit NetworkMessage( MessageHeader header, unsigned long long dataSize, uint8_t *rawData, int clientID );
    explicit NetworkMessage( MessageHeader header, std::shared_ptr<std::vector<uint8_t>> rawData, int clientID );
    explicit NetworkMessage( MessageHeader header, std::vector<std::shared_ptr<std::vector<uint8_t>>> rawData, int clientID );
    explicit NetworkMessage( MessageHeader header, int clientID );
    template<typename T> NetworkMessage( MessageHeader header, T value, int clientID ) :
        NetworkMessage( header, sizeof(T), reinterpret_cast<uint8_t*>( &value ), clientID ) {}

    void _addMessageTail( uint8_t *rawData, unsigned long long tailSize );
    void _addMessageTail( std::shared_ptr<std::vector<uint8_t>> tail, std::shared_ptr<std::vector<uint8_t>> tailSize = nullptr );

    std::shared_ptr<std::vector<uint8_t>> _getMessageTail( unsigned int pos );

    MessageHeader _getMessageHeader() const;

    unsigned long long _getTailsCount(); // Count of attached packets of the data
    unsigned long long _getTotalSize(); // Summ of sizes of all attached packets of the data

    std::vector<boost::asio::const_buffer> _getMsgForTransfer();

    int _getClientID();
    void _setClientID( int clientID );

private:
    int _clientID{};
    MessageHeader _msgHeader{ 0 };
    std::vector<std::shared_ptr<std::vector<uint8_t>>> _msgData;
};

#endif // NETMESSAGE_HPP
