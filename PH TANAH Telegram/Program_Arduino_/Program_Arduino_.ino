#include <Wire.h>                                                   //Memanggil library Wire.h
#include <LiquidCrystal_I2C.h>                                      //Memanggil library LiquidCrystal_I2C.h
LiquidCrystal_I2C lcd(0x27, 16, 2);                                 //Membuat objek lcd dan deklarasi alamat serta kolom dan baris lcd

#include <SoftwareSerial.h>                                         //Memanggil library SoftwareSerial.h
SoftwareSerial nano(2, 3); //RX,TX                                   //Membuat objek nano dan deklarasi pin komunikasi serial

const int sensorPH = A0 ;                                           //Dekalarasi pin sensor PH (sambungkan kabel hitam (output) ke pin A0)
const int sensorKelembaban = A1 ;                                   //Dekalarasi pin sensor Kelembaban (Ao (output) ke pin A1)
const int relay = 4 ;                                               //Dekalarasi pin relay (in relay ke pin D4)
int i;
int adcPH = 0;                                                      //Variable untuk menampung nilai ADC sensor PH
int adcKelembaban = 0 ;                                             //Variable untuk menampung nilai ADC sensor Kelembaban
float PH = 0.0, fix_PH, PH_1;                                                   //Variable untuk menampung nilai PH
String statusTanah ;                                                //Variable untuk menampung status tanah (basah, lembab atau kering)
String stringPH ;                                                   //Varaible untuk menampung nilai PH dalam bentuk string
unsigned long millisLalu ;                                          //Variable untuk menampung nilai millis ;

void setup() {
  Serial.begin(115200);                                             //Mengatur baudrate Komunikasi serial 115200
  nano.begin(9600);                                                 //Mengatur baudrate Komunikasi serial arduino & nodemcu 9600
  lcd.begin();                                                      //Memulai komunikasi dengan lcd i2c
  lcd.backlight();                                                  //Menyalakan backlight LCD
  pinMode(sensorKelembaban , INPUT);                                //Mengatur pin dengan nama senorKelembaban sebagai input
  pinMode(sensorPH , INPUT);                                        //Mengatur pin dengan nama senorPH sebagai input
  pinMode(relay , OUTPUT);                                          //Mengatur pin dengan nama relay sebagai output
  digitalWrite(relay , HIGH);                                       //Mamatikan relay (pompa air);
}

void loop() {
  tampilanLcd();                                                    //Memanggil fungsi tampilanLCD
  cek_sensor();                                                     //Rumus menghitung PH (dari datasheet sensor)


  if (adcKelembaban >= 700) {                                        //Jika nilai ADC sensor kelembaban lebih besar atau sama dengan 700, maka
    digitalWrite(relay , LOW);                                        //Relay ON (Pompa Air hidup)
    statusTanah = "Kering" ;                                          //nilai variable string statusTanah = kering
  }
  else if (adcKelembaban >= 400 && adcKelembaban < 700) {            //Jika nilai ADC sensor kelembaban lebih besar atau sama dengan 400 dan kurang dari 700, maka
    digitalWrite(relay , HIGH);                                       //Relay OFF (Pompa Air mati)
    statusTanah = "Lembab" ;                                        //nilai variable string statusTanah = Lembab
  }
  else if (adcKelembaban < 400) {                                   //Jika nilai ADC sensor kelembaban lebih kecil dari 400, maka
    digitalWrite(relay , HIGH);                                       //Relay OFF (Pompa Air mati)
    statusTanah = "Basah " ;                                        //nilai variable string statusTanah = Basah
  }
  if (millis() - millisLalu >= 200) {                                //Jika millis - millis sebelumnya lebih besar atau sama dengan 200 (interval 200ms), maka
    nano.println('*' + stringPH + ',' + statusTanah + '#');           //Arduino nano mengirim data PH dan status tanah ke nodeMCU
    millisLalu = millis();                                            //millisLalu = millis untuk melakukan pengulangan
  }
}

void tampilanLcd() {                                                //Fungsi tampilan LCD
  lcd.setCursor( 0 , 0);                                            //Mengatur cursor pada baris 1 kolom 1
  lcd.print("PH Tanah :");                                          //Menampilkan "PH Tanah :" pada baris 1 kolom 1 LCD
  lcd.setCursor( 10 , 0);                                           //Mengatur cursor pada baris 1 kolom 11
  rata_rata();
  stringPH = String(fix_PH, 2);                                           //mengubah nilai PH ke bentuk string
  lcd.print(stringPH);                                                    //Menampilkan nilai PH pada baris 2 kolom 11 LCD

  lcd.setCursor( 0 , 1);                                            //Mengatur cursor pada baris 2 kolom 1
  lcd.print("STATUS :");                                            //Menampilkan "STATUS :" pada baris baris 2 kolom 1 LCD
  lcd.setCursor( 8 , 1);                                            //Mengatur cursor pada baris 2 kolom 9
  lcd.print(statusTanah);                                           //Menampilkan nilai PH pada baris 2 kolom 9 LCD
  delay(100);
}


void rata_rata() {
  for ( i = 0; i < 30 ; i++) {
    cek_sensor();
    PH_1 = PH_1 + PH;
    delay(50);
  }
  fix_PH = PH_1 / 30 * -1;
  if (fix_PH < 0) {
    fix_PH = 0;
  }
  else if (fix_PH >= 14) {
    fix_PH = 14;
  }
  PH_1 = 0;

}


void cek_sensor() {
  adcPH = analogRead(sensorPH);                                     //membaca nilai ADC dari sensor PH & menyimpannya pada variable adcPH
  adcKelembaban = analogRead(sensorKelembaban);                     //membaca nilai ADC dari sensor Kelembaban & menyimpannya pada variable adcKelembaban
  PH = (-0.0693 * adcPH) + 7.3855;
}
