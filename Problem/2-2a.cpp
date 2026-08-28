#include <iostream>

int main()
{
    // 入力を行う
    int N;
    std::cin >> N;

    // 答えを調べる
    for (int x = 1; x <= 1000000000; x++)
    {
        if ((x / 5) + (x / 7) + (x / 10) >= N)
        {
            std::cout << x << '\n';
            break;
        }
    }
}
