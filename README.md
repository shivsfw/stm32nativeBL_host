# STM32 Bootloader Communication over CAN Bus

This Windows-based application communicates with the STM32 native bootloader over the CAN bus, allowing developers to manage firmware operations through console commands. The application leverages the **PCAN Basic** library to interface with the STM32 controller’s bootloader, providing essential control features and the ability to handle CAN messages with specific timeouts.

---

## Requirements

- **PCAN Basic library** - Required to interface the CAN channel with the STM32 controller. The library can be downloaded from [PCAN Basic Library by Peak-System](https://www.peak-system.com/PCAN-Basic.239.0.html?&L=1).
- **STM32 Bootloader CAN Protocol** - This application is designed based on STM32’s native bootloader protocol, which uses CAN for communication. Detailed documentation can be found in [STM32 Bootloader CAN Protocol (AN3154)](https://www.st.com/resource/en/application_note/an3154-can-protocol-used-in-the-stm32-bootloader-stmicroelectronics.pdf).

---

## Application Setup

1. **Connect Hardware**
   - Connect the STM32 controller to the specified CAN channel on your PC using a **PCAN adapter**.
   - Attach the STM32 controller's **virtual COM port** to your PC via a USB A-to-B cable (for user command-line input).

2. **Run the Application**
   - Start the application in your Windows environment.
   - In the console, press **'h'** to display available commands.

---

## Application Capabilities

This application provides an interface to interact with the STM32 bootloader over CAN, with the following capabilities:

- **Open/Close CAN Port**: Enable or disable CAN communication with the STM32 bootloader.
- **Send/Receive CAN Messages**: Issue commands to the STM32 bootloader and retrieve responses within specified timeouts.
- **Console Command Support**: Access various commands through the console interface, making the application versatile and extensible.

*Future Extensions*: This application can be further adapted to use **UART** for bootloader communication if needed, allowing for flexible interfacing options depending on the requirements.

---

## Supported Commands

Once the application is running, you can press **'h'** to view a list of supported commands. The commands allow you to perform actions such as opening and closing the CAN port, sending commands to the bootloader, and receiving responses.

---

## Dependencies

Ensure you have the following installed:

- **PCAN Basic Library**: Download from [Peak-System's official website](https://www.peak-system.com/PCAN-Basic.239.0.html?&L=1).

---

## Additional Notes

- For full details on the CAN protocol used in STM32’s bootloader, refer to [ST’s AN3154 application note](https://www.st.com/resource/en/application_note/an3154-can-protocol-used-in-the-stm32-bootloader-stmicroelectronics.pdf).
- It’s recommended to verify the CAN and virtual COM connections before starting the application to ensure stable communication.

---

## Troubleshooting

- **CAN Channel Not Recognized**: Confirm that the PCAN adapter is connected properly and the correct drivers are installed.
- **Timeout Errors**: Adjust the timeout values if communication with the bootloader is inconsistent, ensuring adequate response times.

For further issues or questions, please refer to the documentation in the links provided or contact the maintainers.
