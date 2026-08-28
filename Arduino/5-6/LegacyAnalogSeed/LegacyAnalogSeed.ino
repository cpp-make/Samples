void setup() {
	Serial.begin(9600);
	while (!Serial);
	// アナログピンA0からノイズを読み取り、シード値として設定
	randomSeed(analogRead(0));
}

void loop() {
	int result = random(100);
	Serial.println(result);
	delay(1000);
}
