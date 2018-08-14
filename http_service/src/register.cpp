#include "register.hpp"
#include <string>
#include <utility>
#include <optional>

#include <acorn>
#include <net/http/request.hpp>

// Returns true if the username is valid, else returns false and an error
// message
//
// This isn't the complete validation, as registration can still fail if the
// database returns an error on creating the account
static inline std::pair<bool, std::optional<std::string>>
validate_username(const std::string& username)
{
	// Username length (this shouldn't be hardcoded but w/e
	if (username.length() < 3)
		return { false, "Username too short" };

	if (username.length() > 15)
		return { false, "Username too long" };

	return { true, std::nullopt };
}

// Returns true if the password is valid, else returns false and an error
// message
static inline std::pair<bool, std::optional<std::string>>
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

namespace chan::routes
{
using namespace mana;
using m = http::Method;
void 
acc_register(Request_ptr req, Response_ptr res)
{
	http::Request src = req->source();

	auto username       = uri::decode(src.post_value("username"));
	auto username_valid = validate_username(username);

	auto password      = uri::decode(src.post_value("password"));
	auto password_rep  = uri::decode(src.post_value("password-repeat"));
	auto password_valid = validate_password(password, password_rep);

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
		const auto create_error = [&]() -> std::string&&
		{
			const auto username_err = username_valid.second.has_value() ?
										  username_valid.second.value() + "\n" :
										  "";

			const auto password_err = password_valid.second.has_value() ?
										  password_valid.second.value() + "\n" :
										  "";

			return username_err + password_err;
		};

		res->error(
			{ http::Bad_Request, "Error creating account", create_error() });
	}
}
}
