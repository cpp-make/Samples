#include <iostream>

int main()
{
    // 入力を行う
    int N;
    std::cin >> N;

    // 二分探索の初期値 (答えは 1 以上 10 億以下)
    int Left = 1;
    int Right = 1000000000;

    // 二分探索を行う
    while (true)
    {
        // 答えが 1 通りに絞られた場合、答えを出力
        if (Left == Right)
        {
            std::cout << Left << '\n';
            break;
        }

        // そうでない場合、真ん中での式の値を確認
        int Mid = (Left + Right) / 2;
        int Value = (Mid / 5) + (Mid / 7) + (Mid / 10);

        // もし Value が N 以上ならば、答えは Mid 以下
        if (Value >= N)
        {
            Right = Mid; // 答えの上限を Mid にする
        }
        // そうでないならば、答えは Mid+1 以上
        else
        {
            Left = Mid + 1; // 答えの下限を Mid + 1 にする
        }
    }
}
