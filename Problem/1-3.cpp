#include <iostream>

int main()
{
    // 入力を行う
    int N, K;
    std::cin >> N >> K;

    // 白のカードと灰のカードを全探索
    int ans = 0;
    for (int x = 1; x <= N; x++)
    {
        for (int y = 1; y <= N; y++)
        {
            int z = K - x - y; // 黒のカードが自動的に決まる
            if (z >= 1 && z <= N)
            {
                ans += 1; // 範囲内の場合、答えに 1 を加算
            }
        }
    }
    
    // 出力を行う
    std::cout << ans << '\n';
}
