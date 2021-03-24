#ifndef MESSAGEHEADER_HPP
#define MESSAGEHEADER_HPP

#include <serialize/Serializable.hpp>
#include <serialize/DataTypes.hpp>
#include <common/CurrentTime.hpp>

class MessageHeader : public Serializable {
private:
    // Header contain only two values: Type of message and message creation time
    PACK(struct StructData {
        DataType::NetMsgType messageType;
        uint8_t time[CurrentTime::_rawDataSize];
    });

public:
    explicit MessageHeader( std::shared_ptr<std::vector<uint8_t>> rawData );
    explicit MessageHeader( unsigned int messageType );

    unsigned int _getMessageType();
    CurrentTime _getTime();

    static const size_t _rawDataSize;

private:
    union UnionData {
        StructData param;
        uint8_t raw[sizeof(StructData)]{};
    } _unData;

    uint8_t* _rawDataBegin() override;
    uint8_t* _rawDataEnd() override;
};


#endif // MESSAGEHEADER_HPP
