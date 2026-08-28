constexpr int pirPin = A0;
constexpr int threshold = 300; // 観察した値をもとに決めるしきい値

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(9600);
	// while (!Serial); 
}

void loop() {
	int pirValue = analogRead(pirPin); 
	Serial.println(pirValue); 

	if (pirValue >= threshold) {
		digitalWrite(LED_BUILTIN, HIGH); // LED を点灯
	} else {
		digitalWrite(LED_BUILTIN, LOW);  // LED を消灯
	}
delay(100); 
}
