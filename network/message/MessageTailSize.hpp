#ifndef MESSAGETAILSIZE_HPP
#define MESSAGETAILSIZE_HPP

#include <vector>
#include <memory>
#include <stdexcept>
#include <serialize/Serializable.hpp>
#include <serialize/DataTypes.hpp>

class MessageTailSize : public Serializable {
private:
    // Each data attached to a message is preceded by a structure containing the size of that data
    PACK ( struct StructData {
        DataType::Switch endOfMessage; // Flag that marks end of the message
        DataType::NetMsgDataSize dataSize;
    } );

public:
    explicit MessageTailSize();
    explicit MessageTailSize( std::shared_ptr<std::vector<uint8_t>> rawData );
    explicit MessageTailSize( unsigned long long dataSize, bool endOfMessage = false );

    void _setDataSize( unsigned long long dataSize );
    unsigned long long _getDataSize();

    bool _isMsgOver();

    static const size_t _rawDataSize;

private:
    union UnionData {
        StructData param;
        uint8_t raw[ sizeof(StructData) ]{};
    } _unData;

    uint8_t* _rawDataBegin() override;
    uint8_t* _rawDataEnd() override;
};

#endif // MESSAGETAILSIZE_HPP
