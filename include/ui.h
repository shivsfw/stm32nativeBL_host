#ifndef UI_H
#define UI_H

#include "main.h"
#include "windowsCAN.h"
#include "windowsSerial.h"

CustomSerialPort sp;
CustomCANPort cp;

uint8_t follow_command(char* cmd, void* ptport)  {
#if defined(OVRSERIAL)
    CustomSerialPort* sPort = static_cast<CustomSerialPort*>(ptport);
#else
    CustomCANPort* sPort = static_cast<CustomCANPort*>(ptport);
#endif

    switch(*cmd)
    {
        case 'h':
            printf("\r\n Commands \r\n");
            printf("o : open port\r\n");
            printf("c : close port\r\n");
            //printf("s : start listening\r\n");
            //printf("z : stop listening\r\n");
            printf("q : Quit from the application\r\n");
            printf("0 : Send SYNC command to BL\r\n");
            printf("1 : Send GET\r\n");
            printf("2 : Send GET ID\r\n");
            printf("3 : Read MEM\r\n");
            printf("4 : Erase MEM\r\n");
            printf("5 : Write to MEM\r\n");
            printf("6 : Go to Application\r\n");
            printf("7 : Flush the CAN-bus buffer\r\n");
            printf("8 : Compare files\r\n");
            *cmd = 0;

        break;
        
        case 'o':
            if(sPort->openPort())   {
                sPort->configurePort();
            }

            *cmd = 0;  
        break;

        case 'c':
            sPort->closePort();
            *cmd = 0;
        break;

        case 's':   {
            // uint8_t ser_cmd = 0;
            // while(ser_cmd != 'q')   {
            //     if(_kbhit())    {
            //         ser_cmd = _getch();
            //         if (ser_cmd == 'z') {
            //             break;
            //         }
            //     }
            //     sPort->echoSerial();
            //     Sleep(1000);
            // }
            *cmd = 0;
        }
        break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':   
        case '6':
        case '7':
        case '8':
#if defined(OVRSERIAL)
            uint8_t data_buf = 0x7f;
            uint8_t response_Data[132];
            sPort->writeToPort(&data_buf, 1);
            Sleep(100);
            sPort->readSerialPort(response_Data, 1);
            printf("\r\nBL response %d \r\n", response_Data[0]);
#else
            // TPCANMsg message;
            // message.DATA[0] = 1;
            // message.ID = 0x79;
            // message.LEN =1;
            // message.MSGTYPE = PCAN_MESSAGE_STANDARD;
            // sPort->writeToPort(message);
            // Sleep(100);
            // sPort->readresponse(1000);
            sPort->send_command_to_BL(*cmd);
#endif
            *cmd = 0;
        break;
        
        default:
        break;
    }
    return 0;
}

#endif