#pragma once
#include "SocketIOMessage.h"
#include <functional>
#include "SioEvent.h"

class SioEventAdapter
{
public:

	
	static inline SioEvent create_event(std::string const& nsp, std::string const& name, sio::message::list&& message, bool need_ack)
	{
		return SioEvent(nsp, name, message, need_ack);
	}


};

