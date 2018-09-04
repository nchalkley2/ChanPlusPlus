#include "routes.hpp"
#include "database.hpp"
#include "account_validation.hpp"

#include <optional>
#include <string>
#include <utility>

#include <acorn>
#include <net/http/request.hpp>

namespace chan::routes
{
	using namespace mana;
	void
	acc_register(Request_ptr req, Response_ptr res)
	{
		http::Request src = req->source();

		auto username       = uri::decode(src.post_value("username"));
		auto username_valid = validate_username(username);

		auto password       = uri::decode(src.post_value("password"));
		auto password_rep   = uri::decode(src.post_value("password-repeat"));
		auto password_valid = validate_password(password, password_rep);

		// validate_username and validate_password return a <bool, string> pair.
		// The first bool has whether the username/password is valid
		if (username_valid.first && password_valid.first)
		{
			database::account::create(
				username, password,
				[=](bool created_account, std::string username,
					std::string db_err) {
					if (created_account)
					{
						res->source().add_body("Succesfully created account \""
											   + username + "\".");
						res->source().set_status_code(http::Created);
						res->send();
					}
					else
					{
						res->source().add_body("Failed to create account \""
										   + username + "\".\n"
											   + "Error: " + db_err);
						res->source().set_status_code(http::Bad_Request);
						res->send();
					}
				});
		}
		else
		{
			// res->error expects rvalues so this has to be in a
			// function
			const auto create_error = [&]() -> std::string {
				const auto username_err
					= username_valid.second.has_value() ?
						  username_valid.second.value() + "\n" :
						  "";

				const auto password_err
					= password_valid.second.has_value() ?
						  password_valid.second.value() + "\n" :
						  "";

				return username_err + password_err;
			};

			res->error({ http::Bad_Request, "Error creating account",
						 create_error() });
		}
	}
}
