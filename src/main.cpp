// Dependencies
#include <Arduino.h>
#include <LiquidCrystal_RW1073.h>  // Modded LiquidCrystal.h lib
// functions
#include <uart_com.h>
// classes
#include <c_uart_com.h>
// enums
#include <e_uart_msg.h>


// für das Display EA DIP205J-6NLW muss die LiquidCrystalDisplay.h Bibliotek angepasst werden.
// die Anzeigen adresseierung muss zu  setRowOffsets(0x00, 0x20, 0x40, 0x60); auf Zeile 86 im LiquidCrystal.cpp geändert werden
// In Zeile 188 Folgende befehle zu den Startup Commands hinzufügen
//   command(0x2C);      //Function set RE on   
//   delayMicroseconds(150);
//   command(0x09);  //ext. Function set 4-line display
//   delayMicroseconds(150);
//   command(0x28);  //Function set RE off


// Communication var
int   BitNumb       = 0;
int   MsgNumb       = 0;
int   ComStatus     = 0;
bool  InitSend      = false;
bool  ComRunning    = false;
bool  StartupFin    = false;

// current byte count
int   cnt     = 0;
int   oldcnt  = 0;
int   lastcnt = 0;

// clear serial Buffer
int   VarBuffer = 0;

// lcd print flag
int   lcdPrintLine;
bool  LcdPrint      = false;

// custom lcd charater
unsigned char degreeSymbol[8]  = {0x0C,0x12,0x0C,0x00,0x00,0x00,0x00,0x00};

// Timeout Var
unsigned long   TimeOutCnt  = 0;
unsigned long   TimeLastCom = 0;
unsigned long   MaxTimeOut  = 150; // 150 ms Timeout

// Button Var
int   buttonPin       = 8;
int   buttonState     = 0;
int   buttonStateOld  = 0;
int   TimesPressed    = 0;

//Interrupt Timer Flags
bool  InterruptFlag15ms = false;
bool  InterruptFlag65ms = false;

// Startup
// msg Start bytes to B6C controller
unsigned char DataOutStart[MSG_TYPE::numberStart][2] ={
    //msg 1; 
    //3 byte long
    {0x20,0x20},
    //msg 2
    //24 byte long
    {0x36, 0x21},
    // msg 3
    // 10 byte long
    {0x61, 0x5A},
    //msg 4
    //34 byte long
    {0x81, 0x62},
  };
  // Copy read Data to array
unsigned char DataInStart[MSG_TYPE::numberStart][MSG_TYPE::MaxLgthStrt]={0};

// Data Transfer
// msg Start bytes to B6C controller
unsigned char DataOutCom[MSG_TYPE::numberCom][2]={
    //Com msg 1
    // 20 Byte long                                                            
    {0x48,0x37},
    //Com msg 2
    // 19 byte long
    {0x59,0x49}
  };
// Copy read Data to array
unsigned char DataInCom[MSG_TYPE::numberCom][MSG_TYPE::MaxLgthCom]={0};

// with the arduino pin number it is connected to
const int reset = 34, rs = 36, rw = 38,en = 40, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
// initialize the library by associating any needed LCD interface pin
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// init class for UART communication data
c_uart_com LcdData;


void setup(){
  // Button init
  pinMode(buttonPin, INPUT);
  // Display pin init
  pinMode(rw, OUTPUT);
  digitalWrite(rw,LOW);
  pinMode(reset,OUTPUT);
  digitalWrite(reset,HIGH);
  // Display setup 
  lcd.begin(20, 4);
  // custom degree symbol
  lcd.createChar(1,degreeSymbol);
  // serial setup
  Serial.begin(250000); //initialize serial communication at a 250000 baud rate
}

void loop(){

//---------------------------------Uart Communication-----------------------

//Init: detect device; discard old data & send Broadcast
if (!InitSend && (Serial.available() > 0))
{
  delay(2);

  Serial.write(COM_BYTE::Startbyte);

  // clear serial buffer
  while (Serial.available() > 0)
  {
    VarBuffer = Serial.read();
  }
  
  delay(10);

  InitSend = true;
}

// TODO: Check if it can be integrated with broadcast 
// Timer interrupts
if (StartupFin && !InterruptFlag15ms)
{
  InterruptFlag15ms = InterruptMs(15);
}
else if (!StartupFin && !InterruptFlag65ms)
{
  InterruptFlag65ms = InterruptMs(65);
}


// Broadcast communication start signal every 15 ms after startup
if (InterruptFlag15ms && StartupFin && !ComRunning)
{
  InterruptFlag15ms = false;
  Serial.write(COM_BYTE::Startbyte); 
} // Broadcast communication start signal every 65 ms during startup
else if ((Serial.available() == 0) && InterruptFlag65ms && !StartupFin && !ComRunning)
{
  InterruptFlag65ms = false;
  Serial.write(COM_BYTE::Startbyte);
}


// Startup for UART Communication
if (!StartupFin && InitSend)
{
  // for first two Msg bytes
  if((cnt == UART_START::st_msg1_byte1) ||(cnt == UART_START::st_msg1_byte2) || 
    	(cnt == UART_START::st_msg2_byte1) || (cnt == UART_START::st_msg2_byte2) || 
       (cnt == UART_START::st_msg3_byte1) || (cnt == UART_START::st_msg3_byte2) || 
        (cnt == UART_START::st_msg4_byte1) || (cnt == UART_START::st_msg4_byte2))
  {  
    // Write Msg start data
    ComStatus=Uart_Data_RW(BitNumb, DataInStart[MsgNumb][BitNumb], DataOutStart[MsgNumb][BitNumb]);

    if (ComStatus == 1)
    {
      ComRunning  = true;
      cnt++;
    }
  } 
  else if((cnt == UART_START::st_msg1_end) || (cnt == UART_START::st_msg2_end) || (cnt == UART_START::st_msg3_end)) // end of Msg 1-3
  {  
    // Read and then copy input data to serial output
    ComStatus=Uart_Data_RW_Copy(BitNumb, DataInStart[MsgNumb][BitNumb]);

    if (ComStatus == 1)
    {
      ComRunning  = false;
      BitNumb     = 0;
      MsgNumb++;
      cnt++;
    }
  }
  else if (cnt == (UART_START::st_msg4_end + 1))  // end of Startup
  { 
    StartupFin  = true;
    ComRunning  = false;
    cnt         = 0;
    MsgNumb     = 0;
    BitNumb     = 0;
    delay(5);  // TODO: Check if can be removed
  }
  else // for msg body
  { 
    // Read and then copy input data to serial output
    ComStatus=Uart_Data_RW_Copy(BitNumb, DataInStart[MsgNumb][BitNumb]);
    
    if (ComStatus == 1)
    {
      ComRunning  = true;
      cnt++;
    }
  }
}
else  if (InitSend) // UART data transfer
{
  // for first two msg bytes
  if((cnt == UART_COM::c_msg1_byte1) || (cnt == UART_COM::c_msg1_byte2) || 
      (cnt == UART_COM::c_msg2_byte1) || (cnt == UART_COM::c_msg2_byte2))
  {  
    ComStatus=Uart_Data_RW(BitNumb, DataInCom[MsgNumb][BitNumb], DataOutCom[MsgNumb][BitNumb]);

    if (ComStatus == 1)
    {
      ComRunning    = true;
      cnt++;
    }
  }
  else if (cnt == UART_COM::c_msg1_end)  // end of msg 1
  { 
    // Read and then copy input data to serial output   
    ComStatus=Uart_Data_RW_Copy(BitNumb, DataInCom[MsgNumb][BitNumb]);

    if (ComStatus == 1)
    {
      ComRunning  = false;
      BitNumb     = 0;
      MsgNumb++;
      cnt++;
    }
  }
  else if (cnt == UART_COM::c_msg2_end + 1)  // end of msg 2
  {
    // reset com var
    cnt         = 0;
    MsgNumb     = 0;
    BitNumb     = 0;
    ComRunning  = false;
    LcdPrint    = true;
    LcdData.read(DataInCom);  // pass read data to LcdData class
  }
  else // for msg body
  { 
    // Read and then copy input data to serial output   
    ComStatus=Uart_Data_RW_Copy(BitNumb, DataInCom[MsgNumb][BitNumb]);

    if (ComStatus == 1)
    {
      ComRunning  = true;
      cnt++;
    }
  }
}


// Check connection
if (cnt == oldcnt)
{
  TimeOutCnt = millis() - TimeLastCom;

  if (TimeOutCnt > MaxTimeOut)
  {
    cnt         = 0;
    oldcnt      = 0;
    lastcnt     = 0;
    MsgNumb     = 0;
    BitNumb     = 0;
    ComRunning  = false;
    StartupFin  = false;
    LcdPrint    = true;
    TimeLastCom = millis();   //reset timeout interval
  }
}
else
{
  TimeLastCom = millis();
}
oldcnt= cnt;


// for debug
if (lastcnt < cnt && StartupFin)
{
  lastcnt = cnt;
}


//-----------------------------------------Data In / Output devices-----------------


//Check button
buttonState = digitalRead(buttonPin);

if (buttonState != buttonStateOld)
{
  if(buttonState == HIGH)
  {
    TimesPressed++; 
  }
}
buttonStateOld = buttonState;

// only print one line per cycle to optimize cycle time
if (LcdPrint)
{
  if (lcdPrintLine < 3)
  {
    lcdPrintLine++;
  }
  else
  {
    lcdPrintLine = 0;
  }
}

// Print Data to LCD Screen
if (LcdPrint && !ComRunning)
{
  switch (lcdPrintLine)
  {
  case 0:
  {
    lcd.clearLine(0);
    lcd.setCursor(0, 0);
    lcd.print(" Z");
    lcd.write(0x7E); // ü
    lcd.print("ndwinkel: ");
    lcd.print(LcdData.Output_var(6));
    lcd.write(0x01);    // degree symbol
    break;
  }
  case 1:
  {
    lcd.clearLine(1);
    lcd.setCursor(0, 1);
    lcd.print("   Frequenz: " );
    lcd.print(int(LcdData.Output_var(4)));
    lcd.print("Hz" );
    break;
  }
  case 2:
  {
    lcd.clearLine(2);
    lcd.setCursor(0, 2);
    if (LcdData.Resist_Induct())
    {
      lcd.print("   Induktive ");
    }
    else
    {
      lcd.print("    Ohm'sche ");
    }
    lcd.print("Last");
    break;
  }
  case 3:
  {
    lcd.clearLine(3);
    lcd.setCursor(0, 3);
    if (LcdData.Reverse_Output())
    {
      lcd.print("  L1,L2,L3 ");
    }
    else
    {
      lcd.print("  L3,L2,L1 ");

    }
    lcd.print("Phasen");
    break;
  }  
  default:
  {
    break;
  }
  }
  LcdPrint = false;
}

}