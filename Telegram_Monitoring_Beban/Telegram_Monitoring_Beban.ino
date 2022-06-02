#include <Wire.h>
#include "CTBot.h"                                                  // Memanggil library CTBot
CTBot myBot;
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

String ssid  = "Server Project";                                // ganti ini dengan nama jaringannya
String pass  = "Salamproject2022";
String token = "5264958293:AAH3p58M95LIx9dzkNHyfEADHGfb_QWjA7E";     // token telegram

int  data_tegangan_PLN, i, kunci = 0;
float data_regresi_arus, data_daya_kwh;
String dataIn;
String dt[10];
boolean parsing = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);


  lcd.begin();                                                         // Memulai komunikasi dengan LCD
  lcd.backlight();
  lcd.setCursor( 0 , 0);
  lcd.print("Initialization");
  myBot.wifiConnect(ssid, pass);                                     // Menghubungkan ke jaringan internet
  myBot.setTelegramToken(token);                                     // Menghubungkan ke telegram
  if (myBot.testConnection()) {                                      // Jika terhubung dengan jaringan internet, maka
    lcd.setCursor( 0 , 1);
    lcd.print("System Online ");                           // Tampil "testConnection OK" pada serial monitor
    //myBot.sendMessage(1883697320, "System Online");
    myBot.sendMessage(1441844129, "System Online");
  }
  else {                                                             // Jika tidak, maka
    lcd.setCursor( 0 , 1);
    lcd.print("System Offline ");                          // Tampil "testConnection NOK" pada serial monitor
  }



  delay(1500);
  lcd.clear();

}

void loop() {
  // put your main code here, to run repeatedly:
  TBMessage msg;

  //ambil_data();
  if (Serial.available() > 0)
  {
    char inChar = (char)Serial.read();
    dataIn += inChar;
    if (inChar == '\n') {
      parsing = true;
    }
  }
  if (parsing)
  {
    ambil_data();
    parsing = false;
    dataIn = "";
  }


  lcd.setCursor( 0 , 0);
  lcd.print("Tegangan = ");
  lcd.print(data_tegangan_PLN);



  if (myBot.getNewMessage(msg)) {
    if (msg.text.equalsIgnoreCase("cek")) {
      lcd.setCursor( 0 , 0);
      lcd.print("Update Data");
      myBot.sendMessage(msg.sender.id, "Arus = " + String(data_regresi_arus) + " mA");
      myBot.sendMessage(msg.sender.id, "Daya = " + String(data_daya_kwh) + " KWh");
    }
    else {
    }
  }
  if (data_regresi_arus > 400) {

    if (kunci == 0) {
      myBot.sendMessage(msg.sender.id, "Arus Melebihi 400 MiliAmpere");
      // myBot.sendMessage(1883697320, "Arus Melebihi 400 MiliAmpere");
      lcd.clear();
      lcd.setCursor( 5 , 0);
      lcd.print("WARNING");
      lcd.setCursor( 0 , 1);
      lcd.print("Arus Berlebih");
      delay(2000);
      lcd.clear();
      kunci = 1;
    }
    lcd.setCursor( 0 , 1);
    lcd.print("Arus = ");
    lcd.print(data_regresi_arus);
    lcd.setCursor( 14 , 1);
    lcd.print("mA");
    kunci = 0;
  } else {
    lcd.setCursor( 0 , 1);
    lcd.print("Arus = ");
    lcd.print(data_regresi_arus);
    lcd.setCursor( 14 , 1);
    lcd.print("mA");
    kunci = 0;
  }




}


void ambil_data() {
  int j = 0;
  dt[j] = "";
  for (i = 1; i < dataIn.length(); i++)
  {
    if ((dataIn[i] == '#') || (dataIn[i] == ','))
    {
      j++;
      dt[j] = ""; //inisialisasi variabel array dt[j]
    }
    else
    {
      dt[j] = dt[j] + dataIn[i];
    }
  }
  data_tegangan_PLN = dt[0].toInt();
  data_regresi_arus = dt[1].toFloat();
  data_daya_kwh = dt[2].toFloat();
  Serial.print("data 1 : ");  //sensor tegangan
  Serial.println(dt[0]);
  Serial.print("data 2 : ");  //sensor arus
  Serial.println(dt[1]);
  Serial.print("data 3 : ");  //Daya
  Serial.println(dt[2]);


}
