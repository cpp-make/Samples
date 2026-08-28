#include <Servo.h>
#include <Arduino.h>

// ピン設定
constexpr int LED_PIN = 13;
constexpr int BUTTON_PIN = 3;
constexpr int SERVO_PIN = 9;

// サーボモーターの設定
constexpr int SERVO_MIN_PULSE = 700;
constexpr int SERVO_MAX_PULSE = 2300;
constexpr int SERVO_START_ANGLE = 10;
constexpr int SERVO_WAVE_ANGLE_MIN = 80;
constexpr int SERVO_WAVE_ANGLE_MAX = 100;

// ボタン設定
constexpr int BUTTON_ON = LOW;
constexpr unsigned long DEBOUNCE_TIME = 10; // ms

// タイミング設定
constexpr unsigned long COUNTDOWN_DURATION = 10000; // ms
constexpr unsigned long FAST_BLINK_THRESHOLD = 3000; // ms
constexpr unsigned long FAST_BLINK_RATE = 100; // ms
constexpr unsigned long SLOW_BLINK_RATE = 500; // ms
constexpr unsigned long WAVE_DELAY = 100; // ms
constexpr int WAVE_COUNT = 10;

// システムの状態を定義
enum class SystemState {
	IDLE,           // 待機状態
	COUNTING_DOWN,  // カウントダウン中
	WAVING_FLAG     // 旗を振っている
};

// システムの状態と関連データを管理する構造体
struct SystemStatus {
	SystemState currentState = SystemState::IDLE;
	unsigned long startTime = 0;
	int lastRemainingSeconds = -1;
};

SystemStatus status;
Servo servo;

void setup() {
	Serial.begin(9600);
	// while (!Serial); 
	pinMode(LED_PIN, OUTPUT);
	pinMode(BUTTON_PIN, INPUT_PULLUP);
	servo.attach(SERVO_PIN, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
	Serial.println("初期化完了。ボタンを押してタイマーを開始します。");
}

void loop() {
	// 現在の状態に応じて、対応する処理関数を呼び出す
	switch (status.currentState) {
		case SystemState::IDLE:
			handleIdleState();
			break;
		case SystemState::COUNTING_DOWN:
			handleCountingDownState();
			break;
		case SystemState::WAVING_FLAG:
			handleWavingFlagState();
			break;
	}
}

// 待機状態の処理
void handleIdleState() {
	static unsigned long lastDebounceTime = 0;
	
	if (digitalRead(BUTTON_PIN) == BUTTON_ON && (millis() - lastDebounceTime > DEBOUNCE_TIME)) {
		lastDebounceTime = millis();
		startCountdown();
	}
}

// カウントダウン中の処理
void handleCountingDownState() {
	unsigned long remainingTime = calculateRemainingTime();
	
	if (remainingTime > 0) {
		updateLedBlinking(remainingTime);
		updateRemainingTimeDisplay(remainingTime);
	} else {
		startWavingFlag(); // 時間が来たら旗振り状態へ移行
	}
}

// 旗を振っている状態の処理
void handleWavingFlagState() {
	waveFlag();
	resetToIdle(); // 終わったら待機状態へ戻る
}

// --- ユーティリティ関数群 ---

// カウントダウンを開始する
void startCountdown() {
	status.currentState = SystemState::COUNTING_DOWN;
	status.startTime = millis();
	status.lastRemainingSeconds = -1; // 残り時間表示をリセット
	servo.write(SERVO_START_ANGLE);
	Serial.println("カウントダウン開始");
}

// 残り時間を計算する
unsigned long calculateRemainingTime() {
	unsigned long elapsedTime = millis() - status.startTime;
	return COUNTDOWN_DURATION > elapsedTime ? COUNTDOWN_DURATION - elapsedTime : 0;
}

// 残り時間に応じてLEDの点滅速度を変える
void updateLedBlinking(unsigned long remainingTime) {
	unsigned long blinkRate = (remainingTime <= FAST_BLINK_THRESHOLD) ? 
							FAST_BLINK_RATE : SLOW_BLINK_RATE;
	digitalWrite(LED_PIN, (millis() / blinkRate) % 2);
}

// シリアルモニタに残り時間を表示する
void updateRemainingTimeDisplay(unsigned long remainingTime) {
	int remainingSeconds = remainingTime / 1000;
	if (remainingSeconds != status.lastRemainingSeconds) {
		Serial.print("残り時間: ");
		Serial.print(remainingSeconds);
		Serial.println("秒");
		status.lastRemainingSeconds = remainingSeconds;
	}
}

// 旗振り状態を開始する
void startWavingFlag() {
	status.currentState = SystemState::WAVING_FLAG;
	Serial.println("時間です！旗を振ります。");
}

// 旗を振る動作
void waveFlag() {
	for (int i = 0; i < WAVE_COUNT; ++i) {
		servo.write(SERVO_WAVE_ANGLE_MIN);
		delay(WAVE_DELAY);
		servo.write(SERVO_WAVE_ANGLE_MAX);
		delay(WAVE_DELAY);
	}
	Serial.println("旗振り完了");
}

// 待機状態に戻す
void resetToIdle() {
	digitalWrite(LED_PIN, LOW);
	servo.write(SERVO_START_ANGLE);
	status.currentState = SystemState::IDLE;
	Serial.println("\n待機状態に戻ります。");
}
