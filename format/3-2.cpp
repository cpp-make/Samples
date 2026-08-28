#include <chrono>
#include <format>
#include <iostream>
#include <map>
#include <ranges>
#include <string>
#include <vector>

int main()
{
	// 整数のデフォルトは 10 進数
	std::cout << std::format("{}", 42) << '\n'; // 42

	// インデックスを使って同じ値を複数回利用
	std::cout << std::format("{0:b} {0:d} {0:o} {0:x}", 42) << '\n'; // 101010 42 52 2a

	// 文字列もそのまま展開できる
	std::cout << std::format("{}", std::string("hello, world")) << '\n';

	// コンテナや範囲も標準で対応 (C++23)
	std::vector<int> v = {1, 2, 3};
	std::map<std::string, int> m = {{"one", 1}, {"two", 2}};
	std::cout << std::format("{}", v) << '\n';                      // [1, 2, 3]
	std::cout << std::format("{}", m) << '\n';                      // {"one": 1, "two": 2}
	std::cout << std::format("{}", std::views::iota(0, 5)) << '\n'; // [0, 1, 2, 3, 4]

	// 日付や時刻もフォーマット可能（strftime 風）
	const auto now = std::chrono::system_clock::now();
	std::cout << std::format("{:%FT%T%z}", now) << '\n'; // 例: 2026-03-30T20:38:57.631822+0000
}
