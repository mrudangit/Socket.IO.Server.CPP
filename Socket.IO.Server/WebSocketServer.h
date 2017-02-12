#pragma once


#include"stdafx.h"
#include"SocketIOClientHandler.h"



using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;



class WebSocketServer
{

	int listenPortNumber;
	WsServer server;

	std::atomic_int numberOfClients;

	std::map<WsServer::connection_ptr, SocketIOClientHandler*> clients;



	void OnWebSocketConnectionClose(WsServer *server, websocketpp::connection_hdl hdl) {
		std::cout << "WsServer WebSocket Connection Closed Handle : "  << hdl.lock().get() << std::endl;

		WsServer::connection_ptr connection =server->get_con_from_hdl(hdl);
		clients.erase(connection);

		 std::cout << "Number of Clients : " << clients.size() << std::endl;

	}

	void OnWebSocketMessage(WsServer *server, websocketpp::connection_hdl hdl, message_ptr msg) {
		std::cout << "WsServer OnWebSocketMessage called with hdl: " << hdl.lock().get()
			<< " and message: " << msg->get_payload()
			<< std::endl;
	}


	void OnWebSocketConnectionOpen(WsServer *server, websocketpp::connection_hdl hdl) {



		std::cout << "WsServer OnWebSocketConnectionOpen New Connection Handle : " << hdl.lock().get() << std::endl;

	
		auto client = new SocketIOClientHandler(numberOfClients++,server, hdl);


		WsServer::connection_ptr connection =server->get_con_from_hdl(hdl);



		clients[connection] = client;


		 std::cout << "Number of Clients : " << clients.size() << std::endl;

	}


public:

	WebSocketServer(int listenPort):listenPortNumber(listenPort)
	{


		server.clear_access_channels(websocketpp::log::alevel::all);

		server.clear_error_channels(websocketpp::log::alevel::all);

		server.set_message_handler(websocketpp::lib::bind(&WebSocketServer::OnWebSocketMessage,this, &server, ::_1, ::_2));

		server.set_open_handler(websocketpp::lib::bind(&WebSocketServer::OnWebSocketConnectionOpen, this, &server, ::_1));

		server.set_close_handler(websocketpp::lib::bind(&WebSocketServer::OnWebSocketConnectionClose, this, &server, ::_1));


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

