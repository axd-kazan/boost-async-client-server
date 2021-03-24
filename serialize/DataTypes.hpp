#ifndef DATATYPES_HPP
#define DATATYPES_HPP

#include <stdint.h>

namespace DataType {
    using Switch = uint8_t;
    using Delay = uint32_t;
    
    // TIME
    using Year = uint16_t;
    using Months = uint16_t;
    using Day = uint16_t;
    using Hour = uint64_t;
    using Minute = uint64_t;
    using Second = uint64_t;
    using Millisec = uint64_t;

    // NETWORK
    using NetServerPort = uint16_t;
    using NetMsgType = uint32_t;
    using NetMsgDataSize = uint64_t;
    using IPAddress = uint32_t;
}

#endif // DATATYPES_HPP
