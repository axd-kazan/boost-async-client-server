#include "MessageHeader.hpp"

const size_t MessageHeader::_rawDataSize{ sizeof(StructData) };

MessageHeader::MessageHeader(std::shared_ptr<std::vector<uint8_t> > rawData ) {
    if ( rawData->size() != _rawDataSize )
        throw std::runtime_error( "Incorrect header size!" );

    std::copy( rawData->begin(), rawData->end(), _unData.raw );
}

MessageHeader::MessageHeader( unsigned int messageType ) {
    _unData.param.messageType = messageType;
    CurrentTime time;
    std::copy( time._rawDataBegin(), time._rawDataEnd(), _unData.param.time );
}

CurrentTime MessageHeader::_getTime() {
    return CurrentTime( _unData.param.time );
}

unsigned int MessageHeader::_getMessageType() {
    return _unData.param.messageType;
}

uint8_t* MessageHeader::_rawDataBegin() {
    return _unData.raw;
}

uint8_t* MessageHeader::_rawDataEnd() {
    return _unData.raw + _rawDataSize;
}
