#include <Siv3D.hpp>

// スロットゲームの絵柄
struct Symbol
{
	// 絵柄
	Texture symbol;

	// 賞金
	int32 score;
};

void Main()
{
	// 背景色を設定する
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	// フォント
	const Font font{ FontMethod::MSDF, 48,
		U"example/font/RocknRoll/RocknRollOne-Regular.ttf" };

	// ゲーム開始の効果音
	const Audio soundStart{ GMInstrument::Agogo, PianoKey::A3, 0.3s };

	// リール停止の効果音
	const Audio soundStop{ GMInstrument::SteelDrums, PianoKey::A3, 0.3s };

	// 賞金獲得の効果音（ループ再生）
	const Audio soundGet{ GMInstrument::TinkleBell, PianoKey::A6, 0.1s, 0.0s };
	soundGet.setLoop(true);

	// 絵柄のリスト
	const Array<Symbol> symbols
	{
		{ Texture{ U"💎"_emoji }, 1000 },
		{ Texture{ U"7️⃣"_emoji }, 777 },
		{ Texture{ U"💰"_emoji }, 300 },
		{ Texture{ U"🃏"_emoji }, 100 },
		{ Texture{ U"🍇"_emoji }, 30 },
		{ Texture{ U"🍒"_emoji }, 10 },
	};

	// 1 つのリールに用意される絵柄の基本リスト
	const Array<int32> symbolListBase =
		{ 0, 1, 2, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5 };

	// 3 つのリールに用意される絵柄のリスト（基本リストをシャッフル）
	const std::array<Array<int32>, 3> symbolLists =
	{
		symbolListBase.shuffled(),
		symbolListBase.shuffled(),
		symbolListBase.shuffled()
	};

	// 3 つのリールの描画位置
	const std::array<Rect, 3> reels
	{
		Rect{ 80, 100, 130, 300 },
		Rect{ 230, 100, 130, 300 },
		Rect{ 380, 100, 130, 300 },
	};

	// 所持金の描画位置
	const RoundRect moneyRect{ 560, 440, 190, 60, 20 };

	// 3 つのリールの回転量
	std::array<double, 3> rolls = { 0.0, 0.0, 0.0 };

	// 現在のゲームにおけるリール停止カウント（3 回で結果判定）
	int32 stopCount = 3;

	// 所持金
	int32 money = 1000;

	while (System::Update())
	{
		// スペースキーが押されたら
		if (KeySpace.down())
		{
			// 3 つのリールが停止している場合
			if (stopCount == 3)
			{
				// 所持金が 3 以上ある場合
				if (3 <= money)
				{
					// 所持金を 3 減らす
					money -= 3;

					// リール停止回数を 0 に戻す
					stopCount = 0;

					// ゲーム開始の効果音を再生する
					soundStart.playOneShot();
				}
			}
			else
			{
				// リールを整数位置で停止させる
				rolls[stopCount] = Math::Ceil(rolls[stopCount]);

				// リール停止カウントを増やす
				++stopCount;

				// リール停止の効果音を再生する
				soundStop.playOneShot();

				// 3 つのリールが停止した場合
				if (stopCount == 3)
				{
					// 各リールの絵柄
					const int32 r0 = symbolLists[0][(
						static_cast<int32>(rolls[0] + 1) % symbolLists[0].size())];
					const int32 r1 = symbolLists[1][(
						static_cast<int32>(rolls[1] + 1) % symbolLists[1].size())];
					const int32 r2 = symbolLists[2][(
						static_cast<int32>(rolls[2] + 1) % symbolLists[2].size())];

					// 3 つのリールの絵柄がすべて同じ場合
					if ((r0 == r1) && (r1 == r2))
					{
						// 所持金に賞金を加算する
						money += symbols[r0].score;

						// 賞金獲得の効果音を再生する
						soundGet.play();

						// 賞金獲得の効果音を 1.5 秒後に停止する
						soundGet.stop(1.5s);
					}
				}
			}
		}

		// リールの回転
		for (int32 i = 0; i < 3; ++i)
		{
			// 停止済みのリールはスキップ
			if (i < stopCount)
			{
				continue;
			}

			// 前フレームからの経過時間に応じてリールの回転量を増やす
			rolls[i] += (Scene::DeltaTime() * 12);
		}

		// リールの描画
		for (int32 k = 0; k < 3; ++k)
		{
			// リールの背景
			reels[k].draw();

			// リールの絵柄を描画
			for (int32 i = 0; i < 4; ++i)
			{
				// リールの何番目の要素を指すか（回転量の整数部分）
				const int32 index = (static_cast<int32>(rolls[k] + i)
					% symbolLists[k].size());

				// 絵柄のインデックス
				const int32 symbolIndex = symbolLists[k][index];

				// 絵柄の位置補正（回転量の小数部分）
				const double t = Math::Fraction(rolls[k]);

				// 絵柄の描画
				symbols[symbolIndex].symbol.resized(90)
					.drawAt(reels[k].center().movedBy(0, 140 * (1 - i + t)));
			}
		}

		// リールの上下に背景色を描くことで、はみ出した絵柄を隠す
		Rect{ 80, 0, 430, 100 }.draw(Scene::GetBackground());
		Rect{ 80, 400, 430, 200 }.draw(Scene::GetBackground());

		// リールの影と枠線の描画
		for (const auto& reel : reels)
		{
			// 上の影
			Rect{ reel.tl(), reel.w, 40 }.draw(Arg::top(0.0, 0.3), Arg::bottom(0.0, 0.0));

			// 下の影
			Rect{ (reel.bl() - Point{ 0, 40 }), reel.w, 40 }.draw(Arg::top(0.0, 0.0), Arg::bottom(0.0, 0.3));

			// 枠線
			reel.drawFrame(4, ColorF{ 0.5 });
		}

		// 中央を指す 2 つの三角形の描画
		Triangle{ 60, 250, 36, 90_deg }.draw(ColorF{ 1.0, 0.2, 0.2 });
		Triangle{ 530, 250, 36, -90_deg }.draw(ColorF{ 1.0, 0.2, 0.2 });

		// 絵柄リストを描く
		RoundRect{ 560, 100, 190, 300, 20 }.draw(ColorF{ 0.9, 0.95, 1.0 });

		for (size_t i = 0; i < symbols.size(); ++i)
		{
			// 絵柄を描く
			symbols[i].symbol.resized(32).draw(Vec2{ 586, (114 + i * 48) });

			// 賞金を描く
			font(symbols[i].score).draw(TextStyle::OutlineShadow(0.2, ColorF{ 0.5, 0.3, 0.2 },
				Vec2{ 1.5, 1.5 }, ColorF{ 0.5, 0.3, 0.2 }),
				25, Arg::topRight(720, (109 + i * 48)), ColorF{ 1.0, 0.9, 0.1 });

			if (i != 0)
			{
				// 絵柄の間に区切り線を描く
				Rect{ 570, (105 + i * 48), 170, 1 }.draw(ColorF{ 0.7 });
			}
		}

		// 所持金の背景の描画
		if (soundGet.isPlaying())
		{
			// 賞金獲得中は点滅させる
			const ColorF color = Periodic::Sine0_1(0.3s) * ColorF{ 0.5, 0.6, 0.7 };
			moneyRect.draw(color).drawFrame(1);
		}
		else
		{
			moneyRect.draw(ColorF{ 0.1, 0.2, 0.3 }).drawFrame(1);
		}

		// 所持金の描画
		font(money).draw(30, Arg::rightCenter(moneyRect.rightCenter().movedBy(-30, 0)));
	}
}
