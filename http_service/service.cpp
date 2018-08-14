#include <os>

#include <net/inet4>

#include "chan.hpp"

void
Service::start()
{
	const auto& func = chan::start;

	auto& inet = net::Super_stack::get<net::IP4>(0);
	if (not inet.is_configured())
	{
		inet.on_config(func);
	}
	else
	{
		func(inet);
	}
}
