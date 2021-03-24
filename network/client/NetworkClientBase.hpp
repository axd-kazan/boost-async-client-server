#ifndef NETWORKCLIENTBASE_HPP
#define NETWORKCLIENTBASE_HPP

#include <memory>
#include <thread>
#include <list>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <network/message/NetworkMessage.hpp>
#include <common/Converters.hpp>

class NetworkClientBase : public std::enable_shared_from_this<NetworkClientBase> {
public:
    long long getConnectionDuration();

protected:
    NetworkClientBase(); // Client side
    NetworkClientBase( std::shared_ptr<boost::asio::io_service> service, int clientID ); // Server side
    ~NetworkClientBase();

    std::shared_ptr<boost::asio::io_service> _getService();
    std::shared_ptr<boost::asio::ip::tcp::socket> _getSocket();

    void _startTransfer(); // Set appropriate parameters when connection is reached and enable read/write process
    void _disconnectClient(); // Set appropriate parameters on client disconnection

    void _startTimers(); // Start client timers
    void _stopTimers(); // Stop client timers
    bool _isReconnectRespiteExpired(); // Checking is connection respite time is expired    

    int _getClientID();
    unsigned int _getIpAddress();
    bool _isConnected();

    void _sendMessage( NetworkMessage outbMsg );    

    template <typename T> std::shared_ptr<T> shared_from_base() {
        return std::static_pointer_cast<T>( shared_from_this() );
    }

private:
    boost::log::sources::logger_mt& lg = boostLogger::get(); // Logger
    std::shared_ptr<boost::asio::io_service> _service{};
    std::shared_ptr<boost::asio::ip::tcp::socket> _socket{};
    std::shared_ptr<boost::asio::io_service::strand> _strand{}; // Strand used for tasks execution synchronization
    std::shared_ptr<boost::asio::deadline_timer> _pingTimer{}; // Ping resend timer
    std::shared_ptr<boost::asio::deadline_timer> _initMessageTimer{}; // Initial message resend timer
    std::shared_ptr<boost::asio::deadline_timer> _managementTimer{}; // Management operations timer

    // Maximum time interval between successful transfer operations, if interval is getting longer than this value, then reconnection process is initiated
    unsigned int _reconnectTimeout{ NetConst::Defaults::ClientsReconnectTimeout };
    boost::posix_time::ptime _reconnectRespiteTime{ boost::posix_time::microsec_clock::local_time() };
    boost::posix_time::ptime _startConnectionTime{ boost::posix_time::microsec_clock::local_time() }; // Time when connection was reached

    std::atomic<long long>connectionDuration{};

    // Timers
    const unsigned int _manageLoopFrqency{ 100 };
    unsigned int _initLoopInterval{ NetConst::Defaults::ClientsInitMsgInterval };
    unsigned int _pingInterval{ NetConst::Defaults::ClientsPingInterval };

    bool _connected{}; // Connection condition flag
    unsigned long _ipAddress{}; // Clients IP address, initialized when connection is reached
    int _clientID{}; // Client ID, used on server's side, to determine which client own the message

    std::deque<NetworkMessage> _outputQueue; // Queue for outgoing messages

    unsigned long _loadIpAddress();

    // Timers
    void _tickPingLoop( unsigned int timer );
    void _tickInitMsgLoop( unsigned int timer );
    void _tickManageLoop( unsigned int timer );
    void _pingHandler( const boost::system::error_code &err );
    void _initMsgHandler( const boost::system::error_code &err );
    void _manageLoopHandler( const boost::system::error_code &err );

    // Pure virtual functions that must be overloaded by derived class
    virtual void _initialLoop() = 0; // Send set of initialization messages
    virtual void _managementLoop() = 0; // Menegement operations
    virtual void _confirmTransfer( NetworkMessage netMsg ) = 0; // Confirm that message transferred
    virtual void _processMessage( NetworkMessage netMsg ) = 0; // Process received messages
    virtual void _clientDisconnected() = 0; // Disconnection signal

    // Read
    void _readFromSocket();
    void _handleReadHeader( const boost::system::error_code &err, size_t bytesTrans, std::shared_ptr<std::vector<uint8_t>> buffer );
    void _handleReadSize( const boost::system::error_code &err, size_t bytesTrans, NetworkMessage netMsg, std::shared_ptr<std::vector<uint8_t> > inbTailSize );
    void _handleReadTail( const boost::system::error_code& err, size_t bytesTrans, NetworkMessage netMsg, std::shared_ptr<std::vector<uint8_t> > inbTailSize, std::shared_ptr<std::vector<uint8_t>> inbData );
    // Send
    void _handlerFillBuffer( NetworkMessage outbMsg );
    void _writeToSocket();
    void _handleWriteToSocket( const boost::system::error_code &err, size_t bytesTrans, NetworkMessage outbMsg );
};

#endif /* NETWORKCLIENTBASE_HPP */

