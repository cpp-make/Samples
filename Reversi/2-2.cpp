#include <iostream>

// 盤面の幅と高さ
constexpr int HW = 8;

// 黒石が置かれたマスの値
constexpr int BLACK = 0;

// 白石が置かれたマスの値
constexpr int WHITE = 1;

// 空きマスの値
constexpr int EMPTY = 2;

// 盤面を表現するクラス
class Board
{
public:
	// 初期盤面を構築する
	Board()
	{
		for (int y = 0; y < HW; ++y)
		{
			for (int x = 0; x < HW; ++x)
			{
				m_board[y][x] = EMPTY; // 全マスを空にする
			}
		}
		// 石の初期配置
		m_board[3][3] = WHITE;
		m_board[3][4] = BLACK;
		m_board[4][3] = BLACK;
		m_board[4][4] = WHITE;
	}

	// 盤面を出力する
	void print() const
	{
		// 上辺に x 座標 (a-h) を表示
		std::cout << "  ";
		for (int x = 0; x < HW; ++x)
		{
			// x 座標 (a-h)
			std::cout << (char)('a' + x) << " ";
		}
		std::cout << '\n';

		// 左辺の y 座標 (1-8) と盤面の状態を表示
		for (int y = 0; y < HW; ++y)
		{
			// y 座標 (1-8)
			std::cout << 1 + y << " ";

			// 盤面の状態を表示
			for (int x = 0; x < HW; ++x)
			{
				int cellState = m_board[y][x];

				// 黒石、白石、空きマスをそれぞれ X, O, - で表示
				// cellState が 0 なら X, 1 なら O, 2 なら -
				std::cout << "XO-"[cellState] << " ";
			}
			std::cout << '\n';
		}

		// 黒と白それぞれの石数を数える
		int nBlack = 0, nWhite = 0;
		for (int y = 0; y < HW; ++y)
		{
			for (int x = 0; x < HW; ++x)
			{
				int cellState = m_board[y][x];

				// 黒石だったら nBlack に 1 を加算
				nBlack += (cellState == BLACK);

				// 白石だったら nWhite に 1 を加算
				nWhite += (cellState == WHITE);
			}
		}

		// 石数を表示する
		std::cout << "Black " << nBlack << " - " << nWhite << " White\n";

		// 手番を表示
		if (getPlayer() == BLACK)
		{
			std::cout << "Black (X) to move\n";
		}
		else
		{
			std::cout << "White (O) to move\n";
		}
	}

	// 手番を返す
	int getPlayer() const
	{
		return m_player;
	}

	// 現在の手番から見た相手の色を返す
	int getOpponent() const
	{
		// 黒が 0, 白が 1 なので、XOR 1 すると相手の色になる
		return m_player ^ 1;
	}

private:
	int m_board[HW][HW]; // 各マスの状態を表す二次元配列
	int m_player = BLACK; // 手番（次に石を置くプレイヤー）。初期値は黒番
};

int main()
{
	// 初期盤面を作成
	Board board;

	// 盤面を出力
	board.print();
}
