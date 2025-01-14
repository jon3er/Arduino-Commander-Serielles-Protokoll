#pragma once

// enum 

typedef enum e_strt_msgbytesLength
{
    //start msg byte position in input array
    st_msg1_byte1 = 0,
    st_msg1_byte2 = 1,
    st_msg1_end   = 2,

    st_msg2_byte1 = 3,
    st_msg2_byte2 = 4,
    st_msg2_end   = 26,

    st_msg3_byte1 = 27,
    st_msg3_byte2 = 28,
    st_msg3_end   = 36,

    st_msg4_byte1 = 37,
    st_msg4_byte2 = 38,
    st_msg4_end   = 70,

}UART_START;

typedef enum e_com_msgbytes
{    
    // Com msg byte positions in input array
    c_msg1_byte1 = 0,
    c_msg1_byte2 = 1,
    c_msg1_end   = 19,

    c_msg2_byte1 = 20,
    c_msg2_byte2 = 21,
    c_msg2_end   = 38,
}UART_COM;

typedef enum e_combytes
{
    Startbyte = 0x55
}COM_BYTE;

typedef enum e_messages
{
    // start array info
    MaxLgthStrt = 34,
    numberStart  = 4,
    // com array info
    MaxLgthCom = 20,    
    numberCom = 2,
}MSG_TYPE;
