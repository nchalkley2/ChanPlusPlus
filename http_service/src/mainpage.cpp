#include "routes.hpp"
#include "print.hpp"

#include <acorn>

namespace chan::routes
{
	using namespace mana;

	void
	mainpage(Request_ptr req, Response_ptr res)
	{
		http::Request src = req->source();
	}
}
