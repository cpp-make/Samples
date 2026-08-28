#include <iostream>
#include <optional>
#include <vector>
#include <random>
#include <string>

// 盤面の幅と高さ
constexpr int HW = 8;

// 黒石が置かれたマスの値
constexpr int BLACK = 0;

// 白石が置かれたマスの値
constexpr int WHITE = 1;

// 空きマスの値
constexpr int EMPTY = 2;

// スコアの最大値
constexpr int MAXSCORE = 64;

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
	
	// 評価値
	int value;
	
	Move(int inY, int inX, int inValue)
		: y(inY), x(inX), value(inValue) {}

	// "f5" のように座標を文字列で返す
	std::string toString() const
	{
		return "abcdefgh"[x] + std::to_string(y + 1);
	}
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

		Move result(y, x, 0);

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

	// 着手前の盤面に戻す関数
	void undo(Move& move)
	{
		// 手番を戻す
		m_player ^= 1;

		// 返した石を戻す
		for (int y = 0; y < HW; ++y)
		{
			for (int x = 0; x < HW; ++x)
			{
				if (move.flipped[y][x])
				{
					m_board[y][x] = getOpponent();
				}
			}
		}

		// 置いた石を戻す
		m_board[move.y][move.x] = EMPTY;
	}

	// 合法手の一覧を作成
	std::vector<Move> getLegalMoves() const
	{
		std::vector<Move> legalMoves;

		for (int y = 0; y < HW; ++y)
		{
			for (int x = 0; x < HW; ++x)
			{
				// moveがstd::nulloptでなかったら合法手
				if (const auto move = calcFlipped(y, x))
				{
					legalMoves.push_back(*move);
				}
			}
		}

		return legalMoves;
	}

	// パス処理
	void pass()
	{
		// 手番のみを入れ替える
		m_player = getOpponent();
	}

	// 評価関数
	int evaluate() const
	{
		constexpr int cellWeight[HW][HW] = {
			{2714,  147,   69,  -18,  -18,   69,  147, 2714},
			{ 147, -577, -186, -153, -153, -186, -577,  147},
			{  69, -186, -379, -122, -122, -379, -186,   69},
			{ -18, -153, -122, -169, -169, -122, -153,  -18},
			{ -18, -153, -122, -169, -169, -122, -153,  -18},
			{  69, -186, -379, -122, -122, -379, -186,   69},
			{ 147, -577, -186, -153, -153, -186, -577,  147},
			{2714,  147,   69,  -18,  -18,   69,  147, 2714}
		};
		int value = 0;

		// すべてのマスをチェックする
		for (int y = 0; y < HW; ++y)
		{
			for (int x = 0; x < HW; ++x)
			{
				if (m_board[y][x] == getPlayer())
				{
					// 自分の石なら加点する
					value += cellWeight[y][x];
				}
				else if (m_board[y][x] == getOpponent())
				{
					// 相手の石なら減点する
					value -= cellWeight[y][x];
				}
			}
		}
		// 四捨五入
		value += (value > 0 ? 128 : (value < 0 ? -128 : 0));
		// 重みは最終石差の256倍基準だったため、256で割る
		value /= 256;

		// 範囲 [-64, 64] に収める
		return std::clamp(value, -MAXSCORE, MAXSCORE);
	}

	// プレイヤー視点の石差を計算する
	int score() const
	{
		int nPlayer = 0;
		int nOpponent = 0;
		int nEmpty = 0;
		// 64マス全てを順番に見ていく
		for (int y = 0; y < HW; ++y)
		{
			for (int x = 0; x < HW; ++x)
			{
				if (m_board[y][x] == getPlayer())
				{
					++nPlayer;
				}
				else if (m_board[y][x] == getOpponent())
				{
					++nOpponent;
				}
				else
				{
					++nEmpty;
				}
			}
		}

		int score = nPlayer - nOpponent;
		if (nPlayer > nOpponent)
		{
			score += nEmpty;
		}
		else if (nPlayer < nOpponent)
		{
			score -= nEmpty;
		}
		return score;
	}

private:
	int m_board[HW][HW]; // 各マスの状態を表す二次元配列
	int m_player = BLACK; // 手番（次に石を置くプレイヤー）。初期値は黒番
};

// コンソールから座標を入力する
Move InputMove(const std::vector<Move>& legalMoves)
{
	char c;
	int n;
	for (;;)
	{
		std::cout << "Input Move: ";
		// cにアルファベット、nに整数を入力
		std::cin >> c >> n;
		// 入力を座標に変換する
		int y = n - 1;
		int x = c - 'a';

		// 入力が合法手に含まれていれば、その手を返す
		for (const Move& move : legalMoves)
		{
			if (move.y == y && move.x == x)
			{
				return move;
			}
		}
	}
}

// ランダムに手を選ぶAI
Move AIRandom(const Board& board, const std::vector<Move>& legalMoves)
{
	static std::mt19937 engine{ std::random_device{}() };
	std::uniform_int_distribution<size_t> dist(0, legalMoves.size() - 1);
	return legalMoves[dist(engine)];
}

// 1手読みで着手する AI
Move AI1Move(Board board, std::vector<Move> legalMoves)
{
	Move bestMove(-1, -1, -MAXSCORE - 1);

	// 合法手をすべて試す
	for (Move& move : legalMoves)
	{
		// 手を打つ
		board.move(move);

		// 自分目線の評価値を計算する
		move.value = -board.evaluate();

		// 着手を戻す
		board.undo(move);

		if (move.value > bestMove.value)
		{
			// 最善手を更新する
			bestMove = move;
		}
	}

	return bestMove;
}

// Negamax法で探索する
Move AINegamax(Board board, std::vector<Move> legalMoves, int depth, bool passed, int& nNodes)
{
	++nNodes;
	Move bestMove(-1, -1, -MAXSCORE - 1);

	if (depth == 0)
	{
		// 深さが0なら評価関数を作用させる
		bestMove.value = board.evaluate();
		return bestMove;
	}

	if (legalMoves.empty())
	{
		// 合法手がない場合
		if (passed)
		{
			// 双方打てるところがないので終局
			bestMove.value = board.score();
		}
		else
		{
			// パス
			board.pass();
			bestMove.value = -AINegamax(board, board.getLegalMoves(), depth, true, nNodes).value;
		}
		return bestMove;
	}

	for (Move& move : legalMoves)
	{
		// 手を進める
		board.move(move);
		{
			// 深さを1減らして再帰的に探索を行う
			// 相手目線の値が出てくるのでマイナスをつける
			move.value = -AINegamax(board, board.getLegalMoves(), depth - 1, false, nNodes).value;
		}
		// 着手を戻す
		board.undo(move);

		if (move.value > bestMove.value)
		{
			// 最善手を更新
			bestMove = move;
		}
	}
	return bestMove;
}

// αβ法で探索する (Negamax + αβ法による枝刈り)
Move AIAlphaBeta(Board board, std::vector<Move> legalMoves, int depth, bool passed, int& nNodes, int alpha, int beta)
{
	++nNodes;
	Move bestMove(-1, -1, -MAXSCORE - 1);

	if (depth == 0)
	{
		// 深さが0なら評価関数を作用させる
		bestMove.value = board.evaluate();
		return bestMove;
	}

	if (legalMoves.empty())
	{
		// 合法手がない場合
		if (passed)
		{
			// 双方打てるところがないので終局
			bestMove.value = board.score();
		}
		else
		{
			// パス
			board.pass();
			// 手番を変えるときには[-beta, alpha]の範囲で探索する
			bestMove.value = -AIAlphaBeta(board, board.getLegalMoves(), depth, true, nNodes, -beta, -alpha).value;
		}
		return bestMove;
	}

	for (Move& move : legalMoves)
	{
		// 手を進める
		board.move(move);
		{
			// 深さを1減らして再帰的に探索を行う
			// 相手目線の値が出てくるのでマイナスをつける
			move.value = -AIAlphaBeta(board, board.getLegalMoves(), depth - 1, false, nNodes, -beta, -alpha).value;
		}
		// 着手を戻す
		board.undo(move);

		if (move.value > bestMove.value)
		{
			// 最善手を更新
			bestMove = move;
		}

		// ここを追加するだけで枝刈りが実装できる
		// 探索窓(α=評価値の興味範囲の下限)を更新
		alpha = std::max(alpha, move.value);
		if (alpha >= beta)
		{
			// 探索窓の幅が0以下になったらそれ以上探索しない
			break;
		}
	}
	return bestMove;
}

// 合法手を並べ替える
void MoveOrdering(Board board, std::vector<Move>& legalMoves)
{
	for (Move& move : legalMoves)
	{
		// 手を進める
		board.move(move);
		{
			// 1手進んだときの評価値の良し悪しを使う
			// 相手目線の値が出てくるのでマイナスをつける
			move.value = -board.evaluate();
		}
		// 着手を戻す
		board.undo(move);
	}

	// 値が大きい順＝良さそうな手の順番にソートする
	std::sort(legalMoves.begin(), legalMoves.end(),
		[](const Move& a, const Move& b) { return a.value > b.value; });
}

// αβ法で探索する (Move Orderingを使う)
Move AIAlphaBetaOrdering(Board board, std::vector<Move> legalMoves, int depth, bool passed, int& nNodes, int alpha, int beta)
{
	++nNodes;
	Move bestMove(-1, -1, -MAXSCORE - 1);

	if (depth == 0)
	{
		// 深さが0なら評価関数を作用させる
		bestMove.value = board.evaluate();
		return bestMove;
	}

	if (legalMoves.empty())
	{
		// 合法手がない場合
		if (passed)
		{
			// 双方打てるところがないので終局
			bestMove.value = board.score();
		}
		else
		{
			// パス
			board.pass();
			// 手番を変えるときには[-beta, alpha]の範囲で探索する
			bestMove.value = -AIAlphaBetaOrdering(board, board.getLegalMoves(), depth, true, nNodes, -beta, -alpha).value;
		}
		return bestMove;
	}

	// 手の並べ替え: 良さそうな手から探索して枝刈り効率を上げる
	MoveOrdering(board, legalMoves);

	for (Move& move : legalMoves)
	{
		// 手を進める
		board.move(move);
		{
			// 深さを1減らして再帰的に探索を行う
			// 相手目線の値が出てくるのでマイナスをつける
			move.value = -AIAlphaBetaOrdering(board, board.getLegalMoves(), depth - 1, false, nNodes, -beta, -alpha).value;
		}
		// 着手を戻す
		board.undo(move);

		if (move.value > bestMove.value)
		{
			// 最善手を更新
			bestMove = move;
		}

		// ここを追加するだけで枝刈りが実装できる
		// 探索窓(α=評価値の興味範囲の下限)を更新
		alpha = std::max(alpha, move.value);
		if (alpha >= beta)
		{
			// 探索窓の幅が0以下になったらそれ以上探索しない
			break;
		}
	}
	return bestMove;
}

int main()
{
	// AIは白
	const int aiPlayer = WHITE;

	// 初期盤面を作成
	Board board;
	bool passed = false;

	for (;;)
	{
		const std::vector<Move> legalMoves = board.getLegalMoves();

		if (legalMoves.empty())
		{
			if (passed)
			{
				// 両者が連続でパスしたので終局
				break;
			}
			// パス
			board.pass();
			std::cout << "Pass!\n";
			passed = true;
			continue;
		}

		passed = false;

		board.print();

		if (board.getPlayer() == aiPlayer)
		{
			// AIの手番
			int nNodes = 0; // 訪問ノード数
			//Move move = AIRandom(board, legalMoves);
			//Move move = AI1Move(board, legalMoves); // 1手読み AI を使う
			//Move move = AINegamax(board, legalMoves, 5, false, nNodes); // Negamax法 (5手読み)
			//Move move = AIAlphaBeta(board, legalMoves, 5, false, nNodes, -MAXSCORE, MAXSCORE); // αβ法 (5手読み)
			Move move = AIAlphaBetaOrdering(board, legalMoves, 8, false, nNodes, -MAXSCORE, MAXSCORE); // Move Orderingつきαβ法 (8手読み)

			// AIの手や評価値、訪問ノード数を表示する
			std::cout << "AI moved: " << move.toString() << " value: " << move.value << " nNodes: " << nNodes << '\n';
			board.move(move);
		}
		else
		{
			// 人間の手番
			Move move = InputMove(legalMoves);
			board.move(move);
		}
	}

	std::cout << "Game Over!\n";
	board.print();
}
