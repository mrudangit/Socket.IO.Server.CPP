#include "stdafx.h"
#include "SioEvent.h"



const std::string& SioEvent::get_nsp() const
{
	return m_nsp;
}

const std::string& SioEvent::get_name() const
{
	return m_name;
}

const sio::message::ptr& SioEvent::get_message() const
{
	if (m_messages.size()>0)
		return m_messages[0];
	else
	{
		static sio::message::ptr null_ptr;
		return null_ptr;
	}
}

const sio::message::list& SioEvent::get_messages() const
{
	return m_messages;
}

bool SioEvent::need_ack() const
{
	return m_need_ack;
}

void SioEvent::put_ack_message(sio::message::list const& ack_message)
{
	if (m_need_ack)
		m_ack_message = std::move(ack_message);
}


SioEvent::SioEvent(std::string const& nsp, std::string const& name, sio::message::list&& messages, bool need_ack) :
	m_nsp(nsp),
	m_name(name),
	m_messages(std::move(messages)),
	m_need_ack(need_ack)
{
}


SioEvent::SioEvent(std::string const& nsp, std::string const& name, sio::message::list const& messages, bool need_ack) :
	m_nsp(nsp),
	m_name(name),
	m_messages(messages),
	m_need_ack(need_ack)
{
}