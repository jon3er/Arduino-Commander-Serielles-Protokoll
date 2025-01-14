#include <Arduino.h>
#include <e_uart_msg.h>
#include <uart_com.h>

int Uart_Data_RW(int &bytecnt, unsigned char &Data_in, unsigned char &Data_out)
{
	if (Serial.available() > 0)
	{
		//read Data
		Data_in = Serial.read();
		//write Data
		Serial.write(Data_out);
		//next byte
		bytecnt++;

		return 1;
	}
	else
	{
		return 0;
	}
}

int Uart_Data_RW_Copy(int &bytecnt, unsigned char &Data_in)
{
	if (Serial.available() > 0)
	{
		//read Data
		Data_in = Serial.read();
		//write read Data
		Serial.write(Data_in);
		//next byte
		bytecnt++;

		return 1;
	}
	else
	{
		return 0;
	}
}

bool InterruptMs(unsigned long Time)
{
  static unsigned long TimePassed;

    if ((micros() - TimePassed) <= (Time*1000))
    {
        return false;
    }
    else
    {
        TimePassed = micros();
        return true;
    }
}