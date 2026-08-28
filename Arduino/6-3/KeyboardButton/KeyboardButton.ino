#include "Keyboard.h"

constexpr int buttonPin = 12;
// 前回のボタンの状態を記憶する変数
int lastButtonState = HIGH;

void setup() {
Serial.begin(9600);
pinMode(buttonPin, INPUT_PULLUP);
// キーボード機能の初期化
Keyboard.begin();
}

void loop() {
	int currentButtonState = digitalRead(buttonPin);

	// ボタンが「押されて、離された」瞬間を検知
	if (lastButtonState == LOW && currentButtonState == HIGH) {
		Keyboard.println("Hello, Arduino!");
		Serial.println("Sent: Hello, Arduino!");
	}

	// 現在の状態を保存
	lastButtonState = currentButtonState;
	delay(10); // 簡単なチャタリング対策
}
