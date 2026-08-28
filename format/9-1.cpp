#include <format>
#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <type_traits>
#include <vector>

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

	explicit operator bool() const noexcept {
		return !std::holds_alternative<std::monostate>(value);
	}
};

template<class... Args>
auto test_args(Args&&... args)
{
	using Arg = my_basic_format_arg<std::format_context>;
	return std::vector<Arg> { Arg(args)... };
}

int main()
{
	test_args(true, 42, 3.14, "hello", nullptr);
}
