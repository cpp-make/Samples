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
#include <type_traits>

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

template<class CharT>
std::optional<size_t> parse_digit(CharT c)
{
	switch (c)
	{
		case '0': return 0;
		case '1': return 1;
		case '2': return 2;
		case '3': return 3;
		case '4': return 4;
		case '5': return 5;
		case '6': return 6;
		case '7': return 7;
		case '8': return 8;
		case '9': return 9;
	}
	return std::nullopt;
}

template<std::ranges::input_range InRange>
std::tuple<std::ranges::iterator_t<InRange>, bool>
parse_index(const InRange& in, size_t& out)
{
	out = 0;
	auto i = std::ranges::begin(in);
	for (; i != std::ranges::end(in); ++i)
	{
		auto digit = parse_digit(*i);
		if (!digit)
		{
			break;
		}
		out = out * 10 + *digit;
	}
	if (i == std::ranges::begin(in))
	{
		return {i, false};
	}
	return {i, true};
}

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
			// エスケープされた開き波カッコ
			if (it != pctx.end() && *it == '{')
			{
				*out = '{';
				++it;
				pctx.advance_to(it);
				++out;
				fctx.advance_to(out);
				continue;
			}
			// インデックスを解析する
			size_t index;
			if (auto&& [parsed, ret] = parse_index(pctx, index); ret)
			{
				pctx.check_arg_id(index);
				pctx.advance_to(parsed);
				it = pctx.begin();
			}
			else
			{
				if (parsed != pctx.begin())
				{
					throw std::format_error("invalid format");
				}
				index = pctx.next_arg_id();
			}
			// オプション開始マークを解析する
			if (it != pctx.end() && *it == ':')
			{
				++it;
				pctx.advance_to(it);
			}
			// 引数 1 個をフォーマットする
            fctx.advance_to(std::format_to(fctx.out(), "{}", args[index]));
			// 置換フィールドの終わりを解析する
			it = pctx.begin();
			if (it == pctx.end() || *it != '}')
			{
				throw std::format_error("invalid format");
			}
			++it;
			pctx.advance_to(it);
		}
		else if (*it == '}')
		{
			++it;
			pctx.advance_to(it);
			it = pctx.begin();
			// エスケープされた閉じ波カッコ
			if (it != pctx.end() && *it == '}')
			{
				*out = '}';
				++it;
				pctx.advance_to(it);
				++out;
				fctx.advance_to(out);
				continue;
			}
			throw std::format_error("invalid format");
		}
		else
		{
			// 通常の文字
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
