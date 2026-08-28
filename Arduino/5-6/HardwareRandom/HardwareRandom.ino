void setup() {
	Serial.begin(9600);
	while (!Serial);
}

void loop() {
	// 0から99までの範囲の乱数を生成
	int n = random(100);
	Serial.println(n);
	delay(1000);
}
