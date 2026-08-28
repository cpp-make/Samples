#include <iostream>
#include <vector>

int main()
{
	int N;
	std::cin >> N;
	
	// N 個の整数を格納する配列を作成
	std::vector<int> data(N);

	// 標準入力から N 個の整数を読み込む
	for (auto &x : data)
	{
		std::cin >> x;
	}

	// 読み込めたことを確認するために出力
	for (const auto &x : data)
	{
		std::cout << x << ' ';
	}
}
