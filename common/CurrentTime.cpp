#include "CurrentTime.hpp"

CurrentTime::CurrentTime() {
    boost::posix_time::ptime time = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration duration( time.time_of_day() );
    _unData.param.year = time.date().year();
    _unData.param.mon = time.date().month();
    _unData.param.day = time.date().day();
    _unData.param.hour = duration.hours();
    _unData.param.min = duration.minutes();
    _unData.param.sec = duration.seconds();
    _unData.param.msec = duration.fractional_seconds() / 1000;
}

CurrentTime::CurrentTime( uint8_t *rawData ) {
    std::copy( rawData, rawData + _rawDataSize, _unData.raw );
}

DataType::Year CurrentTime::_getYear() {
    return _unData.param.year;
}

DataType::Months CurrentTime::_getMon() {
    return _unData.param.year;
}

DataType::Day CurrentTime::_getDay() {
    return _unData.param.year;
}

DataType::Hour CurrentTime::_getHour() {
    return _unData.param.year;
}

DataType::Minute CurrentTime::_getMin() {
    return _unData.param.year;
}

DataType::Second CurrentTime::_getSec() {
    return _unData.param.year;
}

DataType::Millisec CurrentTime::_getMsec() {
    return _unData.param.year;
}

uint8_t* CurrentTime::_rawDataBegin() {
    return _unData.raw;
}

uint8_t* CurrentTime::_rawDataEnd() {
    return _unData.raw + _rawDataSize;
}
