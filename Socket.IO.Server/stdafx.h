// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>


#ifdef  _WIN32
#include <tchar.h>
#endif


#include <iostream>
#include <atomic>
#include <list>

// TODO: reference additional headers your program requires here




#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <websocketpp/config/asio_no_tls.hpp>

#include <websocketpp/server.hpp>


typedef websocketpp::server<websocketpp::config::asio> WsServer;
typedef WsServer::message_ptr message_ptr;
