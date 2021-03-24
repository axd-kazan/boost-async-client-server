#include "NetworkServerTest.hpp"
#include <boost/test/unit_test.hpp>

NetworkServerTest::NetworkServerTest() {
}

NetworkServerTest::~NetworkServerTest() {
}

bool NetworkServerTest::isTestFinished() {
    return testFinished;
}

void NetworkServerTest::stopAllClients() {
    _getService()->post( [this]() {        
        for ( auto &id : _clientsIDs )
            stopClient( id );
        });
}

void NetworkServerTest::_initServerLoop() {
        // Prepare message data
        std::shared_ptr<std::vector<uint8_t>> data1( std::make_shared<std::vector<uint8_t>>( 10000, 0 ));

        uint8_t j{};
        for ( unsigned int i = 0; i < 10000; i++ )
            data1->at(i) = j++;

        std::vector<std::shared_ptr<std::vector<uint8_t>>> data2;
        data2.resize( 10, std::make_shared<std::vector<uint8_t>>( *data1 ) );

        // Send test messages            
        sendBroadMessage( 22 );
        sendMultiMessage( _clientsIDs, 23 );

        unsigned int singleVal = 1234567890;            
        sendBroadMessage( 32, singleVal );
        sendMultiMessage( _clientsIDs, 33, singleVal );            

        sendBroadMessage( 42, data1 );
        sendMultiMessage( _clientsIDs, 43, data1 );
            
        sendBroadMessage( 52, data2 );
        sendMultiMessage( _clientsIDs, 53, data2 );
            
        sendBroadMessage( 62, data1->size(), data1->data() );
        sendMultiMessage( _clientsIDs, 63, data1->size(), data1->data() );
}

void NetworkServerTest::_initMessage( std::shared_ptr<ServerSideClient> client ) {
    auto test = std::find_if( _netTests.begin(), _netTests.end(), [ client ] ( const NetworkTestFunc &netTest ) {
        return netTest._getClientID() == client->_getClientID(); });

    if ( test != _netTests.end() ) {
        // Prepare message data
        std::shared_ptr<std::vector<uint8_t>> data1( std::make_shared<std::vector<uint8_t>>( 10000, 0 ));

        uint8_t j{};
        for ( unsigned int i = 0; i < 10000; i++ )
            data1->at(i) = j++;

        std::vector<std::shared_ptr<std::vector<uint8_t>>> data2;
        data2.resize( 10, std::make_shared<std::vector<uint8_t>>( *data1 ) );

        unsigned int singleVal = 1234567890;

        // Send test messages
        sendUniMessage( client->_getClientID(), 21 );
        sendUniMessage( client->_getClientID(), 31, singleVal );
        sendUniMessage( client->_getClientID(), 41, data1 );
        sendUniMessage( client->_getClientID(), 51, data2 );
        sendUniMessage( client->_getClientID(), 61, data1->size(), data1->data() );
    }
}

void NetworkServerTest::_confirmTransfer( std::shared_ptr<ServerSideClient> client, NetworkMessage netMsg ) {
    auto test = std::find_if( _netTests.begin(), _netTests.end(), [ client ] ( const NetworkTestFunc &netTest ) {
        return netTest._getClientID() == client->_getClientID(); });

    if ( test != _netTests.end() )
        test->_sendMessage( netMsg );

    testFinished = !std::count_if( _netTests.begin(), _netTests.end(), []( auto const& netTest ) { return !netTest._isTestFinishedServer(); });
}

void NetworkServerTest::_processMessage( std::shared_ptr<ServerSideClient> client, NetworkMessage netMsg ) {
    auto test = std::find_if( _netTests.begin(), _netTests.end(), [ client ] ( const NetworkTestFunc &netTest ) {
        return netTest._getClientID() == client->_getClientID(); });

    if ( test != _netTests.end() ) {
        if ( !test->_receiveMessage( netMsg )) {
            BOOST_LOG(lg) << Converters::netTransferClientIDToString( client->_getClientID() ) << ": " << "Message processing error!";
            throw std::runtime_error( "Message processing error!" );
        }
    }
}

void NetworkServerTest::_clientConnected( std::shared_ptr<ServerSideClient> client ) {
    _netTests.push_back( NetworkTestFunc{} );
    _netTests.back()._setClientID( client->_getClientID() );
    _clientsIDs.push_back( client->_getClientID() );
}

void NetworkServerTest::_clientDisconnected( std::shared_ptr<ServerSideClient> client ) {
    auto test = std::find_if( _netTests.begin(), _netTests.end(), [ client ] ( const NetworkTestFunc &netTest ) {
        return netTest._getClientID() == client->_getClientID(); });

    if ( test != _netTests.end() ) {
        BOOST_CHECK( test->_isTestFinishedServer() );
        _netTests.erase( test );
    }
}
