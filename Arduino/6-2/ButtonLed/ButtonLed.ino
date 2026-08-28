// ボタンを接続するピン番号を定義
constexpr int buttonPin = 12;

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	// buttonPin を入力モードに設定し、内部プルアップ抵抗を有効化
	pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
	int buttonState = digitalRead(buttonPin);

	if (buttonState == LOW) {
		// ボタンが押されている時 (LOW) に LED を点灯
		digitalWrite(LED_BUILTIN, HIGH);
	} else {
		// ボタンが押されていない時 (HIGH) に LED を消灯
		digitalWrite(LED_BUILTIN, LOW);
	}
}
