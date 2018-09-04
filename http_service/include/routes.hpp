#pragma once
#include <acorn>
#include <utility>
#include <vector>
#include <net/inet4>
#include <net/http/request.hpp>

namespace chan
{
	using route = std::tuple<http::Method, std::string, mana::End_point>;

	// Returns a vector of all of the routes on the server. Hopefully this is
	// only called once during initialization.
	std::vector<route> get_routes();

	namespace routes
	{
		void acc_register(mana::Request_ptr req, mana::Response_ptr res);
		void acc_login(mana::Request_ptr req, mana::Response_ptr res);
	}
}

