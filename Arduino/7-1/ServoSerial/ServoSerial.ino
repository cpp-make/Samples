#include <Servo.h>

constexpr int servoPin = 9;

Servo myServo;

void setup() {
	myServo.attach(servoPin);
	Serial.begin(9600); 
	while (!Serial); 
}

void loop() {
	if (Serial.available() > 0) {
		String input = Serial.readStringUntil('\n'); // 改行文字まで読み取り
		int angle = input.toInt(); // 文字列を整数に変換
		if (angle >= 0 && angle <= 180) {
			myServo.write(angle); // サーボを指定された角度に動かす
			Serial.print("Moving to angle: ");
			Serial.println(angle);
		}
	}
}															
