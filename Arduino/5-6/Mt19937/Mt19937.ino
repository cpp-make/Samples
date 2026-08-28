#ifdef abs
#undef abs
#endif

#include <random>

std::mt19937 rng; // メルセンヌ・ツイスタ乱数生成器

void setup() {
	Serial.begin(9600);
	while (!Serial);
	// ハードウェア乱数をシードとして設定
	rng.seed(random());
}

void loop() {
	// 0から99までの一様整数分布を定義
	std::uniform_int_distribution<int> dist{ 0, 99 };
	int result = dist(rng); // 乱数を生成
	Serial.println(result);
	delay(1000);
}
