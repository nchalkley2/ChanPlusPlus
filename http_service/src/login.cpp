#include "routes.hpp"
#include "database.hpp"
#include "print.hpp"

#include <acorn>
#include <net/http/request.hpp>
#include <net/http/cookie.hpp>

namespace chan::routes
{
	using namespace mana;

	static std::map<std::string, std::string>
	parse_cookies(const std::string& cookie_data)
	{
		const std::regex cookie_pattern_{ "[^;]+" };
		auto position = std::sregex_iterator(
			cookie_data.begin(), cookie_data.end(), cookie_pattern_);
		auto end = std::sregex_iterator();
		
		std::map<std::string, std::string> cookies;

		while (position not_eq end)
		{
			auto cookie = (*position++).str();

			cookie.erase(std::remove(cookie.begin(), cookie.end(), ' '),
						 cookie.end());


			using pair_t = std::pair<std::string, std::string>;
			const auto pos = cookie.find('=');
			if (pos not_eq std::string::npos)
			{
				pair_t kv(cookie.substr(0, pos), cookie.substr(pos + 1));
				cookies.insert(kv);
			}
			else
			{
				pair_t kv(cookie, std::string(""));
				cookies.insert(kv);
			}
		}

		return cookies;
	}

	void
	acc_login(Request_ptr req, Response_ptr res)
	{
		http::Request src = req->source();
		const auto username     = uri::decode(src.post_value("username"));
		const auto password     = uri::decode(src.post_value("password"));

		print(src.to_string(), "\n");

		database::account::login(
			username, password,
			[=](bool login_sucessful, std::string err,
				std::pair<std::string, std::string> cookies) {
				if (login_sucessful)
				{
					res->source().add_body("Succesfully logged in.");
					res->source().set_status_code(http::OK);

					const auto cookie_map = parse_cookies(cookies.second);
					for (const auto& [k, v] : cookie_map)
					{
						print("k: ", k, " v: ", v, "\n");
						if (k == "session_id")
						{
							auto session_id = http::Cookie(k, v);
							res->cookie(session_id);
						}
					}
					
					res->send();
				}
				else
				{
					res->source().add_body(std::string("Failed to login.\n")
										   + "Error: " + err);
					res->source().set_status_code(http::Bad_Request);
					res->send();
				}
			});
	}
}
