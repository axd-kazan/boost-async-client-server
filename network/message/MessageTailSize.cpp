#include "MessageTailSize.hpp"

const size_t MessageTailSize::_rawDataSize{ sizeof(StructData) };;

MessageTailSize::MessageTailSize() {
    _unData.param.endOfMessage = true;
}

MessageTailSize::MessageTailSize(std::shared_ptr<std::vector<uint8_t> > rawData ) {
    if ( rawData->size() != _rawDataSize )
        throw std::runtime_error( "Incorrect tail size!" );

    copy( rawData->begin(), rawData->end(), _unData.raw );
}

MessageTailSize::MessageTailSize( unsigned long long dataSize, bool endOfMessage ) {
    _unData.param.endOfMessage = endOfMessage;
    _setDataSize( dataSize );
}

uint8_t* MessageTailSize::_rawDataBegin() {
    return _unData.raw;
}

uint8_t* MessageTailSize::_rawDataEnd() {
    return _unData.raw + _rawDataSize;
}

bool MessageTailSize::_isMsgOver() {
    return _unData.param.endOfMessage;
}

unsigned long long MessageTailSize::_getDataSize() {
    return _unData.param.dataSize;
}

void MessageTailSize::_setDataSize( unsigned long long dataSize ) {
    _unData.param.dataSize = dataSize;
}
