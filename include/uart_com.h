#pragma once

int Uart_Data_RW(int &bytecnt, unsigned char &Data_in, unsigned char &Data_out);

int Uart_Data_RW_Copy(int &bytecnt, unsigned char &Data_in);

bool InterruptMs(unsigned long Time);