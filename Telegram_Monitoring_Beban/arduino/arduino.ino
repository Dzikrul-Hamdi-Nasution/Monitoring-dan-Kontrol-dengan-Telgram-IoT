#include <SoftwareSerial.h>
#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor emon1;                   // Create an instance
#include "ZMPT101B.h"
SoftwareSerial mySerial(2, 3); // RX, TX
ZMPT101B voltageSensor(A1);
int sensorValue_sensor_tegangan = 0, tegangan_PLN;
int relay = 7;
float regresi_arus, daya_kwh;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  mySerial.begin(115200);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
  emon1.current(A0, 111.1);
  voltageSensor.calibrate();
}

void loop() {
  // put your main code here, to run repeatedly:
  arus();
  daya();
  tegangan();
  kirim();

  if (regresi_arus > 400) {
    digitalWrite(relay, HIGH);
  }
  else {
    digitalWrite(relay, LOW);
  }

  delay(1000);

}



void kirim() {
  mySerial.print("*");
  mySerial.print(tegangan_PLN);
  mySerial.print(",");
  mySerial.print(regresi_arus);
  mySerial.print(",");
  mySerial.print(daya_kwh);
  mySerial.println("#");

  Serial.print("*");
  Serial.print(tegangan_PLN);
  Serial.print(",");
  Serial.print(regresi_arus);
  Serial.print(",");
  Serial.print(daya_kwh);
  Serial.println("#");
}



void arus() {
  double Irms = emon1.calcIrms(500);  // Calculate Irms only
  regresi_arus = Irms * 220 / 30;
  //sensorValue_sensor_arus = -14.9729 + (1.2862 * regresi_arus);

}

void daya() {
  daya_kwh = daya_kwh + (tegangan_PLN * regresi_arus / 1000) / 3600  ;
}

void tegangan() {
  float U = voltageSensor.getVoltageAC();
  float regresi_tegangan = U * 10;
  tegangan_PLN = -0.6918 + (1.0292 * regresi_tegangan);
}
