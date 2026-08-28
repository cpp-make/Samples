void setup() {
	Serial.begin(9600);
	while (!Serial);
	Serial.println(abs(-5));  // 5
}

void loop() {
}
