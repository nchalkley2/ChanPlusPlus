// This file is a part of the IncludeOS unikernel - www.includeos.org
//
// Copyright 2015-2016 Oslo and Akershus University College of Applied Sciences
// and Alfred Bratterud
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <acorn>
#include <os>

using namespace acorn;

static std::unique_ptr<mana::Server> server_;
static std::unique_ptr<Logger> logger_;
// static content from memdisk
static fs::Disk_ptr disk;

#include <isotime>
#include <optional>
#include <net/inet4>
#include <net/http/request.hpp>
#include <util/percent_encoding.hpp>

namespace
{
inline void
async_send_file(mana::Response_ptr res, std::string path)
{
	disk->fs().cstat(path, [res](const auto err, const auto& entry) {
		if (err)
		{
			res->send_code(http::Not_Found);
		}
		else
		{
			res->send_file({ disk, entry });
		}
	});
};

// Returns true if the username is valid,
// else returns false and an error message
inline std::pair<bool, std::optional<std::string>>
validate_username(const std::string& username)
{
	// TODO: do a remote call to the db and make sure these usernames are unique

	// Username length (this shouldn't be hardcoded but w/e
	if (username.length() < 3)
		return { false, "Username too short" };

	if (username.length() > 15)
		return { false, "Username too long" };

	return { true, std::nullopt };
}

inline std::pair<bool, std::optional<std::string>>
validate_password(const std::string& password,
				  const std::string& password_repeat)
{
	if (password != password_repeat)
		return { false, "Passwords don't match" };

	// Username length (this shouldn't be hardcoded but w/e
	if (password.length() < 3)
		return { false, "Password too short" };

	if (password.length() > 15)
		return { false, "Password too long" };

	return { true, std::nullopt };
}

using namespace mana;
using route = std::tuple<http::Method, std::string, mana::End_point>;
inline std::vector<route>
getRoutes()
{
	using m = http::Method;

	return std::vector<route>{
		route(m::GET, "/",
			  [](auto, auto res) { async_send_file(res, "/index.html"); }),

		route(m::POST, "/acc/register",
			  [](Request_ptr req, Response_ptr res) {
				  http::Request src = req->source();

				  auto username       = uri::decode(src.post_value("username"));
				  auto username_valid = validate_username(username);

				  auto password = uri::decode(src.post_value("password"));
				  auto password_rep
					  = uri::decode(src.post_value("password-repeat"));
				  auto password_valid
					  = validate_password(password, password_rep);

				  if (username_valid.first && password_valid.first)
				  {
				  	 printf("Created account: \"%s\"\n", username.c_str());
					  // TODO:
					  res->source().add_body("Created account!!!\n");
					  res->source().set_status_code(http::Created);
					  res->send();
				  }
				  else
				  {
					  // res->error expects rvalues so this has to be in a
					  // function
					  const auto create_error = [&]() -> std::string&& {
						  const auto username_err
							  = username_valid.second.has_value() ?
									username_valid.second.value() + "\n" : "";

						  const auto password_err
							  = password_valid.second.has_value() ?
									password_valid.second.value() + "\n" : "";

						  return username_err + password_err;
					  };

					  res->error({ http::Bad_Request, "Error creating account",
								   create_error() });
				  }
			  }),

		route(
			m::GET, "/acc/register",
			[](auto, auto res) { async_send_file(res, "/acc/register.html"); })
	};
};
}

static void
start_chan(net::Inet<net::IP4>& inet)
{
	const auto router_init = [&inet]()
	{
		mana::Router router;
		for (const auto& [method, route, func] : getRoutes())
		{
			router.on(method, route, func);
		}

		INFO("Router", "Registered routes:\n%s", router.to_string().c_str());

		/** SERVER SETUP **/
		server_ = std::make_unique<mana::Server>(inet.tcp());
		// set routes and start listening
		server_->set_routes(router).listen(80);
	};

	disk = fs::shared_memdisk();
	disk->init_fs([router_init](fs::error_t err, auto& fs)
	{
		if (err)
		{
			panic("Could not mount filesystem...\n");
		}

		list_static_content(fs);

		router_init();
	});
}

void
Service::start()
{
	const auto& func = start_chan;

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
