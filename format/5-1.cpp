#include <format>
#include <iostream>

enum color { red, green, blue };

const char* color_names[] = { "red", "green", "blue" };
const char* jp_color_names[] = { "赤", "緑", "青" };

// color 用の std::formatter を定義
// オプション書式 %j で日本語名を、%e で英語名を出力する
template<>
struct std::formatter<color>
{
	bool is_jp = false;

	// 書式オプションの解析
	// pctx はオプション書式から先を指す範囲。
	// オプションが空でなければ、 *pctx.begin() は : の次の文字
	constexpr auto parse(std::format_parse_context& pctx)
	{
		auto it = pctx.begin();
		if (it != pctx.end() && *it == '%')
		{
			++it;
			if (it == pctx.end())
			{
				throw std::format_error("invalid format");
			}
			else if (*it == 'j')
			{
				is_jp = true;
			}
			else if (*it == 'e')
			{
				is_jp = false;
			}
			else
			{
				throw std::format_error("invalid format");
			}
			++it;
		}
		// オプション書式の次の文字を指すイテレータを返す
		// オプションをサポートしないなら pctx.begin() を返せば OK
		return it;
	}

	// 出力処理
	auto format(color c, std::format_context& fctx) const
	{
		return std::format_to(fctx.out(), "{}",
			is_jp ? jp_color_names[c] : color_names[c]
		);
	}
};

int main()
{
	std::cout << std::format("{:%j} {:%e}", red, blue) << '\n';
}
