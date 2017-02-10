#pragma once
#include "stdafx.h"
#include "WebSocketServer.h"
#include "SocketIOPacket.h"


using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;


class SocketIOClientHandler
{
	
	int clientId;

	sio::PacketManager packetManager;
	WsServer* server_;
	websocketpp::connection_hdl hdl_;


	void OnMessage(WsServer *server, websocketpp::connection_hdl hdl, message_ptr msg) {
		std::cout << "Client Id " << clientId <<   " SocketIOClientHandler called with hdl: " << hdl.lock().get()
			<< " and message: " << msg->get_payload()
			<< std::endl;

		packetManager.PutPayLoad(msg->get_payload());

	}


	void OnPacketDecode(sio::SocketIOPacket const& packet)
	{
		sio::SocketIOPacket pong(sio::SocketIOPacket::frame_pong);

		std::string payLoad;
		std::cout << "Packet Decoded " << packet.get_frame() << " data : " << packet.get_message() << std::endl;
		switch(packet.get_frame())
		{
		case sio::SocketIOPacket::frame_open: 
			break;
		case sio::SocketIOPacket::frame_close: break;
		case sio::SocketIOPacket::frame_ping: 
			std::cout << "Recvd Ping : " << std::endl;

			

			packetManager.encode(pong,
				[&](bool isBin, std::shared_ptr<const std::string> payload)
			{
				server_->send(hdl_, *payload, websocketpp::frame::opcode::TEXT);
			});




			break;
		case sio::SocketIOPacket::frame_pong: break;
		case sio::SocketIOPacket::frame_message: break;
		case sio::SocketIOPacket::frame_upgrade: break;
		case sio::SocketIOPacket::frame_noop: break;
		default: ;
		}
	}

public:

	SocketIOClientHandler(int clientNumber, WsServer *server, websocketpp::connection_hdl hdl): server_(server), hdl_(hdl)
	{
		clientId = clientNumber;
		clientNumber++;
		WsServer::connection_ptr connection = server->get_con_from_hdl(hdl);
		connection->set_message_handler(bind(&SocketIOClientHandler::OnMessage,this, server, ::_1, ::_2));

		packetManager.SetDecodeCallback(bind(&SocketIOClientHandler::OnPacketDecode,this,_1));

		// Send Handshake
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

		document.AddMember("sid", "12345678", allocator);

		rapidjson::Value array(rapidjson::kArrayType);

		document.AddMember("upgrades", array, allocator);
		document.AddMember("pingInterval", 5000, allocator);
		document.AddMember("pingTimeout", 10000, allocator);


		rapidjson::StringBuffer strbuf;
		rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
		document.Accept(writer);

		


		char        buffer[256] = { '\0' };
		
		



		sprintf_s(buffer,sizeof(buffer),"%d%s", 0, strbuf.GetString());
		std::string handshake(buffer);

		std::cout << "HandShake : "<< handshake << std::endl;


		

		server->send(hdl, handshake,websocketpp::frame::opcode::TEXT);


		sprintf_s(buffer, sizeof(buffer), "%d%d",4,0);


		std::string openFrame(buffer);

		std::cout << "Open : " << openFrame << std::endl;


		server->send(hdl, openFrame, websocketpp::frame::opcode::TEXT);





	}

	virtual ~SocketIOClientHandler()
	{
	}
};

