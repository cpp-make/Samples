#include <Siv3D.hpp>

void Main()
{
	// 1 つのブロックのサイズ
	constexpr Size BrickSize{ 40, 20 };

	// ボールの速さ（ピクセル / 秒）
	constexpr double BallSpeed = 480.0;

	// ボールの速度
	Vec2 ballVelocity{ 0, -BallSpeed };

	// ボール
	Circle ball{ 400, 400, 8 };

	// ブロックの配列
	Array<Rect> bricks;

	for (int32 y = 0; y < 5; ++y)
	{
		for (int32 x = 0; x < (800 / BrickSize.x); ++x)
		{
			bricks << Rect{ (x * BrickSize.x), (60 + y * BrickSize.y), BrickSize };
		}
	}

	// ブロックの破壊音
	const Audio sound{ GMInstrument::Woodblock, PianoKey::A4, 0.3s };

	while (System::Update())
	{
		// ボールを移動させる
		ball.moveBy(ballVelocity * Scene::DeltaTime());

		// ブロックを順にチェックする
		for (auto it = bricks.begin(); it != bricks.end(); ++it)
		{
			// ブロックとボールが交差していたら
			if (it->intersects(ball))
			{
				// ブロックの上辺、または底辺と交差していたら
				if (it->bottom().intersects(ball) || it->top().intersects(ball))
				{
					// ボールの速度の Y 成分の符号を反転する
					ballVelocity.y *= -1;
				}
				else // ブロックの左辺または右辺と交差していたら
				{
					// ボールの速度の X 成分の符号を反転する
					ballVelocity.x *= -1;
				}

				// ブロックを配列から削除する（イテレータは無効になる）
				bricks.erase(it);

				// ブロックの破壊音を再生する
				sound.playOneShot(0.5);

				// これ以上チェックしない
				break;
			}
		}

		// 天井にぶつかったら
		if ((ball.y < 0) && (ballVelocity.y < 0))
		{
			// ボールの速度の Y 成分の符号を反転する
			ballVelocity.y *= -1;
		}

		// 左右の壁にぶつかったら
		if (((ball.x < 0) && (ballVelocity.x < 0))
			|| ((800 < ball.x) && (0 < ballVelocity.x)))
		{
			// ボールの速度の X 成分の符号を反転する
			ballVelocity.x *= -1;
		}

		// パドル
		const Rect paddle{ Arg::center(Cursor::Pos().x, 500), 60, 10 };

		// パドルにあたったら
		if ((0 < ballVelocity.y) && paddle.intersects(ball))
		{
			// パドルの中心からの距離に応じてはね返る方向（速度ベクトル）を変える
			ballVelocity = Vec2{ (ball.x - paddle.center().x) * 10, -ballVelocity.y }.withLength(BallSpeed);
		}

		// すべてのブロックを描画する
		for (const auto& brick : bricks)
		{
			// ブロックの Y 座標に応じて色を変える
			brick.stretched(-1).draw(HSV{ brick.y - 40 });
		}

		// ボールを描く
		ball.draw();

		// パドルを描く
		paddle.rounded(3).draw();

		// マウスカーソルを非表示にする
		Cursor::RequestStyle(CursorStyle::Hidden);
	}
}
