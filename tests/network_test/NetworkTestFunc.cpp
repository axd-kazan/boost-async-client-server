#include "NetworkTestFunc.hpp"
#include <boost/test/unit_test.hpp>

void NetworkTestFunc::_setClientID( int clientID ) {
    this->clientID = clientID;
}

int NetworkTestFunc::_getClientID() const {
    return clientID;
}

bool NetworkTestFunc::_isTestFinishedServer() const {
    if ( send21 >= 5 && send22 >= 5 && send23 >= 5 && send31 >= 5 && send32 >= 5 && send33 >= 5 && send41 >= 5 && send42 >= 5 && send43 >= 5 &&
         send51 >= 5 && send52 >= 5 && send53 >= 5 && send61 >= 5 && send62 >= 5 && send63 >= 5 &&
         recv21 >= 5 && recv31 >= 5 && recv41 >= 5 && recv51 >= 5 && recv61 >= 5 ) return true;
    return false;
}

bool NetworkTestFunc::_isTestFinishedClient() {
    if ( send21 >= 5 && send31 >= 5 && send41 >= 5 && send51 >= 5 && send61 >= 5 &&
         recv21 >= 5 && recv22 >= 5 && recv23 >= 5 && recv31 >= 5 && recv32 >= 5 && recv33 >= 5 && recv41 >= 5 && recv42 >= 5 && recv43 >= 5 &&
         recv51 >= 5 && recv52 >= 5 && recv53 >= 5 && recv61 >= 5 && recv62 >= 5 && recv63 >= 5 ) return true;
    return false;
}

bool NetworkTestFunc::_receiveMessage( NetworkMessage netMsg ) {
    // Single value message
    if ( netMsg._getMessageHeader()._getMessageType() == 31 || netMsg._getMessageHeader()._getMessageType() == 32 || netMsg._getMessageHeader()._getMessageType() == 33 )
        if ( *reinterpret_cast<unsigned int*>( netMsg._getMessageTail(0)->data() ) != 1234567890 ) return false;

    // Single-tail message
    if ( netMsg._getMessageHeader()._getMessageType() == 41 || netMsg._getMessageHeader()._getMessageType() == 42 || netMsg._getMessageHeader()._getMessageType() == 43 ||
         netMsg._getMessageHeader()._getMessageType() == 61 || netMsg._getMessageHeader()._getMessageType() == 62 || netMsg._getMessageHeader()._getMessageType() == 63 ) {

        if ( netMsg._getTailsCount() != 1 ) return false;
    }

    // Multi-tail message
    if ( netMsg._getMessageHeader()._getMessageType() == 51 || netMsg._getMessageHeader()._getMessageType() == 52 || netMsg._getMessageHeader()._getMessageType() == 53 ) {

        if ( netMsg._getTailsCount() != 10 ) return false;

    }

    if ( netMsg._getMessageHeader()._getMessageType() == 41 || netMsg._getMessageHeader()._getMessageType() == 42 || netMsg._getMessageHeader()._getMessageType() == 43 ||
         netMsg._getMessageHeader()._getMessageType() == 51 || netMsg._getMessageHeader()._getMessageType() == 52 || netMsg._getMessageHeader()._getMessageType() == 53 ||
         netMsg._getMessageHeader()._getMessageType() == 61 || netMsg._getMessageHeader()._getMessageType() == 62 || netMsg._getMessageHeader()._getMessageType() == 63 ) {

        for ( unsigned int i = 0; i < netMsg._getTailsCount(); i++ ) {

            std::shared_ptr<std::vector<uint8_t>> data = netMsg._getMessageTail( i );

            if ( data->size() != 10000 ) return false;

            uint8_t k{};
            for ( unsigned int j = 0; j < data->size(); j++ )
                if ( data->at( j ) != k++ ) return false;

        }
    }

    if ( netMsg._getMessageHeader()._getMessageType() == 21 ) recv21++;
    if ( netMsg._getMessageHeader()._getMessageType() == 22 ) recv22++;
    if ( netMsg._getMessageHeader()._getMessageType() == 23 ) recv23++;
    if ( netMsg._getMessageHeader()._getMessageType() == 31 ) recv31++;
    if ( netMsg._getMessageHeader()._getMessageType() == 32 ) recv32++;
    if ( netMsg._getMessageHeader()._getMessageType() == 33 ) recv33++;
    if ( netMsg._getMessageHeader()._getMessageType() == 41 ) recv41++;
    if ( netMsg._getMessageHeader()._getMessageType() == 42 ) recv42++;
    if ( netMsg._getMessageHeader()._getMessageType() == 43 ) recv43++;
    if ( netMsg._getMessageHeader()._getMessageType() == 51 ) recv51++;
    if ( netMsg._getMessageHeader()._getMessageType() == 52 ) recv52++;
    if ( netMsg._getMessageHeader()._getMessageType() == 53 ) recv53++;
    if ( netMsg._getMessageHeader()._getMessageType() == 61 ) recv61++;
    if ( netMsg._getMessageHeader()._getMessageType() == 62 ) recv62++;
    if ( netMsg._getMessageHeader()._getMessageType() == 63 ) recv63++;

    return true;
}

void NetworkTestFunc::_sendMessage( NetworkMessage netMsg ) {
    if (netMsg._getMessageHeader()._getMessageType() == 21) send21++;
    if (netMsg._getMessageHeader()._getMessageType() == 22) send22++;
    if (netMsg._getMessageHeader()._getMessageType() == 23) send23++;
    if (netMsg._getMessageHeader()._getMessageType() == 31) send31++;
    if (netMsg._getMessageHeader()._getMessageType() == 32) send32++;
    if (netMsg._getMessageHeader()._getMessageType() == 33) send33++;
    if (netMsg._getMessageHeader()._getMessageType() == 41) send41++;
    if (netMsg._getMessageHeader()._getMessageType() == 42) send42++;
    if (netMsg._getMessageHeader()._getMessageType() == 43) send43++;
    if (netMsg._getMessageHeader()._getMessageType() == 51) send51++;
    if (netMsg._getMessageHeader()._getMessageType() == 52) send52++;
    if (netMsg._getMessageHeader()._getMessageType() == 53) send53++;
    if (netMsg._getMessageHeader()._getMessageType() == 61) send61++;
    if (netMsg._getMessageHeader()._getMessageType() == 62) send62++;
    if (netMsg._getMessageHeader()._getMessageType() == 63) send63++;
}

void NetworkTestFunc::_resetCounters() {
    send21 = 0;
    send22 = 0;
    send23 = 0;
    send31 = 0;
    send32 = 0;
    send33 = 0;
    send41 = 0;
    send42 = 0;
    send43 = 0;
    send51 = 0;
    send52 = 0;
    send53 = 0;
    send61 = 0;
    send62 = 0;
    send63 = 0;
    recv21 = 0;
    recv22 = 0;
    recv23 = 0;
    recv31 = 0;
    recv32 = 0;
    recv33 = 0;
    recv41 = 0;
    recv42 = 0;
    recv43 = 0;
    recv51 = 0;
    recv52 = 0;
    recv53 = 0;
    recv61 = 0;
    recv62 = 0;
    recv63 = 0;
}
