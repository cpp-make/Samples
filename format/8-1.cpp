#include <format>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <array>

template<class Out, class CharT>
requires std::output_iterator<Out, CharT>
class my_basic_format_context
{
	std::span<const int> args_;
	Out out_;

public:
	using iterator = Out;
	using char_type = CharT;

	my_basic_format_context (
		std::span<const int> args,
		iterator out
	)
		: args_(args)
		, out_(out)
	{
	}

	int arg(size_t id) const
	{
		return args_[id];
	}

	iterator out()
	{
		return std::move(out_);
	}

	void advance_to(iterator it)
	{
		out_ = std::move(it);
	}
};

template<class CharT, std::output_iterator<CharT> Out>
Out my_vformat_to(Out out, std::basic_string_view<CharT> fmt, std::span<const int> args)
{
	using Context = my_basic_format_context<Out, CharT>;
	using ParseContext = std::basic_format_parse_context<typename decltype(fmt)::value_type>;
	Context fctx{args, out};
	ParseContext pctx{fmt};

	// はじめ、pctx は fmt と等しい範囲となっている。
	// pctx.advance_to で先頭イテレータを進めることで書式文字列が短くなり、
	// 最後は空になってループが終わる。
	while (!std::ranges::empty(pctx))
	{
		auto it = pctx.begin();
		// 書式文字列をそのまま出力する
		*out = *it;
		// 書式文字列の先頭イテレータを進める
		++it;
		pctx.advance_to(it);
		// 出力イテレータを進める
		++out;
		fctx.advance_to(out);
	}
	return out;
}

template<std::output_iterator<char> Out, class... Args>
Out my_format_to(Out out, std::string_view fmt, Args&&... args)
{
	const std::array args_array = {args...};
	return my_vformat_to<char, Out>(out, fmt, args_array);
}

template<class... Args>
std::string my_format(std::string_view fmt, Args&&... args)
{
	std::string str;
	my_format_to(std::back_inserter(str), fmt, std::forward<Args>(args)...);
	return str;
}

int main()
{
	std::cout << my_format("{} + {} = {}", 1, 2, 3) << '\n';
}
