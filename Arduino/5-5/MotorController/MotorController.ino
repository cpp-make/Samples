#ifdef abs
#undef abs
#endif

#include <unordered_map>
#include <string>
#include <algorithm>
#include <Servo.h>

// モーターを名前で管理するクラス
class MotorController {
private:
	std::unordered_map<std::string, Servo> servos;

public:
	// モーターを追加
	bool addMotor(const std::string& name, int pin) {
		Servo servo;
		if (servo.attach(pin) == INVALID_SERVO) {
			return false;
		}
		auto result = servos.emplace(name, std::move(servo));
		return result.second;
	}

	// モーターの角度を設定
	bool setAngle(const std::string& name, int angle) noexcept {
		const auto it = servos.find(name);
		if (it == servos.end()) {
			return false;
		}
		
		// 角度を0-180度の範囲に制限
		angle = std::clamp(angle, 0, 180);
		it->second.write(angle);
		return true;
	}
};

MotorController motors;

void setup() {
	Serial.begin(9600);
	while (!Serial);

	// 名前を付けてモーターを初期化・登録
	motors.addMotor("right_hand", 9);
	motors.addMotor("left_hand", 10);
	motors.addMotor("head", 11);
}

void loop() {
	// シリアルポートに "名前,角度" の形式でデータが送られてきたら処理
	if (Serial.available() > 0) {
		String arduinoStr = Serial.readStringUntil('\n');
		const auto commaIndex = arduinoStr.indexOf(',');
		
		if (commaIndex > 0) {
			String nameStr = arduinoStr.substring(0, commaIndex);
			const auto angle = arduinoStr.substring(commaIndex + 1).toInt();
			
			// C++ の std::string に変換して処理
			std::string name = nameStr.c_str();
			
			if (motors.setAngle(name, angle)) {
				Serial.print("Set ");
				Serial.print(name.c_str());
				Serial.print(" to ");
				Serial.println(angle);
			}
		}
	}
	delay(100);
}
