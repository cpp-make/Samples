#include <boost/format.hpp> // Boost Format Library
#include <fmt/format.h>    // {fmt}

#include <format>
#include <iostream>

int main()
{
	// Boost Format Library
	std::cout << boost::format("Hello! %1%, %2%\n") % 12345 % 3.14;

	// {fmt}
	std::cout << fmt::format("Hello! {}, {}\n", 12345, 3.14);

	// <format>
	std::cout << std::format("Hello! {}, {}\n", 12345, 3.14);
}
