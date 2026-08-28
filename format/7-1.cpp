#include <iterator>
#include <string>
#include <span>

template<class Out, class CharT>
requires std::output_iterator<Out, CharT>
class my_basic_format_context
{
	std::span<int> args_;
	Out out_;

public:
	using iterator = Out;
	using char_type = CharT;

	my_basic_format_context (
		iterator out,
		std::span<int> args
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

int main()
{
	std::string str;
	using Context = my_basic_format_context<std::back_insert_iterator<std::string>, char>;
	Context context{std::back_inserter(str), std::span<int>()};
	context.advance_to(context.out());
}
