#include <iostream>

int main()
{
    // 入力を行う
    int N, K;
    std::cin >> N >> K;

    // 全部の書き方を調べる
    int ans = 0;
    for (int x = 1; x <= N; x++)
    {
        for (int y = 1; y <= N; y++)
        {
            if (x + y == K) 
            {
                ans += 1; // 合計が K なので、答えに 1 足す
            }
        }
    }

    // 出力を行う
    std::cout << ans << '\n';
}
