#include <format>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>

// 第 8 章で自作する関数。この章では標準ライブラリに委譲する。
template<class Out, class... Args>
Out my_vformat_to(Out out, std::format_string<Args...> fmt, Args&&... args)
{
	return std::vformat_to(out, fmt.get(), std::make_format_args(args...));
}

template<class... Args>
std::string my_format(std::format_string<Args...> fmt, Args&&... args)
{
	std::string str;
	// back_inserter 関数を使って back_insert_iterator を作る
	my_vformat_to(std::back_inserter(str), fmt, std::forward<Args>(args)...);
	return str;
}

int main()
{
	std::cout << my_format("{} + {} = {}", 1, 2, 3) << '\n';
}
