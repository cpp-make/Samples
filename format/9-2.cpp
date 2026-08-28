#include <format>
#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <type_traits>
#include <array>
#include <iostream>

// std::basic_string_view または std::basic_string の特殊化かどうかを判定するコンセプト
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
	// TODO class handle;
private:
	using char_type = typename Context::char_type;

	std::variant<std::monostate, bool, char_type,
			int, unsigned int, long long int, unsigned long long int,
			float, double, long double,
			const char_type*, std::basic_string_view<char_type>,
			const void* /*TODO, handle*/> value;

	template<class T>
	static auto get_initial_value(T& v) noexcept {
		using TD = std::remove_const_t<T>;
		// TD が bool なら、v で初期化
		if constexpr (std::same_as<TD, bool> || std::same_as<TD, char_type>)
		{
			return v;
		}
		// TD が char かつ char_type が wchar_t なら、static_cast<wchar_t>(v) で初期化
		else if constexpr (std::same_as<TD, char> && std::same_as<char_type, wchar_t>)
		{
			return static_cast<wchar_t>(v);
		}
		// TD が符号つき整数型かつ int 以下のサイズなら、static_cast<int>(v) で初期化
		else if constexpr (std::signed_integral<TD> && sizeof(TD) <= sizeof(int))
		{
			return static_cast<int>(v);
		}
		// TD が符号なし整数型かつ unsigned int 以下のサイズなら、static_cast<unsigned int>(v) で初期化
		else if constexpr (std::unsigned_integral<TD> && sizeof(TD) <= sizeof(unsigned int))
		{
			return static_cast<unsigned int>(v);
		}
		// TD が符号つき整数型かつ long long int 以下のサイズなら、static_cast<long long int>(v) で初期化
		else if constexpr (std::signed_integral<TD> && sizeof(TD) <= sizeof(long long int))
		{
			return static_cast<long long int>(v);
		}
		// TD が符号なし整数型かつ unsigned long long int 以下のサイズなら、static_cast<unsigned long long int>(v) で初期化
		else if constexpr (std::unsigned_integral<TD> && sizeof(TD) <= sizeof(unsigned long long int))
		{
			return static_cast<unsigned long long int>(v);
		}
		// TD が浮動小数点数なら、v で初期化
		else if constexpr (std::floating_point<TD>)
		{
			return v;
		}
		// TD が char_type を文字とする std::basic_string_view または std::basic_string であれば、
		// std::basic_string_view<char_type>(v.data(), v.size()) で初期化
		else if constexpr (basic_string_or_string_view_of<char_type, TD>)
		{
			return std::basic_string_view<char_type>(v.data(), v.size());
		}
		// std::decay_t<T> が char_type* または const char_type* なら、static_cast<const char_type*>(v) で初期化
		else if constexpr (std::same_as<std::decay_t<TD>, char_type*> || std::same_as<std::decay_t<TD>, const char_type*>)
		{
			return static_cast<const char_type*>(v);
		}
		// TD が void ポインターまたはヌルポインターなら、static_cast<const void*>(v) で初期化
		else if constexpr (std::is_void_v<std::remove_pointer_t<TD>> || std::is_null_pointer_v<TD>)
		{
			return static_cast<const void*>(v);
		}
		// それ以外なら、handle(v) で初期化
		// (まだ実装しない)
	}

public:
	my_basic_format_arg() noexcept = default;

	template<class T> explicit my_basic_format_arg(T& v) noexcept
		:value(get_initial_value(v))
	{
	}

	explicit operator bool() const noexcept {
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

template<class Context, class... Args>
my_format_arg_store<Context, Args...> my_make_format_args(Args&... args)
{
	return {my_basic_format_arg<Context>(args)...};
}

template<class... Args>
void print_args(Args&&... args)
{
	auto arg_store = my_make_format_args<std::format_context>(args...);
	my_basic_format_args<std::format_context> vargs = arg_store;
	for (size_t i = 0; i < sizeof...(args); ++i)
	{
		my_visit_format_arg([]<class T>(const T& value) {
			if constexpr(!std::same_as<std::monostate, T>)
			{
				std::cout << value << std::endl;
			}
			else {
				std::cout << "std::monostate" << std::endl;
			}
		}, vargs.get(i));
	}
}

int main()
{
	print_args(true, 42, 3.14, "hello", nullptr);
}
