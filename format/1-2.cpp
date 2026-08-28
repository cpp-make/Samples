#include <iostream>

int main()
{
	std::cout << true << ' ' << false << '\n'; // 1 0

	std::cout << std::boolalpha; // cout の状態を変更する
	std::cout << true << ' ' << false << '\n'; // true false

	std::cout << std::noboolalpha; // cout の状態を元に戻す
	std::cout << true << ' ' << false << '\n'; // 1 0
}
