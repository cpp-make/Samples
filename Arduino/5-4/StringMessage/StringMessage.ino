void setup() {
	Serial.begin(9600);
	while (!Serial); 
	
	int value = 100;
	String message = String("Hello, ") + value + " Arduino fans!";
	
	Serial.println(message);  // 直接シリアル出力可能
}

void loop() {
}
