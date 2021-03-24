#include "Converters.hpp"

std::string Converters::netTransferClientIDToString( int clientID ) {
    if ( clientID == NetConst::ClientID::ClientSide )
        return "ClientSide";
    if ( clientID == NetConst::ClientID::ServerSide )
        return "ServerSide";
    return "Server-" + std::to_string( clientID );
}

std::string Converters::convertIpAddress( unsigned int ipAddress ) {
    return std::to_string( ((uint8_t*)&ipAddress)[3] ) + "." + std::to_string( ((uint8_t*)&ipAddress)[2] ) + "." + std::to_string( ((uint8_t*)&ipAddress)[1] ) + "." + std::to_string( ((uint8_t*)&ipAddress)[0] );
}
