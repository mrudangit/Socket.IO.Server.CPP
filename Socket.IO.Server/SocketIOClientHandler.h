#pragma once
#include "stdafx.h"
#include "WebSocketServer.h"
#include "SocketIOPacket.h"
#include "SioEvent.h"
#include "SioEventAdapter.h"

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
		std::cout << "Client Id " << clientId << " SocketIOClientHandler called with hdl: " << hdl.lock().get()
			<< " and message: " << msg->get_payload()
			<< std::endl;

		packetManager.PutPayLoad(msg->get_payload());

	}


	void SendPong()
	{
		sio::SocketIOPacket pong(sio::SocketIOPacket::frame_pong);

		std::string pongPayLoad;


		packetManager.encode(pong,
			[&](bool isBin, std::shared_ptr<const std::string> pongPayLoad)
		{
			server_->send(hdl_, *pongPayLoad, websocketpp::frame::opcode::TEXT);
		});


	}

	void HandlleConnect(std::string& nsp)
	{

		sio::SocketIOPacket connect(sio::SocketIOPacket::type_connect, nsp);

		std::string connectPayLoad;
		packetManager.encode(connect,
			[&](bool isBin, std::shared_ptr<const std::string> connectPayLoad)
		{
			server_->send(hdl_, *connectPayLoad, websocketpp::frame::opcode::TEXT);
		});
	}


	void HandleEvent(sio::SocketIOPacket const& eventPacket)
	{
		const sio::message::ptr ptr = eventPacket.get_message();

		if(ptr->get_flag() ==  sio::message::flag_array)
		{
			const sio::array_message* array_ptr = static_cast<const sio::array_message*>(ptr.get());
			if (array_ptr->get_vector().size() >= 1 && array_ptr->get_vector()[0]->get_flag() == sio::message::flag_string)
			{
				const sio::string_message* name_ptr = static_cast<const sio::string_message*>(array_ptr->get_vector()[0].get());
				sio::message::list mlist;
				for (size_t i = 1;i<array_ptr->get_vector().size();++i)
				{
					mlist.push(array_ptr->get_vector()[i]);
				}
				this->CreateEvent(eventPacket.get_nsp(), eventPacket.get_pack_id(), name_ptr->get_string(), std::move(mlist));
			}
		}

	}


	void CreateEvent(const std::string& nsp, int msgId, const std::string& name, sio::message::list && message)
	{


		
		bool needAck = msgId >= 0;
		SioEvent ev = SioEventAdapter::create_event(nsp, name, std::move(message), needAck);

		auto mmp= (std::map<std::string, sio::message::ptr> &&) ev.get_message()->get_map();

		std::cout << "Property A" << mmp["propA"]->get_string() << std::endl;


		

		

	}

	void HandleMessage(sio::SocketIOPacket const& msgPacket)
	{
		sio::SocketIOPacket::type msgType = msgPacket.get_type();
		std::string nameSpace = msgPacket.get_nsp();
		sio::message::ptr const msg = msgPacket.get_message();

		
	

		switch (msgType)
		{

		case sio::SocketIOPacket::type_connect:
			HandlleConnect(nameSpace);
			break;
		case sio::SocketIOPacket::type_disconnect: break;
		case sio::SocketIOPacket::type_event: 
			HandleEvent(msgPacket);

			break;
		case sio::SocketIOPacket::type_ack: break;
		case sio::SocketIOPacket::type_error: break;
		case sio::SocketIOPacket::type_binary_event: break;
		case sio::SocketIOPacket::type_binary_ack: break;
			break;
		case sio::SocketIOPacket::type_undetermined: break;
		default:;
		}



	}


	void OnPacketDecode(sio::SocketIOPacket const& packet)
	{

		std::cout << "Packet Decoded " << packet.get_frame()
			<< " data : " << packet.get_message()
			<< " NameSpace :" << packet.get_nsp()

			<< std::endl;

		switch (packet.get_frame())
		{


		case sio::SocketIOPacket::frame_open:
			std::cout << "Recvd Open" << std::endl;
			break;
		case sio::SocketIOPacket::frame_close: break;
		case sio::SocketIOPacket::frame_ping:
			std::cout << "Recvd Ping : " << std::endl;
			SendPong();
			break;
		case sio::SocketIOPacket::frame_pong: break;
		case sio::SocketIOPacket::frame_message:
			std::cout << "Recvd Message" << " Packet Type : " << packet.get_type() << std::endl;
			HandleMessage(packet);
			break;
		case sio::SocketIOPacket::frame_upgrade: break;
		case sio::SocketIOPacket::frame_noop: break;
		default:;
		}
	}

public:

	SocketIOClientHandler(int clientNumber, WsServer *server, websocketpp::connection_hdl hdl) : server_(server), hdl_(hdl)
	{
		clientId = clientNumber;
		clientNumber++;
		WsServer::connection_ptr connection = server->get_con_from_hdl(hdl);
		connection->set_message_handler(bind(&SocketIOClientHandler::OnMessage, this, server, ::_1, ::_2));

		packetManager.SetDecodeCallback(bind(&SocketIOClientHandler::OnPacketDecode, this, _1));

		// Send Handshake
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& allocator = document.GetAllocator();


		boost::uuids::uuid uuid = boost::uuids::random_generator()();
		std::string sid = boost::uuids::to_string(uuid);
		document.AddMember("sid", rapidjson::StringRef(sid.c_str()), allocator);

		rapidjson::Value array(rapidjson::kArrayType);

		document.AddMember("upgrades", array, allocator);
		document.AddMember("pingInterval", 50000, allocator);
		document.AddMember("pingTimeout", 10000, allocator);


		rapidjson::StringBuffer strbuf;
		rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
		document.Accept(writer);




		char        buffer[256] = { '\0' };






#ifdef _WIN32
		sprintf_s(buffer, sizeof(buffer), "%d%s", 0, strbuf.GetString());
#else
        sprintf(buffer, "%d%s", 0, strbuf.GetString());
#endif
		std::string handshake(buffer);

		std::cout << "HandShake : " << handshake << std::endl;




		server->send(hdl, handshake, websocketpp::frame::opcode::TEXT);

#ifdef _WIN32
		sprintf_s(buffer, sizeof(buffer), "%d%d", 4, 0);
#else
		sprintf(buffer, "%d%d", 4, 0);
#endif

		std::string openFrame(buffer);

		std::cout << "Open : " << openFrame << std::endl;


		server->send(hdl, openFrame, websocketpp::frame::opcode::TEXT);





	}

	virtual ~SocketIOClientHandler()
	{
	}
};

