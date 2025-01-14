#pragma once

#include <e_uart_msg.h>

class c_uart_com
{
private:
    float I_Limit_Set;
    float I_Limit_Trip;
    float Soft_Start_Stop_up;
    float Soft_Start_Stop_down;
    
    unsigned char Freq;
    unsigned char Angle_percent;
    float Angle_deg;

    float I_Run;
    float V_Run;

    // Status flags 1
    bool Analog_Dig_flg;
    bool Resist_Induct_flg;
    bool delay_flg;
    bool Soft_start_flg;
    bool limit_Input_flg;
    bool Reverse_Output_flg;

    // Status flags 2
    bool burst_phase_flg;
    bool change_Phase_flg;
    bool toggle_Trip_warning;
    bool I_Limit_blink;
    bool Warning_Sound;
    bool Phase_fault;

    float FloatData[9];

public:
    c_uart_com();

    void read(unsigned char (&DataIn)[2][20]);
    
    float Output_var(unsigned int VarNumb);

    // status flags 1
    bool Analog_Dig();
    bool Resist_Induct();
    bool delay();     
    bool Soft_start();  
    bool limit_Input(); 
    bool Reverse_Output(); 
    
    ~c_uart_com();
};

c_uart_com::c_uart_com(/* args */)
{
}

c_uart_com::~c_uart_com()
{
}

void c_uart_com::read(unsigned char (&DataIn)[2][20])
{
  I_Limit_Set   = float((int(DataIn[0][3]) << 8) + DataIn[0][2])/10.0;
  I_Limit_Trip  = float((int(DataIn[0][5]) << 8) + DataIn[0][4])/10.0;

  Soft_Start_Stop_up    = float((int(DataIn[0][7]) << 8) + DataIn[0][6])/100.0; 
  Soft_Start_Stop_down  = float((int(DataIn[0][9]) << 8) + DataIn[0][8])/100.0;

  Analog_Dig_flg      = bool(DataIn[0][10] && 0b1);
  Resist_Induct_flg   = bool(DataIn[0][10] && 0b10);
  delay_flg           = bool(DataIn[0][10] && 0b100);
  Soft_start_flg      = bool(DataIn[0][10] && 0b1000);
  limit_Input_flg     = bool(DataIn[0][10] && 0b10000);
  Reverse_Output_flg  = bool(DataIn[0][10] && 0b100000);

  Freq          = DataIn[0][11];
  Angle_percent = DataIn[0][12];
  Angle_deg     = (float(DataIn[0][12]) * 180.0) / 100.0;

  I_Run = float((int(DataIn[0][14]) << 8) + DataIn[0][13])/10.0;

  burst_phase_flg     = bool(DataIn[0][15] && 0b1);
  change_Phase_flg    = bool(DataIn[0][15] && 0b10);
  toggle_Trip_warning = bool(DataIn[0][15] && 0b100);
  I_Limit_blink       = bool(DataIn[0][15] && 0b1000);
  Warning_Sound       = bool(DataIn[0][15] && 0b10000);
  Phase_fault         = bool(DataIn[0][15] && 0b100000);

  V_Run = float((int(DataIn[0][17]) << 8) + DataIn[0][16])/10.0;

}

float c_uart_com::Output_var(unsigned int VarNumb)
{
  FloatData[0] = I_Limit_Set;
  FloatData[1] = I_Limit_Trip;
  FloatData[2] = Soft_Start_Stop_up;
  FloatData[3] = Soft_Start_Stop_down;
  FloatData[4] = float(Freq);
  FloatData[5] = float(Angle_percent);
  FloatData[6] = Angle_deg;
  FloatData[7] = I_Run;
  FloatData[8] = V_Run;

  if (VarNumb > 8)
  {
    VarNumb = VarNumb % 9;
  }
  
  return FloatData[VarNumb];
}

// B6C function bits
bool c_uart_com::Analog_Dig()
{
    return Analog_Dig_flg;
}

bool c_uart_com::Resist_Induct()
{
    return Resist_Induct_flg;
}

bool c_uart_com::delay()
{
    return delay_flg;
}

bool c_uart_com::Soft_start()
{
    return Soft_start_flg;
}

bool c_uart_com::limit_Input()
{
    return limit_Input_flg;
}

bool c_uart_com::Reverse_Output()
{
    return Reverse_Output_flg;
}


