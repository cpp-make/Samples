int valueIntA = 5;
int valueIntB = 2;
double valueDouble = 1.23;

void setup() {
	Serial.begin(9600);
	while (!Serial);

	Serial.println("--- printlnの例 ---");
	Serial.println(valueDouble); // double型も出力可能
	Serial.println(valueIntA * valueIntB); // 計算結果も出力可能

	Serial.println("\n--- printの例 ---");
	Serial.print("printは");
	Serial.print("改行なしで");
	Serial.println("連結できます。");

	Serial.println("\n--- printfの例 ---");
	Serial.printf("printfも使えます: %d * %d = %d \n", valueIntA, valueIntB, valueIntA * valueIntB); 
	}

void loop() {
}
