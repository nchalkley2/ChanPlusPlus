#include "chan.hpp"
#include "database.hpp"
#include "routes.hpp"
#include <acorn>
#include <net/inet4>

#include <functional>

static std::unique_ptr<mana::Server> server;
static fs::Disk_ptr disk;

namespace chan
{

fs::Disk_ptr
disk()
{
	return ::disk;
}

void
start(net::Inet<net::IP4>& inet)
{
	// These callbacks are in reverse order, so the last one is at the top of
	// this file
	auto router_init = [&inet]()
	{
		mana::Router router;
		for (const auto& [method, route, func] : get_routes())
		{
			router.on(method, route, func);
		}

		INFO("Router", "Registered routes:\n%s", router.to_string().c_str());

		/** SERVER SETUP **/
		server = std::make_unique<mana::Server>(inet.tcp());
		// set routes and start listening
		server->set_routes(router).listen(80);
	};

	// This opens the database and once it has connected it will call
	// router_init
	auto db_init = [&inet, router_init]()
	{
		chan::database::open(inet, router_init);
	};

	// Init the disk first
	::disk = fs::shared_memdisk();
	::disk->init_fs([db_init](fs::error_t err, auto& fs)
	{
		if (err)
		{
			panic("Could not mount filesystem...\n");
		}

		acorn::list_static_content(fs);

		db_init();
	});
}
}
