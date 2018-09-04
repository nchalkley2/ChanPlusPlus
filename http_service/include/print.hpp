#include <iostream>

template<typename... Args>
void
print(Args&&... args)
{
	(std::cout << ... << args);
}
