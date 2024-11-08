#include "windowsCAN.h"

uint16_t CustomCANPort::checkChannels()    {
    TPCANHandle channelsToCheck[] = { PCAN_USBBUS1, PCAN_USBBUS2, PCAN_USBBUS3 }; 
    DWORD condition; 
    for (int i = 0; i < 3; i++) { 
        if (CAN_GetValue(channelsToCheck[i], PCAN_CHANNEL_CONDITION, &condition, sizeof(condition)) == PCAN_ERROR_OK) 
            if ((condition & PCAN_CHANNEL_AVAILABLE) == PCAN_CHANNEL_AVAILABLE) 
                printf("The channel-handle 0x%X is AVAILABLE\n", channelsToCheck[i]); 
                return channelsToCheck[i];
    }
    return 0;
}

bool CustomCANPort::openPort()   {
    uint16_t channel_temp = 0;
    channel_temp = checkChannels();
    if (channel_temp)   {
        PCAN_channel = channel_temp;
        return true;
    } 
    PCAN_channel = 0;
    return false;
}

bool CustomCANPort::configurePort()    {
    TPCANStatus status = CAN_Initialize(PCAN_channel, PCAN_BAUD_125K, 0, 0, 0);
    if (status != PCAN_ERROR_OK) {
        std::cout << "Failed to initialize CAN channel. Error: " << status << std::endl;
        return false;
    }
    std::cout << "CAN channel opened successfully." << std::endl;    
    return true;    
}

// return false if status is not PCAN_ERROR_OK
bool CustomCANPort::writeToPort(TPCANMsg* message) {
    print_req(message);
    TPCANStatus status = CAN_Write(PCAN_channel, message);

    if (status != PCAN_ERROR_OK) {
        std::cerr << "Failed to send message. Error: " << status << "\n";
        return false;
    }
    std::cout << "\n Message sent successfully\n";
    return true;
}

void CustomCANPort::closePort()
{
    TPCANStatus status = PCAN_ERROR_OK;
    if (canComm != -1) {
        status = CAN_Uninitialize(PCAN_channel);
    }
    std::cout << "\r\n The return " << status << endl;
}

TPCANStatus CustomCANPort::receiveCANMessage(TPCANMsg* message) {
    TPCANTimestamp timestamp;
    TPCANStatus status = CAN_Read(PCAN_channel, message, &timestamp);

    if (status != PCAN_ERROR_OK) {
        std::cout << "Failed to read message. Error: " << std::hex << status << "\n";
    }
    return status;
}

// This function will echo the message rxd on CAN port on the console.
// gotAck will be 1 if it gets ACK message.
// gotACk will be same NACK byte if it gets ACK message.
TPCANStatus CustomCANPort::echoSerial(TPCANMsg* message)   {
    TPCANStatus status;

    status = receiveCANMessage(message);
    if(status == PCAN_ERROR_OK)     {
        ackstatus =  message->DATA[0];

        //echo data to console.
        std::cout << "<== Message rxd:";
        std::cout << "ID: " << std::hex << message->ID << "\tData: " << std::setw(2) << std::setfill('0');
        for (int i = 0; i < message->LEN; ++i) {
            std::cout << std::hex << std::setw(2) << std::setfill('0')<< static_cast<int>(message->DATA[i]) << "\t";
        }
        std::cout << "\n";
        //{                                          // uncomment this if want to save data in global vector and process it later.
        // mess.ID = message->ID;
        // mess.MSGTYPE = message->MSGTYPE;
        // mess.LEN = message->LEN;
        // mess.DATA[0] = message->DATA[0];          //I repeated this for 8 bytes
        // mess.DATA[1] = message->DATA[1];
        // mess.DATA[2] = message->DATA[2];
        // mess.DATA[3] = message->DATA[3];
        // mess.DATA[4] = message->DATA[4];
        // mess.DATA[5] = message->DATA[5];
        // mess.DATA[6] = message->DATA[6];
        // mess.DATA[7] = message->DATA[7];

        // nowresponse.push_back(mess);
        //}
    }
    return status;
}

TPCANStatus CustomCANPort::flushbuffer(int timeoutms)    {
    TPCANStatus status;
    TPCANMsg message = {};

    vector<uint8_t> response_datas;

    auto startTime = std::chrono::steady_clock::now();  // start time
    while(1)    {
        memset(&message, 0, sizeof(TPCANMsg));
        status = echoSerial(&message);

        if(status == PCAN_ERROR_OK) {  
            continue;           //continue scanning port for incoming messages
        } else if (status != PCAN_ERROR_QRCVEMPTY)  {
            std::cerr << "Error reading \n" << std::hex << status;
            return status;                  
        }

        //Check for timeout.
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
        if (elapsedTime >= timeoutms) {
            std::cerr << "Timeout! No CAN message received within " << timeoutms << " ms." << std::endl;
            return PCAN_ERROR_QRCVEMPTY;  // Timeout or empty queue            
        }
        Sleep(10);
    }
}

// Scan the CAN port for any rxd messsages (and echo the message if rxd any) until times out. 
// Read responses until it receives ACK message from BL.
TPCANStatus CustomCANPort::readresponse(int timeoutms)    {
    TPCANStatus status;
    TPCANMsg message = {};

    vector<uint8_t> response_datas;

    auto startTime = std::chrono::steady_clock::now();  // start time
    while(1)    {
        memset(&message, 0, sizeof(TPCANMsg));
        status = echoSerial(&message);

        if(status == PCAN_ERROR_OK) {  
            if ((message.LEN == 1) && (message.DATA[0] == ACK_BYTE))    {
                return status;
            } else if ((message.LEN == 1) && (message.DATA[0] == NACK_BYTE))    {
                cout << "\nNACK rxd, exiting now.\n";
                return status;
            }
            response_datas.clear();
            // response_datas.push_back((message.ID >> 24) & 0xff);
            // response_datas.push_back((message.ID >> 16) & 0xff);
            // response_datas.push_back((message.ID >> 8) & 0xff);
            // response_datas.push_back(message.ID & 0xff);
            // response_datas.push_back(0x11);         //H.tab
            for (int i = 0; i < 8; ++i) {
                response_datas.push_back(message.DATA[i]);  // Add only 8 elements.
            }
            // response_datas.push_back(0x13);         //CR
            // response_datas.push_back(0x10);         //LF
            fileobj.append_to_file(OUTPUT_FILE, response_datas);
            continue;           //continue scanning port for incoming messages
        } else if (status != PCAN_ERROR_QRCVEMPTY)  {
            std::cerr << "Error reading \n" << std::hex << status;
            return status;                  
        }

        //Check for timeout.
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
        if (elapsedTime >= timeoutms) {
            std::cerr << "Timeout! No CAN message received within " << timeoutms << " ms." << std::endl;
            return PCAN_ERROR_QRCVEMPTY;  // Timeout or empty queue            
        }
        Sleep(10);
    }
}
  
TPCANStatus CustomCANPort::receiveAck(int timeoutms)    {
    TPCANMsg message = {};
    auto startTime = std::chrono::steady_clock::now();

    while (true) {
        memset(&message, 0, sizeof(TPCANMsg));
        TPCANStatus status = echoSerial(&message);
        if (status == PCAN_ERROR_OK) {
            return status;
        } else if (status != PCAN_ERROR_QRCVEMPTY) {
            std::cerr << "Error reading CAN: " << std::hex << status << std::endl;
            return status;
        } else if (status == PCAN_ERROR_QRCVEMPTY)  {
            continue;       //continue scanning the port if the queue was empty, letting BL send msg.
        }

        // Handle timeout
        auto elapsed = std::chrono::steady_clock::now() - startTime;
        if (std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() >= timeoutms) {
            std::cerr << "Timeout waiting for ACK/NACK.\n";
            return PCAN_ERROR_QRCVEMPTY;
        }

        Sleep(10);  // Avoid busy waiting
    }
}

void CustomCANPort::read_from_memory(uint32_t start_address, uint32_t total_bytes) {
    const uint32_t MAX_READ_SIZE = 248;  // Maximum bytes read in one go
    uint32_t current_address = start_address; 
    uint32_t bytes_left = total_bytes;
    uint32_t bytes_to_read;
    TPCANMsg message = {};  // CAN message structure for sending the command
    vector<uint8_t> response_data;

    // Prepare the command structure
    message.ID = CMD_READ_MEMORY;  // Example command ID for reading
    message.LEN = 5;
    message.MSGTYPE = PCAN_MESSAGE_STANDARD;

    while (bytes_left > 0) {
        bytes_to_read = (std::min)(MAX_READ_SIZE, bytes_left);  // Handle last chunk

        // Set message payload: start address and number of bytes to read
        message.DATA[0] = (current_address >> 24) & 0xFF;  // Address MSB
        message.DATA[1] = (current_address >> 16) & 0xFF;
        message.DATA[2] = (current_address >> 8) & 0xFF;
        message.DATA[3] = current_address & 0xFF;  // Address LSB
        message.DATA[4] = bytes_to_read - 1;  // Bytes to read - 1 (as per protocol)

        // **Step 1: Send Request Message (ID: 0x11)**
        if (!writeToPort(&message)) {
            std::cerr << "Failed to send read command." << std::endl;
            return;
        }
        Sleep(100);

        // **Step 2: Wait for ACK Response**
        if ((receiveAck(1000) != PCAN_ERROR_OK) || (ackstatus == NACK_BYTE)) {
            std::cerr << "Failed to receive ACK for request.\n";
            return;
        }

        // **Step 3: Wait and read the response from the CAN port**
        if ((readresponse(2000) != PCAN_ERROR_OK) || (ackstatus == NACK_BYTE)) {                               // will write to global buffer
            std::cerr << "Error receiving response." << std::endl;
            return;
        }
        Sleep(50);

        // Update address and bytes left for next iteration
        current_address += bytes_to_read;
        bytes_left -= bytes_to_read;
        std::cout << "Memory read complete. Bytes left: " << bytes_left  << "     " << current_address << std::endl;
    }
}

void CustomCANPort::write_fw(const std::vector<uint8_t>& source_data, uint32_t start_address) {
    const size_t MAX_WRITE_SIZE = 248;  // Max chunk size for each block write
    const size_t CAN_DATA_SIZE = 8;     // Max data size per CAN message

    size_t totalSize = source_data.size();                  //Total size of fw file
    size_t bytesSent = 0;                                   // Total bytes sent in the current chunk of 248bytes
    size_t currentChunk = 0;                                // Number of chunk sent
    size_t noOfChunksToSend = (totalSize / MAX_WRITE_SIZE) + (totalSize % MAX_WRITE_SIZE != 0); //total no of chunks to be sent 

    TPCANMsg reqMessage = {}, dataMessage = {};
    std::cout << "\r\n Writing FW now \r\n";
    while (currentChunk < noOfChunksToSend) {
        size_t bytesToWrite = (std::min)(MAX_WRITE_SIZE, totalSize - bytesSent);

        // **Step 1: Send Request Message (ID: 0x31)**
        reqMessage.ID = CMD_WRITE_MEMORY;  
        reqMessage.MSGTYPE = PCAN_MESSAGE_STANDARD;
        reqMessage.LEN = 5;
        reqMessage.DATA[0] = (start_address >> 24) & 0xFF;
        reqMessage.DATA[1] = (start_address >> 16) & 0xFF;
        reqMessage.DATA[2] = (start_address >> 8) & 0xFF;
        reqMessage.DATA[3] = start_address & 0xFF;
        reqMessage.DATA[4] = bytesToWrite - 1;

        if (!writeToPort(&reqMessage)) {
            std::cerr << "Error sending request message\n";
            return;
        }

        // **Step 2: Wait for ACK Response**
        if ((receiveAck(1000) != PCAN_ERROR_OK) || (ackstatus == NACK_BYTE))  {
            std::cerr << "Failed to receive ACK for request.\n";
            return;
        }

        // **Step 3: Send Data Messages (ID: 0x04)**
        for (size_t i = 0; i < bytesToWrite; i += CAN_DATA_SIZE) {
            size_t chunkSize = (std::min)(CAN_DATA_SIZE, bytesToWrite - i);

            dataMessage.ID = CMD_WRITE_DATA;
            dataMessage.MSGTYPE = PCAN_MESSAGE_STANDARD;
            dataMessage.LEN = chunkSize;
            memcpy(dataMessage.DATA, source_data.data() + bytesSent + i, chunkSize);

            if (!writeToPort(&dataMessage)) {
                std::cerr << "Error sending data message\n";
                return;
            }

            // **Step 4: Wait for ACK after each Data Message**
            if ((receiveAck(1000) != PCAN_ERROR_OK) || (ackstatus == NACK_BYTE))  {
                std::cerr << "No second ACK received.\n";
                return;
            }
        }

        if ((receiveAck(1000) != PCAN_ERROR_OK) || (ackstatus == NACK_BYTE))  {
            std::cerr << "No second ACK received.\n";
            return;
        }

        // **Step 5: Update Start Address and Bytes Sent**
        bytesSent += bytesToWrite;
        start_address += bytesToWrite;  // Increment start address by the number of bytes written

        currentChunk++;  // Move to the next chunk
    }
}


void CustomCANPort::print_req(TPCANMsg *msg)    {
    cout << "\n ==> Message txd:";
    cout << "ID: " << std::hex << msg->ID << "\tData: " << std::setw(2) << std::setfill('0');
    for (auto i = 0; i <= 7; i++)
    {
        cout << " " << std::hex << static_cast<int>(msg->DATA[i]); 
    }
    cout << "\n";
}

void CustomCANPort::send_command_to_BL(uint32_t command_code)
{
    int ret_value=0;
    vector<uint8_t> file_data;
    uint32_t filesize = 0;
    TPCANMsg message = {};
    uint32_t start_address = 0x08000000;
    uint32_t total_bytes = 0;

    message.LEN = 1;
    message.MSGTYPE = PCAN_MESSAGE_STANDARD;

    switch(command_code)
    {
    case '0':
        cout << "\n   Command == > BL_SYNC";
        message.DATA[0] = 1;
        message.ID = ACK_BYTE;
        writeToPort(&message);
        Sleep(100);
        readresponse(1000);
        break;

    case '1':
        cout << "\n   Command == > BL_GET";
        message.DATA[0] = 1;
        message.ID = CMD_GET_COMMAND;
        writeToPort(&message);
        Sleep(100);
        readresponse(1000);
        break;

    case '2':
        cout << "\n   Command == > BL_GET_ID";
        message.DATA[0] = 1;
        message.ID = CMD_GET_ID;
        writeToPort(&message);
        Sleep(100);
        receiveAck(100);
        readresponse(1000);
        break;

    case '3':
        cout << "\n   Command == > BL_READ_MEM";
        fileobj.open_file(USER_APP);
        total_bytes = fileobj.calc_file_len(USER_APP);
        read_from_memory(start_address, total_bytes);
        fileobj.close_the_file();
        break;

    case '4':
        cout << "\n   Command == > BL_ERASE_MEM";
        message.ID = CMD_ERASE_MEMORY;
        message.LEN = 0x01;
        message.DATA[0] = 0xff;
        writeToPort(&message);
        Sleep(100);
        readresponse(1000);
        break;

    case '5':
        cout << "\n   Command == > BL_WRITE_MEM";
        // filesize = fileobj.calc_file_len();
        // cout << "\n   File size is " << std::dec << filesize << endl;

        fileobj.open_file(USER_APP);
        filesize = fileobj.calc_file_len(USER_APP);
        fileobj.read_from_position(file_data, 0, filesize);
        
        write_fw(file_data, start_address);

        //std::cout << "\n";
        // for (size_t i = 0; i < file_data.size(); ++i) {
        //     // Print each byte in hexadecimal format
        //     std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(file_data[i]) << " ";
        
        //     // Print a newline every 16 bytes for better readability
        //     if ((i + 1) % 16 == 0) {
        //         std::cout << "\n";
        //     }
        // }

        fileobj.close_the_file();
        break;

    case '6':
        cout << "\n   Command == > BL_GO_TO_ADDRESS";
        message.ID = CMD_GO;
        message.LEN = 0x04;
        message.DATA[0] = (start_address >> 24) & 0xFF;
        message.DATA[1] = (start_address >> 16) & 0xFF;
        message.DATA[2] = (start_address >> 8) & 0xFF;
        message.DATA[3] = start_address & 0xFF;
        writeToPort(&message);
        Sleep(100);
        receiveAck(1000);

        break;

    case '7':
        cout << "\n   Command == > Flush CAN buffer";
        flushbuffer(2000);
        break;    

    case '8':
        cout << "\n   Command == > Compare Files";
        fileobj.compare_files(USER_APP, OUTPUT_FILE);
        break;     
    default:
        break;
    }
}