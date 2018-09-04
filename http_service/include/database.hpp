#pragma once
#include <net/inet4>
#include <utility>

namespace chan::database
{
	// Open the socket to the python database server
	// calls the callback once connected
	void open(net::Inet<net::IP4>& inet, const delegate<void()> cb = nullptr);


	namespace account
	{
		// params: true if the account was created, username, db_error
		using create_callback = delegate<void(bool, std::string, std::string)>;
		void create(const std::string& username, const std::string& password,
					const create_callback cb = nullptr);

		using login_callback = delegate<void(bool, std::string, std::pair<std::string, std::string>)>;
		void login(const std::string& username, const std::string& password,
				   const login_callback cb = nullptr);
	}
}
