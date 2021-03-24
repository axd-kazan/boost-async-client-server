#ifndef NETWORKCONST_HPP
#define NETWORKCONST_HPP

#include <boost/log/attributes/constant.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(boostLogger, boost::log::sources::logger_mt)

namespace NetConst {

    enum ClientID {
        ClientSide = -1,
        ServerSide = -2
    };

    enum Defaults {
        ServersPort = 1910,
        ClientsReconnectTimeout = 15000,
        ClientsInitMsgInterval = 10000,
        ClientsPingInterval = 1000,
        ServersInitMsgInterval = 10000,
        ServersDeadlineTimeout = 16000
    };

    enum MessageType {
        Ping = 10,
        Retransmit = 190
    };

}

#endif // NETWORKCONST_HPP
