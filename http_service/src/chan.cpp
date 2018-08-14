#include "chan.hpp"
#include "database.hpp"
#include "routes.hpp"
#include <acorn>
#include <net/inet4>

static std::unique_ptr<mana::Server> server_;
static fs::Disk_ptr disk;

namespace chan
{
fs::Disk_ptr
disk()
{
	return ::disk;
}

using namespace acorn;
void start(net::Inet<net::IP4>& inet)
{
	const auto router_init = [&inet]()
	{
		mana::Router router;
		for (const auto& [method, route, func] : get_routes())
		{
			router.on(method, route, func);
		}

		INFO("Router", "Registered routes:\n%s", router.to_string().c_str());

		/** SERVER SETUP **/
		server_ = std::make_unique<mana::Server>(inet.tcp());
		// set routes and start listening
		server_->set_routes(router).listen(80);

		database::open(inet);

	};

	::disk = fs::shared_memdisk();
	::disk->init_fs([router_init](fs::error_t err, auto& fs)
	{
		if (err)
		{
			panic("Could not mount filesystem...\n");
		}

		list_static_content(fs);

		router_init();
	});
}
}
