#include <format>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <locale>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <array>
#include <type_traits>
#include <chrono>

template <typename T>
struct is_basic_string_or_string_view_t : std::false_type {};

template <typename CharT, typename Traits>
struct is_basic_string_or_string_view_t<std::basic_string_view<CharT, Traits>> : std::true_type {};

template <typename CharT, typename Traits, typename Alloc>
struct is_basic_string_or_string_view_t<std::basic_string<CharT, Traits, Alloc>> : std::true_type {};

template <typename C, typename T>
concept basic_string_or_string_view_of = is_basic_string_or_string_view_t<T>::value && std::same_as<typename T::value_type, C>;

template<class Context>
class my_basic_format_arg {
public:
private:
	using char_type = typename Context::char_type;

	std::variant<std::monostate, bool, char_type,
			int, unsigned int, long long int, unsigned long long int,
			float, double, long double,
			const char_type*, std::basic_string_view<char_type>,
			const void*> value;

	template<class T>
	static auto get_initial_value(T& v) noexcept {
		using TD = std::remove_const_t<T>;
		if constexpr (std::same_as<TD, bool> || std::same_as<TD, char_type>)
		{
			return v;
		}
		else if constexpr (std::same_as<TD, char> && std::same_as<char_type, wchar_t>)
		{
			return static_cast<wchar_t>(v);
		}
		else if constexpr (std::signed_integral<TD> && sizeof(TD) <= sizeof(int))
		{
			return static_cast<int>(v);
		}
		else if constexpr (std::unsigned_integral<TD> && sizeof(TD) <= sizeof(unsigned int))
		{
			return static_cast<unsigned int>(v);
		}
		else if constexpr (std::signed_integral<TD> && sizeof(TD) <= sizeof(long long int))
		{
			return static_cast<long long int>(v);
		}
		else if constexpr (std::unsigned_integral<TD> && sizeof(TD) <= sizeof(unsigned long long int))
		{
			return static_cast<unsigned long long int>(v);
		}
		else if constexpr (std::floating_point<TD>)
		{
			return v;
		}
		else if constexpr (basic_string_or_string_view_of<char_type, TD>)
		{
			return std::basic_string_view<char_type>(v.data(), v.size());
		}
		else if constexpr (std::same_as<std::decay_t<TD>, char_type*> || std::same_as<std::decay_t<TD>, const char_type*>)
		{
			return static_cast<const char_type*>(v);
		}
		else if constexpr (std::is_void_v<std::remove_pointer_t<TD>> || std::is_null_pointer_v<TD>)
		{
			return static_cast<const void*>(v);
		}
	}

public:
	my_basic_format_arg() noexcept = default;

	template<class T> explicit my_basic_format_arg(T& v) noexcept
		:value(get_initial_value(v))
	{
	}

	explicit operator bool() const noexcept
	{
		return !std::holds_alternative<std::monostate>(value);
	}

	template<class Visitor>
	static auto visit(Visitor&& vis, my_basic_format_arg<Context> arg)
	{
		return std::visit(std::forward<Visitor>(vis), arg.value);
	}
};

template<class Visitor, class Context>
auto my_visit_format_arg(Visitor&& vis, my_basic_format_arg<Context> arg)
{
	return my_basic_format_arg<Context>::visit(std::forward<Visitor>(vis), arg);
}

template<class Context, class... Args>
struct my_format_arg_store
{
	std::array<my_basic_format_arg<Context>, sizeof...(Args)> args;
};

template<class Context, class... Args>
my_format_arg_store<Context, Args...> my_make_format_args(Args&... args)
{
	return {my_basic_format_arg<Context>(args)...};
}

template<class Context>
class my_basic_format_args {
	size_t size_;
	const my_basic_format_arg<Context>* data_;

public:
	my_basic_format_args() noexcept
	  :size_(0)
	{}

	template<class... Args>
	my_basic_format_args(const my_format_arg_store<Context, Args...>& store) noexcept
	  :size_(sizeof...(Args))
	  ,data_(store.args.data())
	{}

	my_basic_format_arg<Context> get(std::size_t i) const noexcept
	{
		return i < size_ ? data_[i] : my_basic_format_arg<Context>();
	}
};

template<class CharT>
struct standard_format_context_for;

template<>
struct standard_format_context_for<char>
{
	using type = std::format_context;
};

template<>
struct standard_format_context_for<wchar_t>
{
	using type = std::wformat_context;
};

template<class CharT>
using standard_format_context_for_t = typename standard_format_context_for<CharT>::type;

template<class T, class CharT>
class my_formatter
{
private:
	std::basic_string_view<CharT> option;
public:
	constexpr auto parse(std::basic_format_parse_context<CharT>& pctx)
	{
		std::formatter<T, CharT> formatter;
		auto begin = pctx.begin();
		auto end = formatter.parse(pctx);
		option = std::basic_string_view<CharT>(begin, end);
		return end;
	}

	template<class Context>
	auto format(const T& value, Context& fctx) const
	{
		static_assert(std::same_as<CharT, typename Context::char_type>);
		using StandardContext = standard_format_context_for_t<CharT>;
		std::basic_string<CharT> format_field(1, static_cast<CharT>('{'));
		if (!option.empty())
		{
			format_field += static_cast<CharT>(':');
			format_field += option;
		}
		format_field += static_cast<CharT>('}');
		return std::vformat_to(fctx.out(), format_field, std::make_format_args<StandardContext>(value));
	}
};

template<class Out, class CharT>
requires std::output_iterator<Out, CharT>
class my_basic_format_context
{
	my_basic_format_args<my_basic_format_context> args_;
	Out out_;

public:
	using iterator = Out;
	using char_type = CharT;
	template<class T> using formatter_type = my_formatter<T, CharT>;

	constexpr my_basic_format_context (
		my_basic_format_args<my_basic_format_context> args,
		iterator out
	)
		: args_(args)
		, out_(out)
	{
	}

	constexpr std::basic_format_arg<my_basic_format_context> arg(size_t id) const
	{
		return args_.get(id);
	}

	constexpr iterator out()
	{
		return std::move(out_);
	}

	constexpr void advance_to(iterator it)
	{
		out_ = std::move(it);
	}
};

// --------------

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
Out my_vformat_to(Out out, std::basic_string_view<CharT> fmt, my_basic_format_args<my_basic_format_context<Out, CharT>> args)
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
			// フォーマッタを呼び出す
			my_visit_format_arg([&]<class T>(const T& value)
			{
				if constexpr (std::same_as<T, std::monostate>)
				{
					throw std::format_error("invalid format");
				}
				else
				{
					typename Context::template formatter_type<T> formatter;
					pctx.advance_to(formatter.parse(pctx));
					fctx.advance_to(formatter.format(value, fctx));
				}
			}, args.get(index));
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
	using Context = my_basic_format_context<Out, char>;
	return my_vformat_to<char, Out>(out, fmt, my_make_format_args<Context>(args...));
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
	std::cout << my_format("{1:#b} + {0:#b} = {2:#b}", 1, 2, 3) << '\n';
	std::cout << my_format("{} + {} = {}", "a", "b", "ab") << '\n';
}
