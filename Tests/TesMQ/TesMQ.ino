void setup() {
  // put your setup code here, to run once:
  pinMode(A0, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Analog Readings : ");
  Serial.println(analogRead(A0));
  delay(500);
}
