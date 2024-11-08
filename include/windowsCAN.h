#ifndef BLCH_
#define BLCH_

#include "main.h"
#include "PCANBasic.h"
#include "Filecommands.h"
#include <chrono>
#include <iomanip> 

using namespace std;

extern fileops fileobj;

/* Exported constants --------------------------------------------------------*/
#define ERROR_COMMAND                     0xECU             /* Error command */
#define ACK_BYTE                          0x79U             /* Acknowledge Byte ID */
#define NACK_BYTE                         0x1FU             /* No Acknowledge Byte ID */

//BL Commands
#define CMD_GET_COMMAND                   0x00U             /* Get commands command */
#define CMD_GET_VERSION                   0x01U             /* Get Version command */
#define CMD_GET_ID                        0x02U             /* Get ID command */
#define CMD_SPEED                         0x03U
#define CMD_WRITE_DATA                    0x04U             /* Speed command */
#define CMD_READ_MEMORY                   0x11U             /* Read Memory command */
#define CMD_WRITE_MEMORY                  0x31U             /* Write Memory command */
#define CMD_GO                            0x21U             /* GO command */
#define CMD_READ_PROTECT                  0x82U             /* Readout Protect command */
#define CMD_READ_UNPROTECT                0x92U             /* Readout Unprotect command */
#define CMD_ERASE_MEMORY                  0x43U             /* Erase Memory command */
#define CMD_WRITE_PROTECT                 0x63U             /* Write Protect command */
#define CMD_WRITE_UNPROTECT               0x73U             /* Write Unprotect command */
#define CMD_NS_WRITE_MEMORY               0x32U             /* No Stretch Write Memory command */
#define CMD_NS_ERASE_MEMORY               0x45U             /* No Stretch Erase Memory command */
#define CMD_NS_WRITE_PROTECT              0x64U             /* No Stretch Write Protect command */
#define CMD_NS_WRITE_UNPROTECT            0x74U             /* No Stretch Write Unprotect command */
#define CMD_NS_READ_PROTECT               0x83U             /* No Stretch Read Protect command */
#define CMD_NS_READ_UNPROTECT             0x93U             /* No Stretch Read Unprotect command */
#define CMD_SPECIAL_COMMAND               0x50U             /* Special Read Protect */
#define CMD_EXTENDED_SPECIAL_COMMAND      0x51U             /* Special write command */
#define CMD_CHECKSUM                      0xA1U             /* Checksum command */

extern const char* USER_APP;
extern const char* OUTPUT_FILE;

typedef struct atagTPCANMsg
{
    DWORD             ID;      // 11/29-bit message identifier
    TPCANMessageType  MSGTYPE; // Type of the message
    BYTE              LEN;     // Data Length Code of the message (0..8)
    BYTE              DATA[16]; // Data of the message (DATA[0]..DATA[7])
}aTPCANMsg;
class CustomCANPort    {
private:
    TPCANHandle canComm;                  //Windows handler
    uint16_t PCAN_channel;                    //PCAN channel detected, to be manually set so far.

public:
    CustomCANPort() : canComm(0xFFFF) {
    }

    ~CustomCANPort() {
        if (canComm != -1) {
            CAN_Uninitialize(PCAN_channel);
        }
    }

    uint16_t checkChannels();
    bool openPort();
    bool configurePort();
    bool writeToPort(TPCANMsg*);
    void closePort();
    TPCANStatus receiveCANMessage(TPCANMsg*);                                       //simple rx a message if any, if not return error
    TPCANStatus echoSerial(TPCANMsg* msg);                           //poll the CAN port, with modifying the ack status and message
    TPCANStatus readresponse(int timeoutms);    
    TPCANStatus flushbuffer(int timeoutms);                                    //read the response with an option to save the response to the output file
    TPCANStatus receiveAck(int timeoutms);                                       //read the response with an option to save the response to the output file
    void read_from_memory(uint32_t start_address, uint32_t total_bytes);            //reads the stm32 memory with starting address and total bytes in chunks of 248 bytes.
    void write_fw(const std::vector<uint8_t>& data, uint32_t start_address);        //write total_bytes to stm32f767 starting at start_address 
    void send_command_to_BL(uint32_t command_code);
    void print_req(TPCANMsg*);
    int ackstatus;
    aTPCANMsg mess;
    vector<aTPCANMsg> nowresponse;
   //class member of application struct
};

#endif