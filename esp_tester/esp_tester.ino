#define LED_PIN 38

//Blink LED oneper 500ms 

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("LED Blink Test Started");
}

void loop() {
  Serial.println("LED ON");
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  Serial.println("LED OFF");
  digitalWrite(LED_PIN, LOW);
  delay(500);
}
