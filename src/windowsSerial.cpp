#include "windowsSerial.h"

bool CustomSerialPort::openPort() {
    serialComm = CreateFile(TEXT("\\\\.\\COM9"),              // Name of the Port to be opened
                        GENERIC_READ | GENERIC_WRITE,  // Read/Write Access
                        0,                             // No Sharing, ports can't be shared
                        NULL,                          // No Security
                        OPEN_EXISTING,                 // Open existing port only
                        0,                             // Non Overlapped I/O
                        NULL);                         // Null for Comm Devices

    if (serialComm == INVALID_HANDLE_VALUE) {
        std::cerr << "\n   Error! - Port " << comport << " can't be opened\n";
        std::cerr << "   Check board connection and Port Number\n";
        return false;
    } else {
        std::cout << "\n   Port " << comport << " Opened\n";
        return true;
    }
    return false;
}

bool CustomSerialPort::setTimeouts() {
    COMMTIMEOUTS timeouts = { 0 };

    timeouts.ReadIntervalTimeout = 300;
    timeouts.ReadTotalTimeoutConstant = 300;
    timeouts.ReadTotalTimeoutMultiplier = 300;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(serialComm, &timeouts)) {
        std::cerr << "\n   Error! in Setting Timeouts\n";
        return false;
    } else {
        std::cout << "\n   Setting Serial Port Timeouts Successful\n";
        return true;
    }
}    

bool CustomSerialPort::configurePort() {
    DCB dcbSerialParams = { 0 };  // Initializing DCB structure
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(serialComm, &dcbSerialParams)) {
        std::cerr << "\n   Error! in GetCommState()";
        return false;
    }

    // Setting the parameters for the serial port
    dcbSerialParams.BaudRate = 9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.fBinary = 1;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = EVENPARITY;
    dcbSerialParams.fAbortOnError = TRUE;

    if (!SetCommState(serialComm, &dcbSerialParams)) {
        std::cerr << "\n   Error! in Setting DCB Structure";
        return false;
    } else {
        std::cout << "\n   Setting DCB Structure Successful\n";
        std::cout << "       Baudrate = " << dcbSerialParams.BaudRate << "\n";
        std::cout << "       ByteSize = " << static_cast<int>(dcbSerialParams.ByteSize) << "\n";
        std::cout << "       StopBits = " << static_cast<int>(dcbSerialParams.StopBits) << "\n";
        std::cout << "       Parity   = " << static_cast<int>(dcbSerialParams.Parity) << "\n";
        setTimeouts();
        return true;
    }
}

bool CustomSerialPort::setCommMask() {
    if (!SetCommMask(serialComm, EV_RXCHAR)) {
        std::cerr << "\n   Error! in Setting CommMask\n";
        return false;
    } else {
        std::cout << "\n   Setting CommMask successful\n";
        return true;
    }
}

// Reads from the serial port and returns the count of bytes read
uint32_t CustomSerialPort::readSerialPort(uint8_t *pBuffer, uint32_t len)
{
    DWORD no_of_bytes_read = 0;
    if (!ReadFile(serialComm, pBuffer, len, &no_of_bytes_read, NULL)) {
        std::cerr << "Error reading from serial port: " << GetLastError() << std::endl;
    }
    return static_cast<uint32_t>(no_of_bytes_read);
}

// Sends data over the serial port
void CustomSerialPort::writeToPort(const uint8_t *data_buf, uint32_t len)
{
    DWORD dNoOfBytesWritten = 0;
    BOOL status = WriteFile(serialComm,
                            data_buf,
                            len,
                            &dNoOfBytesWritten,
                            NULL);

    if (status == TRUE)
    {
        std::cout << "\n   Sending Command:\n";
        for(uint32_t i = 0 ; i < len ; i++)
        {
            std::cout << "   0x" << std::hex << (int)data_buf[i] << " ";
            if (i % 8 == 7) {
                std::cout << "\n";
            }
        }
    }
    else
    {
        std::cerr << "\n  Error " << GetLastError() << " in Writing to Serial Port" << std::endl;
    }
}

// Clears the serial port buffers
void CustomSerialPort::purgeSerialPort()
{
    if (!PurgeComm(serialComm, PURGE_RXCLEAR | PURGE_TXCLEAR)) {
        std::cerr << "Error purging serial port: " << GetLastError() << std::endl;
    }
}

// Closes the serial port
void CustomSerialPort::closePort()
{
    if (serialComm != INVALID_HANDLE_VALUE)
    {
        CloseHandle(serialComm);
        serialComm = INVALID_HANDLE_VALUE;
    }
    purgeSerialPort();
}

// to check the bytes received in the buffer
DWORD CustomSerialPort::checkReceivedBytes() {
    DWORD errors;                   // Stores any errors reported by ClearCommError
    COMSTAT status;                 // Stores the status info of the COM port

    // Use ClearCommError to check the state of the port and retrieve the number of bytes in the input buffer
    if (ClearCommError(serialComm, &errors, &status)) {
        return status.cbInQue;     // Number of bytes received and waiting in the input buffer
    } else {
        std::cerr << "Error checking received bytes" << std::endl;
        return 0;
    }
}

void CustomSerialPort::echoSerial()   {
    DWORD bytesAvailable = this->checkReceivedBytes();

    if (bytesAvailable > 0) {
        uint8_t *pbuf = new uint8_t[bytesAvailable];
        std::cout << "Bytes available to read: " << bytesAvailable << std::endl;
        this->readSerialPort(pbuf, bytesAvailable);
        for(uint32_t byte = 0; byte < bytesAvailable; byte++)   {
            cout << pbuf[byte];
        }
        delete[] pbuf;
    } else {
        std::cout << "No bytes available" << std::endl;
    }
}