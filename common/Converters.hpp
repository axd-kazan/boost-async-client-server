#ifndef CONVERTERS_HPP
#define CONVERTERS_HPP

#include <network/NetworkConst.hpp>

struct Converters {
public:
    static std::string netTransferClientIDToString( int clientID ); // Convert ClientID to string
    static std::string convertIpAddress( unsigned int ipAddress ); // Convert 4 bytes IP address to string
  
private:

};

#endif // CONVERTERS_HPP
