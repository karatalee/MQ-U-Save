/*
 * Arduino Sketch file untuk Arduino Nano
 * 
 * Sketch ini berisikan kode-kode untuk
 * - Menerima input dari sensor MQ-135 & MQ-7
 * - Menjalankan output sesuai perintah-perintah
 * - Penjelasan cara kerja sistem dapat melihat readme.txt
 *
 * Bagian-bagian yang terdapat pada sketch ini:
 *
 * - Directives Section
 * - Pinout
 *   $ Pinout Sensor
 *   $ Pinout LCD
 * - Variabel
 * - Timer
 * - Pembacaan Sensor
 * - LCD
 * - Buzzer
 * - Button
 * - Battery Level
 * - Code Section (setup, loop)
 */

/* ========================== DIRECTIVES SECTION ========================== */

#define MQ_7_INPUT 13           // Inisialisasi Pin Digital D13
#define MQ_135_INPUT 12         // Inisialisasi Pin Digital D12
#define button 3                // Inisialisasi Pin Digital D3
#define buzzer 2                // Inisialisasi Pin Digital D2
 
#include <LiquidCrystal_I2C.h>  // Library untuk menggunakan LCD, version 1.2.1
#include <Wire.h>               // Library ini dibutuhkan oleh library LCD

 /* =============================== Pinout =============================== */

    /**
     * Pinout Sensor
     * -> Vcc = 5V Nano
     * -> GND = GND Nano
     * -> A0 not connected
     * -> D0 = pin MQ_INPUT
    
     * Pinout LCD
     * -> Vcc = 5V Nano
     * -> GND = GND Nano
     * -> SDA = A4
     * -> SCL = A5
     */

 /* =============================== Variabel =============================== */

    // 0xF9 = 249 untuk mengukur 1 ms
    // 0x3D08 = 15624 untuk mengukur 1 detik
    // dst dst
    #define REGISTER_LIMIT 15624 // 1 detik
    
    unsigned int pengali = 0;

    // Inisialisasi Library untuk Menentukan Pin Arduino yang akan di pakai
    LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

    // Battery Indicator
    // 8 yang pertama = berapa tingkatan
    // 8 yang kedua = ukuran byte2 nya
    byte battery_level[8][8] = {
      { B01110, B10001, B10001, B10001, B10001, B10001, B10001, B11111,}, // 12.5% - 0%
      { B01110, B10001, B10001, B10001, B10001, B10001, B11111, B11111,}, // 25% - 12.5%
      { B01110, B10001, B10001, B10001, B10001, B11111, B11111, B11111,}, // 37.5% - 25%
      { B01110, B10001, B10001, B10001, B11111, B11111, B11111, B11111,}, // 50% - 37.5%
      { B01110, B10001, B10001, B11111, B11111, B11111, B11111, B11111,}, // 62.5% - 50%
      { B01110, B10001, B11111, B11111, B11111, B11111, B11111, B11111,}, // 75% - 62.5%
      { B01110, B11111, B11111, B11111, B11111, B11111, B11111, B11111,}, // 87.5% - 75%
      { B01110, B11111, B10101, B10001, B11011, B11011, B11111, B11111,}  // 100% - 87.5%
    };


 /* =============================== Timer =============================== */

   /**
   * Timer1 : 16 bit
   * 
   * Rumus :
   * (Waktu delay dalam detik) * ((16 * 10^6)/prescaler) - 1 = Nilai Register OCRxA
   * 
   * Nilai Maksimal dari Register OCR0A adalah 65535 (16 bit)
   * 
   * Supaya  delay cukup lama, ditambahkan faktor pengali
   */
   
    void initTimer(){
      // Sumber : https://www.instructables.com/id/Arduino-Timer-Interrupts/
      
      cli();      //Matikan segala jenis interrupt selagi setup timer
      TCCR1A = 0;     //Bersihkan dulu registernya
      TCCR1B = 0;
      TCNT1 = 0;    //Counter yang pake oleh timer
       
      OCR1A = REGISTER_LIMIT;   //Nilai dari register ini akan menjadi batas atas dari TCNTx
      
      TCCR1B = (1<<WGM12);    //Pake mode CTC (Clear Timer on Compare)
    
      //Prescaler = 1024 untuk Timer1
      TCCR1B|=(1<<CS12);
      TCCR1B|=(0<<CS11);    
      TCCR1B|=(1<<CS10);
      TIMSK1 |= (1<<OCIE1A);  //Set the interrupt request
      sei();      //Nyalakan pas udah selesai setup timer
    }
  
    void setTimerSeconds(int seconds){
      pengali = seconds;
    }
    
    // Function ini dipanggil saat nilai register dari Timer1 = OCR0A
    // Isi function harus sesimpel mungkin supaya tidak mengganggu flow progam
    ISR(TIMER1_COMPA_vect){    
      if(pengali > 0) 
        pengali--;
    }

 /* ========================= Pembacaan Sensor ========================= */
   bool readMQ7(){
    //Kalau menggunakan analog, tinggal ganti syarat
    return digitalRead(MQ_7_INPUT) == 0; 
    
    //int nilai_batas = 512;
    //return analogRead(MQ_7_INPUT) > nilai_batas
   }
  
   // Penggunaan MQ_135 sama seperti MQ_7
   bool readMQ135(){
    return digitalRead(MQ_135_INPUT) == 0;
  }
 
 /* =============================== LCD =============================== */

  // Inisialisasi lokasi cursor untuk output LCD
  void writeStringToLCD(int x, int y, String text){
      if( x < 2 && x >= 0 && y < 16 && y >= 0){
        lcd.setCursor(x, y);
      } else {
        lcd.setCursor(0, 0);
      }

      //Bisa check panjang text di sini
      lcd.print(text);
  }

  void printBatteryLevel(int x, int y, int idx){
      // Boleh tambahin pengecekan idx < 8 di sini    
      lcd.createChar(0, battery_level[idx]);
      lcd.setCursor(x, y);
      lcd.write(byte(0));
  }
  
 /* =============================== Buzzer =============================== */
   void buzzerOn(){
      digitalWrite(buzzer, HIGH);
   }
  
   void buzzerOff(){
      digitalWrite(buzzer, LOW);
   }

 /* =============================== Button =============================== */
    bool buttonCheck(){
        return digitalRead(button) == 1;
    }
    
 /* =============================== Battery Level =============================== */

    //read internal voltage
    long readVcc() {
      long result;
      // Read 1.1V reference against AVcc
      ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
      
      delay(2); // Wait for Vref to settle
      
      ADCSRA |= _BV(ADSC); // Convert
      while (bit_is_set(ADCSRA, ADSC));
      
      result = ADCL;
      result |= ADCH << 8;
      result = 1126400L / result; // Back-calculate AVcc in mV
      return result;
    }
    
    //KHUSUS UNTUK BATERAI 5V
    int checkBattery(){
        //read the voltage and convert it to volt
        double curvolt = double( readVcc() ) / 1000;
        int batt_lvl = 0;

        if(curvolt > 4.2){
          batt_lvl = 7;
        } else if (curvolt <= 4.2 && curvolt > 4.0){
          batt_lvl = 6;
        } else if (curvolt <= 4.0 && curvolt > 3.8){
          batt_lvl = 5;
        } else if (curvolt <= 3.8 && curvolt > 3.6){
          batt_lvl = 4;
        } else if (curvolt <= 3.6 && curvolt > 3.4){
          batt_lvl = 3;
        } else if (curvolt <= 3.4 && curvolt > 3.){
          batt_lvl = 2;
        } else if (curvolt <= 3.2 && curvolt > 3.0){
          batt_lvl = 1;
        } else if (curvolt < 3.0){
          batt_lvl = 0;
        }

        return batt_lvl;
    }

 /* ============================ Code Section ============================ */
void setup() {
  Serial.begin(9600);
  initTimer();

  // Inputs
  pinMode(MQ_7_INPUT, INPUT);
  pinMode(MQ_135_INPUT, INPUT);
  pinMode(button, INPUT);

  // Outputs
  pinMode(buzzer, OUTPUT);
  lcd.begin(16,2);          // 16 characters, 2 baris
  lcd.backlight();
}


void loop() {
  printBatteryLevel(15, 0, checkBattery());

  // Pembacaan sensor MQ
  bool bahaya = readMQ7()|| readMQ135();
  String mq7_status = "MQ-7 : ";
  if(!bahaya){
    mq7_status += "LOW ";
  } else {
    mq7_status += "HIGH";
  }
  
  writeStringToLCD(0, 0, mq7_status);
  String tampilan;

  // Untuk output LCD
  if(!bahaya){              // ketika sensor tidak detect
    pengali = 0;
    tampilan = "AMAN  ";
  } else {                  // ketika sensor detect
    tampilan = "BAHAYA";
    
    if(buttonCheck()){
      setTimerSeconds(5);   // dalam satuan detik
    }
  }

  //Output Buzzer
  if(bahaya && pengali == 0){
    buzzerOn();             // menyalakan buzzer
  } else {
    buzzerOff();            // mematikan buzzer
  }

  writeStringToLCD(0, 1, tampilan);
}
