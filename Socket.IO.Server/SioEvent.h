#pragma once
#include "SocketIOMessage.h"

class SioEvent
{
public:

	const std::string& get_nsp() const;

	const std::string& get_name() const;

	const sio::message::ptr& get_message() const;

	const sio::message::list& get_messages() const;

	bool need_ack() const;

	void put_ack_message(sio::message::list const& ack_message);

	sio::message::list const& get_ack_message() const;

protected:
	SioEvent(std::string const& nsp, std::string const& name, sio::message::list const& messages, bool need_ack);
	SioEvent(std::string const& nsp, std::string const& name, sio::message::list&& messages, bool need_ack);

	sio::message::list& get_ack_message_impl();

private:
	const std::string m_nsp;
	const std::string m_name;
	const sio::message::list m_messages;
	const bool m_need_ack;
	sio::message::list m_ack_message;

	friend class SioEventAdapter;
};

