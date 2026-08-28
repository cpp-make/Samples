#ifdef abs
#undef abs
#endif

#include <random>

// --- 設定 ---
constexpr int pirPin = A0;
constexpr int pirThreshold = 300;
bool isHandDetected = false;

// C++ の乱数生成器
std::mt19937 rng;
std::uniform_int_distribution<int> dice_dist(1, 6);

// --- セットアップ ---
void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(9600);
	// while (!Serial); 
	
	// ハードウェア乱数生成器(TRNG)を乱数のシード（種）にする
	rng.seed(random());
}

// --- メインループ ---
void loop() {
	int pirValue = analogRead(pirPin);
	bool isCurrentlyDetected = pirValue > pirThreshold;

	// 手をかざした瞬間を検知
	if (isCurrentlyDetected && !isHandDetected) {
		isHandDetected = true;
		digitalWrite(LED_BUILTIN, HIGH); // 検知したことを LED で示す
		rollDice();
	} 
	// 手が離れた瞬間を検知
	else if (!isCurrentlyDetected && isHandDetected) {
		isHandDetected = false;
		digitalWrite(LED_BUILTIN, LOW);
		Serial.println("Ready to roll the dice...");
	}
}

// --- サイコロを振る関数 ---
void rollDice() {
	Serial.println("\nRolling the dice...");
	int dice1 = dice_dist(rng);
	int dice2 = dice_dist(rng);

	Serial.print("Dice 1: ");
	Serial.print(dice1);
	Serial.print(", Dice 2: ");
	Serial.print(dice2);
	Serial.print(", Total: ");
	Serial.println(dice1 + dice2);
}
