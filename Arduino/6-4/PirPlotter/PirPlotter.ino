constexpr int pirPin = A0; 

void setup() {
	Serial.begin(9600); 
}

void loop() {
	// A0 ピンの電圧を既定の 10 ビット値 (0〜1023) として読み取る
	int pirValue = analogRead(pirPin); 
	Serial.println(pirValue); 
	delay(100); 
}
