#include <iostream>
#include <optional>

// 盤面の幅と高さ
constexpr int HW = 8;

// 黒石が置かれたマスの値
constexpr int BLACK = 0;

// 白石が置かれたマスの値
constexpr int WHITE = 1;

// 空きマスの値
constexpr int EMPTY = 2;

// 座標が盤面内にあるかを判定
bool InBounds(int y, int x)
{
	return 0 <= y && y < HW && 0 <= x && x < HW;
}

// 着手を表現するクラス
struct Move
{
public:
	// 着手座標
	int y;
	int x;

	// 個々の石について、返るかどうかの判定結果（すべて false で初期化）
	bool flipped[HW][HW] = {};

	Move(int inY, int inX)
		: y(inY), x(inX) {}
};

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

	// 返る石を求める
	std::optional<Move> calcFlipped(int y, int x) const
	{
		// 空きマスでなければ着手できないため、返る石はない
		if (m_board[y][x] != EMPTY)
		{
			return std::nullopt;
		}

		Move result(y, x);

		// 周囲 8 方向にアクセスするための、y, x の変化量
		constexpr int dy[8] = { 1, -1, 0, 0, 1, 1, -1, -1 };
		constexpr int dx[8] = { 0, 0, 1, -1, 1, -1, 1, -1 };

		// 返る石が見つかったかどうか
		bool flipFound = false;

		for (int i = 0; i < 8; ++i)
		{
			// 相手の石が見つかったかどうか
			bool opponentDiscFound = false;

			for (int j = 1; j < HW; ++j)
			{
				// 調べるマスの座標
				int y1 = y + dy[i] * j;
				int x1 = x + dx[i] * j;

				// 盤面外に出ていたら break
				if (!InBounds(y1, x1))
				{
					break;
				}

				if (m_board[y1][x1] == getOpponent())
				{
					// 相手の石が見つかった
					opponentDiscFound = true;
				}
				else if (m_board[y1][x1] == getPlayer())
				{
					if (opponentDiscFound)
					{
						// 自分の石で挟めた
						for (int k = 1; k < j; ++k)
						{
							int y2 = y + dy[i] * k;
							int x2 = x + dx[i] * k;

							// 挟まれた石は返る
							result.flipped[y2][x2] = true;
						}

						flipFound = true;
					}

					break;
				}
				else if (m_board[y1][x1] == EMPTY)
				{
					// 空きマスがあったらbreak
					break;
				}
			}
		}

		if (flipFound)
		{
			return result;
		}
		else
		{
			// どの石も返らなかったら nullopt を返す
			return std::nullopt;
		}
	}

	// 着手する
	void move(const Move& move)
	{
		// 着手した座標に石を置く
		m_board[move.y][move.x] = getPlayer();

		// 石を返す
		for (int y = 0; y < HW; ++y)
		{
			for (int x = 0; x < HW; ++x)
			{
				if (move.flipped[y][x])
				{
					m_board[y][x] = getPlayer();
				}
			}
		}

		// 手番を交代する
		m_player ^= 1;
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
