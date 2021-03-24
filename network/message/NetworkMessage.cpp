#include "NetworkMessage.hpp"

NetworkMessage::NetworkMessage( MessageHeader header, unsigned long long dataSize, uint8_t *rawData, int clientID ) {
    _clientID = clientID;
    _msgHeader = header;

    _msgData.push_back( _msgHeader._getSharedData() ); // Header data
    _msgData.push_back( MessageTailSize()._getSharedData() ); // Empty tail size
    _addMessageTail( rawData, dataSize ); // Data tail
}

NetworkMessage::NetworkMessage( MessageHeader header, std::shared_ptr<std::vector<uint8_t>> rawData, int clientID ) {
    _clientID = clientID;
    _msgHeader = header;
    
    _msgData.push_back( _msgHeader._getSharedData() );
    _msgData.push_back( MessageTailSize()._getSharedData() );
    _addMessageTail( rawData );
}

NetworkMessage::NetworkMessage( MessageHeader header, std::vector<std::shared_ptr<std::vector<uint8_t>>> rawData, int clientID ) {
    _clientID = clientID;
    _msgHeader = header;

    _msgData.push_back( _msgHeader._getSharedData()  );
    _msgData.push_back( MessageTailSize()._getSharedData() );

    // Add data tail's
    for ( std::shared_ptr<std::vector<uint8_t>> &data : rawData ) {
        _addMessageTail( data );
    }
}

NetworkMessage::NetworkMessage( MessageHeader header, int clientID ) {
    _clientID = clientID;
    _msgHeader = header;

    _msgData.push_back( _msgHeader._getSharedData()  );
    _msgData.push_back( MessageTailSize()._getSharedData() );
}

void NetworkMessage::_addMessageTail( uint8_t *rawData, unsigned long long tailSize ) {
    _msgData.back() = MessageTailSize( tailSize )._getSharedData();
    _msgData.push_back( std::make_shared<std::vector<uint8_t>>( rawData, rawData + tailSize )); // Add tail data
    _msgData.push_back( MessageTailSize()._getSharedData() ); // Empty tail size
}

void NetworkMessage::_addMessageTail( std::shared_ptr<std::vector<uint8_t>> tail, std::shared_ptr<std::vector<uint8_t>> tailSize ) {
    if ( tailSize != nullptr ) _msgData.back() = tailSize;
    else _msgData.back() = MessageTailSize{ tail->size() }._getSharedData();

    _msgData.push_back( tail ); // Add tail data
    _msgData.push_back( MessageTailSize()._getSharedData() ); // Next tail size
}

MessageHeader NetworkMessage::_getMessageHeader() const {
    return _msgHeader;
}

unsigned long long NetworkMessage:: _getTailsCount() {
    return ( _msgData.size() - 2 ) / 2;
}

unsigned long long NetworkMessage::_getTotalSize() {
    unsigned long long totalSize = 0;
    for ( unsigned int i = 0; i < _getTailsCount(); i++ )
        totalSize += _getMessageTail(i)->size();
    return totalSize;
}

std::shared_ptr<std::vector<uint8_t>> NetworkMessage::_getMessageTail( unsigned int pos ) {
    if ( pos >= _getTailsCount() ) throw std::runtime_error( "Tail is not exists!" );
    return _msgData[ (pos * 2 ) + 2 ];
}

std::vector<boost::asio::const_buffer> NetworkMessage::_getMsgForTransfer() {
    std::vector<boost::asio::const_buffer> transMsg;

    for ( std::shared_ptr<std::vector<uint8_t>>& msgPart : _msgData )
        transMsg.push_back(boost::asio::buffer( *msgPart ) );

    return transMsg;
}

int NetworkMessage::_getClientID() {
    return _clientID;
}

void NetworkMessage::_setClientID( int clientID ) {
    _clientID = clientID;
}
