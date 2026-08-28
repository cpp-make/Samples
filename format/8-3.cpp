#include <format>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
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

	while (!std::ranges::empty(pctx))
	{
		auto it = pctx.begin();
		if (*it == '{')
		{
			++it;
			pctx.advance_to(it);
			it = pctx.begin();
			// 置換フィールドを見つけたことを通知し、インデックスを取得する
			const size_t index = pctx.next_arg_id();
			// 引数 1 個をフォーマットする
			fctx.advance_to(std::format_to(fctx.out(), "{}", args[index]));
			// '{' の次の文字が無い、または '}' でない場合はエラー
			it = pctx.begin();
			if (it == pctx.end() || *it != '}')
			{
				throw std::format_error("invalid format");
			}
			++it;
			pctx.advance_to(it);
		}
		else
		{
			*out = *it;
			++it;
			pctx.advance_to(it);
			++out;
			fctx.advance_to(out);
		}
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
