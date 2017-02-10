#pragma once

#include"stdafx.h"
#include"SocketIOClientHandler.h"


using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;



class WebSocketServer
{

	int listenPortNumber;
	WsServer server;
	std::list<SocketIOClientHandler*> clients;
	int numberOfClients;

	void OnWebSocketMessage(WsServer *server, websocketpp::connection_hdl hdl, message_ptr msg) {
		std::cout << "OnWebSocketMessage called with hdl: " << hdl.lock().get()
			<< " and message: " << msg->get_payload()
			<< std::endl;
	}


	void OnWebSocketConnectionOpen(WsServer *server, websocketpp::connection_hdl hdl) {
		
		std::cout << "OnWebSocketConnectionOpen New Connection Handle : " << hdl.lock().get() << std::endl;

	
		auto client = new SocketIOClientHandler(numberOfClients++,server, hdl);
		clients.push_back(client);

	}


public:

	WebSocketServer(int listenPort):listenPortNumber(listenPort)
	{

		
		
		server.clear_access_channels(websocketpp::log::alevel::all);

		server.clear_error_channels(websocketpp::log::alevel::all);

		server.set_message_handler(bind(&WebSocketServer::OnWebSocketMessage,this, &server, ::_1, ::_2));

		server.set_open_handler(bind(&WebSocketServer::OnWebSocketConnectionOpen, this, &server, ::_1));

	}

	~WebSocketServer()
	{
	}


	void Start() {

		try {

			server.init_asio();
			server.listen(listenPortNumber);
			server.start_accept();

			std::cout << "Server Listening on Port : " << listenPortNumber << std::endl;
			server.run();


		}
		catch (websocketpp::exception const & e) {
			std::cout << e.what() << std::endl;
		}
		catch (...) {
			std::cout << "other exception" << std::endl;
		}

	}

};

