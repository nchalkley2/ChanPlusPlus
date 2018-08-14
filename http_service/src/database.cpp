#include "database.hpp"
#include <net/inet4>
#include <net/socket.hpp>

#include "account.pb.h"

namespace chan::database
{
void
open(net::Inet<net::IP4>& inet)
{
	using namespace net;
	using Connection_ptr = net::tcp::Connection_ptr;
	using Disconnect     = net::tcp::Connection::Disconnect;

	Socket python_server{ { 10, 0, 0, 131 }, 5347 };

	printf("Trying to connect...\n");
	Connection_ptr db_server = inet.tcp().connect(python_server);
	db_server->on_connect([](Connection_ptr db_server) {
		printf("Connected!\n");

		RegisterAccountReq request;
		request.set_username("Nick");
		request.set_password("nick1234");

		db_server->write(request.SerializeAsString());

		db_server->on_read(1024, [](auto buf) {
			std::string data{ (char*)buf->data(), buf->size() };
			printf("Recieved data!: %s\n", data.c_str());
		});

		db_server->on_disconnect([db_server](auto, auto) {
			printf("Disconnected\n");
			db_server->close();
		});
	});
}
}
