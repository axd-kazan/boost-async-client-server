#ifndef SERIALIZABLE_HPP
#define SERIALIZABLE_HPP

#include <stdint.h>
#include <memory>
#include <vector>

#ifdef __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif

class Serializable {
public:
    std::shared_ptr<std::vector<uint8_t> > _getSharedData() {
        return std::make_shared<std::vector<uint8_t>>( _rawDataBegin(), _rawDataEnd() );
    }

private:
    virtual uint8_t* _rawDataBegin() = 0;
    virtual uint8_t* _rawDataEnd() = 0;

};

#endif // SERIALIZABLE_HPP
