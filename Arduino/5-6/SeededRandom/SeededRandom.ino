void setup() {
	Serial.begin(9600);
	while (!Serial);
	// シード値を固定することで、常に同じ乱数列を生成
	randomSeed(12345);
}

void loop() {
	// 12345をシードとした疑似乱数を生成
	int result = random(100);
	Serial.println(result);
	delay(1000);
}
