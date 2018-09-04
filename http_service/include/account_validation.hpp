#pragma once
#include <string>
#include <optional>

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


	const auto is_alphanumeric = [](const char c) -> bool {
		return ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z')
				|| (c >= 'a' && c <= 'z'));
	};

	for (const auto c : username)
	{
		if (!is_alphanumeric(c))
			return { false,
					 "Username does not contain only alphanumeric characters" };
	}


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

