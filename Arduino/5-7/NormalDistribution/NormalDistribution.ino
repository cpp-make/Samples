#ifdef abs
#undef abs
#endif

#include <random>

std::mt19937 rng;

void setup() {
	Serial.begin(9600);
	while (!Serial);
	// ハードウェア乱数をシードとして設定
	rng.seed(random());
}

void loop() {
	// 平均0.0、標準偏差1.0の正規分布を定義
	std::normal_distribution<double> dist{ 0.0, 1.0 };
	double result = dist(rng); // 乱数を生成
	Serial.println(result, 6); // 小数点以下6桁で出力
	delay(1000);
}
