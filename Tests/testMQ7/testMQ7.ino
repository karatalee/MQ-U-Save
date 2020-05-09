#define MQ_7_INPUT 13 //Pin Digital D13

/**
 * Pinout Sensor
 * -> Vcc = 5V Nano
 * -> GND = GND Nano
 * -> A0 not connected
 * -> D0 = pin MQ_INPUT
 */

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(MQ_7_INPUT, INPUT);
}

void loop() {
  // Hasil Read Digital 1 artinya "Tidak terdeteksi" dan lampu LED akan redup (tapi tetap menyala)
  // Hasil Read Digital 0 artinya "Terdeteksi" dan lampu LED akan terang
  Serial.print("Digital Read Value : ");
  Serial.println(digitalRead(MQ_7_INPUT));
  delay(500);
}
