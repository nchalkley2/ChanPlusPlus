#include "database.hpp"
#include "print.hpp"
#include <net/http/client.hpp>
#include <net/inet4>
#include <net/socket.hpp>
#include <net/super_stack.hpp>

#include <optional>

#include <acorn>

#include "byte_helpers.hpp"
#include <type_traits>

// If this is 1 then it will just skip waiting for the database connection
// when the program starts
#define DATABASE_TEST_MODE 0
namespace chan::database
{
	static http::Basic_client* client;
	static net::Socket db_addr({ 10, 0, 0, 131 }, 5000);
	// True if the database is connected

	void
	open(net::Inet<net::IP4>& inet, const delegate<void()> cb)
	{
		using namespace net;

#if (DATABASE_TEST_MODE == 0)
		client = new http::Basic_client{ inet.tcp() };

		if (cb)
			cb();
#else  // DATABASE_TEST_MODE == 1
		printf("Skipping database connection...\n");
		connected = true;

		if (cb)
			cb();
#endif // DATABASE_TEST_MODE == 0
	}

	namespace account
	{
		void
		create(const std::string& username, const std::string& password,
			   const create_callback cb)
		{
			const auto path = "register?username=" + username
							  + "&password=" + password;

			const auto db_cb = http::Response_handler::make_packed(
				[cb, username, path](http::Error err, http::Response_ptr res,
									 http::Connection&) {

					if (not err)
					{
						if (res->status_code() == http::OK)
						{
							if (cb)
								cb(true, username, "");
						}
						else
						{
							if (cb)
								cb(false, username, res->body().data());
						}
					}
					else
					{
						print(path, " - No response: ", err.to_string(), "\n");
						print("Make sure the virtual machine can reach "
							  "internet.\n");

						if (cb)
							cb(false, username,
							   "Failed to connect to database.");
					}
				});

			client->get(db_addr, path, {}, db_cb);
		}

		void
		login(const std::string& username, const std::string& password,
			  const login_callback cb)
		{
			const auto path = "login?username=" + username
							  + "&password=" + password;

			const auto db_cb = http::Response_handler::make_packed(
				[cb, username, path](http::Error err, http::Response_ptr res,
									 http::Connection&) {

					// An ugly wrapper for calling the login_callback with
					// default args
					const auto call_cb
						= [cb](bool successful, std::string err = "",
							   std::pair<std::string, std::string> cookies
							   = std::pair<std::string, std::string>("", "")) {
							  if (cb)
								  cb(successful, err, cookies);
						  };

					if (not err)
					{
						if (res->status_code() == http::OK)
						{
							const auto cookies
								= std::pair<std::string, std::string>(
									"Set-Cookies",
									res->header().value(http::header::Set_Cookie));

							call_cb(true, "", cookies);
						}
						else
						{
							call_cb(false, "Username or password is incorrect.");
						}
					}
					else
					{
						print(path, " - No response: ", err.to_string(), "\n");
						print("Make sure the virtual machine can reach "
							  "internet.\n");

						call_cb(false, "Failed to connect to database.");
					}
				});

			client->get(db_addr, path, {}, db_cb);
		}
	}
}
