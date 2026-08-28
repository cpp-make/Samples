constexpr int ShortDelay = 200;
constexpr int LongDelay = 500;

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
}

void blink(int count) {
	for (int i = 0; i < count; ++i) {
		digitalWrite(LED_BUILTIN, HIGH);
		delay(ShortDelay);
		digitalWrite(LED_BUILTIN, LOW);
		delay(ShortDelay);
	}
	delay(LongDelay);
}

void loop() {
	blink(3);
	blink(3);
	blink(7);
}
