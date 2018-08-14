#pragma once
#include <net/inet4>

namespace chan::database
{
	// open the socket to the python database server
	void open(net::Inet<net::IP4>& inet);
}
