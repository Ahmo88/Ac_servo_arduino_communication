
#include <ModbusMaster.h>

static uint32_t i;
uint8_t j, result;
uint16_t data[6];
boolean st = false;
int a = 0;
int b = 0;
int x;
boolean jednom = false;

int limitPrekidac = 24;
int buttonPin = 2;
int buttonGore = 3;
int lampa = 13;
boolean prosloStanje = false;
boolean limitProsloStanje = true;


// instantiate ModbusMaster object
ModbusMaster node;
#include <CmdMessenger.h>
CmdMessenger cmdMessenger = CmdMessenger(Serial);

enum
{
  paljenjeServa,
  gasenjeServa,
  brzina1,
  brzina2,
  brzina3,
  manualBrzina,
  pozicija1,
  pozicija2,
  idi,
  homePozicija,
  parametarVisual,
  vrijednostVisual,
  trenutnaPozicija,
  okidacTrenutnePozicije,
  trenutnaPozicija2,
  stani,
  gore,
  goreStani,
  dole,
  doleStani,
  homeOn,
};
void attachCommandCallbacks()
{
  cmdMessenger.attach(paljenjeServa, paliServo);
  cmdMessenger.attach(gasenjeServa, ugasiServo);
  cmdMessenger.attach(brzina1, brzinaPrva);
  cmdMessenger.attach(brzina2, brzinaDruga);
  cmdMessenger.attach(brzina3, brzinaTreca);
  cmdMessenger.attach(manualBrzina, podesiBrzinu);
  cmdMessenger.attach(pozicija1, pozicijaPrva); // pozicija iz visuala poziva void u arduinu
  cmdMessenger.attach(pozicija2, pozicijaDruga);
  cmdMessenger.attach(homePozicija, homeKreni);
  cmdMessenger.attach(idi, triger);
  cmdMessenger.attach(parametarVisual, parametar1);
  cmdMessenger.attach(vrijednostVisual, vrijednost);
  cmdMessenger.attach(stani, Stop);
  cmdMessenger.attach(gore, idiGore);
  cmdMessenger.attach(goreStani, staniGore);
  cmdMessenger.attach(dole, idiDole);
  cmdMessenger.attach(doleStani, staniDole);
 cmdMessenger.attach(homeOn,homeStaniKreni);

}

void setup()
{
  pinMode(limitPrekidac, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buttonGore, INPUT_PULLUP);
  pinMode(lampa, OUTPUT);

  // use Serial (port 0); initialize Modbus communication baud rate
  Serial.begin(19200);
  Serial1.begin(19200, SERIAL_8N2);
  cmdMessenger.printLfCr();
  attachCommandCallbacks();

 // communicate with Modbus slave ID 2 over Serial (port 0)
  node.begin(2, Serial1);  // Ime motora

}
void loop()
{
   
  cmdMessenger.feedinSerialData();
  delay(50);

  if ( digitalRead (buttonGore) ==  LOW  )  // za dugme
  {
    prosloStanje = true;
    result = node.writeSingleRegister(120, -200);
    delay(50);
    result = node.writeSingleRegister(71, 4095);
    delay(50);
    result = node.writeSingleRegister(71, 3071);
 
  }
  else if (prosloStanje == true)
  {

    delay(50);

    result = node.writeSingleRegister(71, 2047);
    prosloStanje = false;

  }   

 if (digitalRead (limitPrekidac)== HIGH) 
{
  if (limitProsloStanje = true)

{
  
  result = node.writeSingleRegister(71, 2047);
  delay(50);

 limitProsloStanje = false;
}
  else if
  (limitProsloStanje == false)
   {
    homeStaniKreni();
    
    }
  } 
   
  if ( digitalRead (buttonPin) == LOW )
  {
    if (jednom == true)
    {
      delay(260); //  OVDE MORA BITI VISE NAJMANJE 260 DELAY ZBOG POZICIJA
      triger();
      jednom = false;
    }
  }

  int j = 0;
  int i = 0;
  cmdMessenger.feedinSerialData();
  delay(50);
  result = node.readHoldingRegisters(387, 2);
  if (result == node.ku8MBSuccess)
  {

    j = node.getResponseBuffer(0); //low bit
    i = node.getResponseBuffer(1); //high bit
  }
  cmdMessenger.sendCmd(trenutnaPozicija, i);
  cmdMessenger.sendCmd(trenutnaPozicija2, j);
}
void paliServo()
{
  cmdMessenger.feedinSerialData();
  delay(50);
  result = node.writeSingleRegister(3, 1);
}

void ugasiServo()
{

  cmdMessenger.feedinSerialData();
  delay(50);
  result = node.writeSingleRegister(3, 0);
  delay(50);
  //  result = node.writeSingleRegister(8, 300); //torque naprijed 300
  // delay(50);
  //  result = node.writeSingleRegister(9, -300);//torque nazad 300
 
}

void brzinaPrva()
{
  cmdMessenger.feedinSerialData();
  delay(50);
  result = node.writeSingleRegister(128, 300);
}

void brzinaDruga()
{
  cmdMessenger.feedinSerialData();
  delay(50);
  result = node.writeSingleRegister(128, 1500);
}
void brzinaTreca()
{
  cmdMessenger.feedinSerialData();
  delay(50);
  result = node.writeSingleRegister(128, 3000);
}

void podesiBrzinu()

{
  cmdMessenger.feedinSerialData();
  delay(50);
  a = cmdMessenger.readInt32Arg();
  result = node.writeSingleRegister(128, a);
}

void pozicijaPrva()  // manji broj krugova registar 120
{
  cmdMessenger.feedinSerialData();
  delay(50);
  a = cmdMessenger.readInt32Arg();
  result = node.writeSingleRegister(120, a);
  delay(50);
  jednom = true;
}

void pozicijaDruga()  // veci broj krugova registar 121
{
  cmdMessenger.feedinSerialData();
  delay(50);
  a = cmdMessenger.readInt32Arg();
  result = node.writeSingleRegister(121, a);
  jednom = true;
}

void homeKreni()

{
 
  cmdMessenger.feedinSerialData();
  delay(50);
  //result = node.writeSingleRegister(9, -10);
 // delay(50);
  result = node.writeSingleRegister(120, -100);
  delay(50);
  result = node.writeSingleRegister(71, 4095);
  delay(50);
  result = node.writeSingleRegister(71, 3071);
 
}
 void homeStaniKreni()
 {
 
  result = node.writeSingleRegister(120, 10);
  delay(500);
  result = node.writeSingleRegister(71, 4095);
  delay(50);
  result = node.writeSingleRegister(71, 3071);  
  delay (2000);
  limitProsloStanje = true;

 }
void triger()
{
  cmdMessenger.feedinSerialData();
  delay(50);
  result = node.writeSingleRegister(71, 4095);
  delay(50);
  result = node.writeSingleRegister(71, 3071);
}

void Stop()
{
  cmdMessenger.feedinSerialData();
  delay(50);
  result = node.writeSingleRegister(71, 2047);
}
void parametar1()
{
  cmdMessenger.feedinSerialData();
  delay(50);
  x = cmdMessenger.readInt32Arg();
}
void vrijednost()
{
  cmdMessenger.feedinSerialData();
  delay(50);
  int y;
  y = cmdMessenger.readInt32Arg();
  result = node.writeSingleRegister(x, y);
}

void idiGore() // pretvara void u loop cita ga stalno idiGore();

{
 
 cmdMessenger.feedinSerialData();
 delay(50);
 result = node.writeSingleRegister(120, -100);
 delay(50);
  result = node.writeSingleRegister(71, 4095);
 delay(50);
  result = node.writeSingleRegister(71, 3071);

}

void staniGore() // pretvara void u loop cita ga stalno ako se stavi u void lop  idiGore();

{
  cmdMessenger.feedinSerialData();
  delay(50); 
  result = node.writeSingleRegister(71, 2047);
  
}
void idiDole() 

{
  
  cmdMessenger.feedinSerialData();
  delay(50);
  result = node.writeSingleRegister(120, 100);
  delay(50);
  result = node.writeSingleRegister(71, 4095);
  delay(50);
  result = node.writeSingleRegister(71, 3071);
}

void staniDole() 

{
  cmdMessenger.feedinSerialData();
 delay(50);

  result = node.writeSingleRegister(71, 2047);

}
