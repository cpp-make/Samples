#include <cstddef>
#include <format>
#include <iostream>
#include <iterator>
#include <utility>

template<class CharT>
class Counter
{
	size_t count_ = 0;
public:
	using value_type = CharT;

	constexpr void push_back(const value_type&)
	{
		count_++;
	}

	constexpr size_t size() const
	{
		return count_;
	}
};

// 第 8 章で自作する関数。この章では標準ライブラリに委譲する。
template<class Out, class... Args>
Out my_vformat_to(Out out, std::format_string<Args...> fmt, Args&&... args)
{
	return std::vformat_to(out, fmt.get(), std::make_format_args(args...));
}

template<class... Args>
size_t my_formatted_size(std::format_string<Args...> fmt, Args&&... args)
{
	Counter<char> counter;
	my_vformat_to(std::back_inserter(counter), fmt, std::forward<Args>(args)...);
	return counter.size();
}

int main()
{
	std::cout << my_formatted_size("{} + {} = {}", 1, 2, 3) << '\n';
}
