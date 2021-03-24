#include <chrono>

#define BOOST_TEST_MODULE RasPiRemCamTest
#include <boost/test/unit_test.hpp>

#include <tests/network_test/NetworkServerTest.hpp>
#include <tests/network_test/NetworkClientTest.hpp>

BOOST_AUTO_TEST_CASE( TransferTest ) {
    int clientsCnt = 100;

    boost::log::sources::logger_mt& lg = boostLogger::get();

    std::shared_ptr<NetworkServerTest> testServer = std::make_shared<NetworkServerTest>();
    std::vector<std::shared_ptr<NetworkClientTest>> testClients;

    testServer->setPort(2005);

    BOOST_LOG(lg) << "Network module test" << ": " << "Starting clients...";

    for ( int i = 0; i < clientsCnt; i++ ) {
        testClients.push_back( std::make_shared<NetworkClientTest>() );
        testClients.back()->setServerAddress( std::pair<std::string, std::string>( "localhost", "2005" ));
        testClients.back()->startClient();
    }

    std::this_thread::sleep_for( std::chrono::seconds( 10 ));

    BOOST_LOG(lg) << "Network module test" << ": " << "Starting test server...";

    testServer->startServer();

    BOOST_LOG(lg) << "Network module test" << ": " << "Wait till test's finished...";

    while ( true ) {
        bool clintsTestFinished = !std::count_if( testClients.begin(), testClients.end(), []( auto const& client ) { return !client->isTestFinished(); });        

        if ( clintsTestFinished && testServer->isTestFinished() ) break;        
        std::this_thread::sleep_for( std::chrono::seconds( 1 ));
    }

    BOOST_LOG(lg) << "Network module test" << ": " << "Test clients reconnection...";

    testServer->stopAllClients();

    for ( auto& client : testClients )
        client->resetCounters();

    std::this_thread::sleep_for( std::chrono::seconds( 10 ));

    BOOST_LOG(lg) << "Network module test" << ": " << "Wait till test's finished...";

    while (true) {
        bool clintsTestFinished = !std::count_if(testClients.begin(), testClients.end(), [](auto const& client) { return !client->isTestFinished(); });

        if ( clintsTestFinished && testServer->isTestFinished() ) break;
        std::this_thread::sleep_for( std::chrono::seconds( 1 ));
    }

    BOOST_LOG(lg) << "Network module test" << ": " << "Stopping test client...";

    for ( auto &client : testClients )
        client->stopClient();

    BOOST_LOG(lg) << "Network module test" << ": " << "Stopping test server...";
    testServer->stopServer();
}
