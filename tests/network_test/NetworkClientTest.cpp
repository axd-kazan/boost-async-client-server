#include "NetworkClientTest.hpp"
#include <boost/test/unit_test.hpp>

NetworkClientTest::NetworkClientTest() {
}

NetworkClientTest::~NetworkClientTest() {
    BOOST_CHECK( _netTest._isTestFinishedClient() );
}

bool NetworkClientTest::isTestFinished() {
    return testFinished;
}

void NetworkClientTest::resetCounters() {
    _getService()->post( std::bind( &NetworkTestFunc::_resetCounters, _netTest ));
    testFinished = false;
}

void NetworkClientTest::_initialLoop() {
    // Prepare message data
    std::shared_ptr<std::vector<uint8_t>> data1( std::make_shared<std::vector<uint8_t>>( 10000, 0 ));

    uint8_t j{};
    for ( unsigned int i = 0; i < 10000; i++ )
        data1->at( i ) = j++;

    std::vector<std::shared_ptr<std::vector<uint8_t>>> data2;
    data2.resize( 10, std::make_shared<std::vector<uint8_t>>( *data1 ) );

    unsigned int singleVal = 1234567890;

    // Send test messages
    sendMessage( 21 );
    sendMessage( 31, singleVal );
    sendMessage( 41, data1 );
    sendMessage( 51, data2 );
    sendMessage( 61, data1->size(), data1->data() );
}

void NetworkClientTest::_confirmTransfer( NetworkMessage netMsg ) {
    _netTest._sendMessage( netMsg );

    testFinished = _netTest._isTestFinishedClient();
}

void NetworkClientTest::_processMessage( NetworkMessage netMsg ) {
    if ( !_netTest._receiveMessage( netMsg )) {
        BOOST_LOG(lg) << Converters::netTransferClientIDToString( _getClientID() ) << ": " << "Message processing error!";
        throw std::runtime_error( "Message processing error!" );
    }
}

void NetworkClientTest::_clientDisconnected() {
}
