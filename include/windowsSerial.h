#ifndef WSERIAL_H
#define WSERIAL_H

#include "main.h"
#include "configure.h"

class CustomSerialPort    {
private:
    HANDLE serialComm;                  //Windows handler

public:
    CustomSerialPort() : serialComm(INVALID_HANDLE_VALUE) {}

    ~CustomSerialPort() {
        if (serialComm != INVALID_HANDLE_VALUE) {
            CloseHandle(serialComm);
        }
    }
    bool openPort();
    bool setTimeouts();
    bool configurePort();
    bool setCommMask();
    uint32_t readSerialPort(uint8_t *pBuffer, uint32_t len);
    void writeToPort(const uint8_t *data_buf, uint32_t len);
    void purgeSerialPort();
    void closePort();
    DWORD checkReceivedBytes();
    void echoSerial();


};




#endif