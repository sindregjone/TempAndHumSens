## Introduction
This project is created for the ESP32-C3 microcontroller. It is built upon the esp-idf BluFi example project, but it has been edited. 

## File structure
- main.c: This is where the app_main function is located, where the program starts
- definitions.h: Header file containing necessary definitions and macros used in the project
- Kconfig.projbuild: Configuration file for the project used to define build settings
- CMakeLists.txt: Here the components is registered
- battery_monitor.c`: Source file that handles battery monitoring functionality.
- `BT_Handler.c`: Source file for handling Bluetooth communication.
- `NVS_Handler.c`: Source file for handling Non-Volatile Storage (NVS) operations.
- `blufi_init.c`: Source file for initializing Bluetooth Low Energy (BLE) functionalities using Blufi.
- `blufi_security.c`: Source file that handles security aspects of the Blufi protocol.
- `http_client.c`: Source file for managing HTTP client operations.
- `SHTC3.c`: Source file for interfacing with the SHTC3 sensor, which measures temperature and humidity.
