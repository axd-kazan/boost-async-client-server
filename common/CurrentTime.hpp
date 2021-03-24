#ifndef CURRENTTIME_HPP
#define CURRENTTIME_HPP

#include <iostream>
#include <boost/date_time.hpp>
#include <serialize/Serializable.hpp>
#include <serialize/DataTypes.hpp>

class CurrentTime : Serializable {
private:
    PACK(struct StructData {
        DataType::Year year;
        DataType::Months mon;
        DataType::Day day;
        DataType::Hour hour;
        DataType::Minute min;
        DataType::Second sec;
        DataType::Millisec msec;
    });

public:
    explicit CurrentTime();
    explicit CurrentTime( uint8_t *rawData );

    DataType::Year _getYear();
    DataType::Months _getMon();
    DataType::Day _getDay();
    DataType::Hour _getHour();
    DataType::Minute _getMin();
    DataType::Second _getSec();
    DataType::Millisec _getMsec();

    uint8_t* _rawDataBegin() override;
    uint8_t* _rawDataEnd() override;

    static const size_t _rawDataSize{ sizeof(StructData) };

private:
    union UnionData {
        StructData param;
        uint8_t raw[ _rawDataSize ]{};
    } _unData;
};

#endif // CURRENTTIME_HPP
