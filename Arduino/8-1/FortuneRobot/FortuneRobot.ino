#ifdef abs
#undef abs
#endif

#include <random>
#include <Servo.h>
#include <array>

// センサーとLEDの設定
constexpr int PIR_PIN = A0;
constexpr int PIR_THRESHOLD = 300;
constexpr int LED_PIN = LED_BUILTIN;

// サーボの設定
constexpr int SERVO_PIN = 9;
Servo fortuneServo;
constexpr int DEFAULT_POSITION = 90; // デフォルトの位置

// 占いの内容と対応する角度
constexpr std::array<const char*, 5> fortunes = {
		"大吉!", "吉!", "中吉", "小吉", "凶..."
};
constexpr std::array<int, 5> angles = {10, 46, 82, 118, 154}; // 占い結果に対応するサーボの角度

// C++ の乱数生成器
std::mt19937 rng;
std::uniform_int_distribution<int> fortune_dist(0, fortunes.size() - 1);

bool isHandDetected = false;

void setup() {
	pinMode(PIR_PIN, INPUT);
	pinMode(LED_PIN, OUTPUT);
	Serial.begin(9600);
	rng.seed(random()); // ハードウェア乱数をシードとして設定
	fortuneServo.attach(SERVO_PIN);
	fortuneServo.write(DEFAULT_POSITION); // 初期位置にサーボを設定
	Serial.println("おみくじロボット、起動しました。");
}

void loop() {
	int pirValue = analogRead(PIR_PIN);
	bool isCurrentlyDetected = pirValue > PIR_THRESHOLD;

	// 人を検知した瞬間
	if (isCurrentlyDetected && !isHandDetected) {
		isHandDetected = true;
		digitalWrite(LED_PIN, HIGH); // LEDを点灯
		rollFortune();
	} 
	// 人がいなくなった瞬間
	else if (!isCurrentlyDetected && isHandDetected) {
		isHandDetected = false;
		digitalWrite(LED_PIN, LOW);  // LEDを消灯
		fortuneServo.write(DEFAULT_POSITION); // サーボを初期位置に戻す
		Serial.println("\n次の人を待っています...");
	}
}

void rollFortune() {
	Serial.println("ようこそ！あなたの運勢を占います...");
	delay(1500); // 演出のための待ち時間

	int category = fortune_dist(rng); // 0から4の乱数を生成
	int angle = angles[category];     // 乱数に対応する角度を取得
	
	fortuneServo.write(angle);
	
	Serial.print("今日の運勢は…… ");
	Serial.println(fortunes[category]);
	Serial.print("（サーボ角度: ");
	Serial.print(angle);
	Serial.println("度）");
}
