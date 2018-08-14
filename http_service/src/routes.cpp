#include "chan.hpp"
#include "routes.hpp"
#include "register.hpp"
#include <vector>

inline void
async_send_file(mana::Response_ptr res, std::string path)
{
	chan::disk()->fs().cstat(path, [res](const auto err, const auto& entry) {
		if (err)
		{
			res->send_code(http::Not_Found);
		}
		else
		{
			res->send_file({ chan::disk(), entry });
		}
	});
};

namespace chan
{
using namespace mana;
using route = std::tuple<http::Method, std::string, mana::End_point>;

std::vector<route>
get_routes()
{
	using m = http::Method;

	return std::vector<route>{
		route(m::GET, "/",
			  [](auto, auto res) { async_send_file(res, "/index.html"); }),

		route(m::POST, "/acc/register", routes::acc_register),

		route(
			m::GET, "/acc/register",
			[](auto, auto res) { async_send_file(res, "/acc/register.html"); })
	};
}
}
