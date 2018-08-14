#pragma once
#include <acorn>
#include <net/inet4>

namespace chan
{
// TODO: see if I can forward declare this ptr
fs::Disk_ptr disk();

void start(net::Inet<net::IP4>& inet);
}
