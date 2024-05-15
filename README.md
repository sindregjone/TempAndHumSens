## Introduction
This project is created for the ESP32-C3 microcontroller. It is built upon the esp-idf BluFi example project, but it has been edited. 

## File structure
- `main.c`: This is where the app_main function is located, where the program starts
- `definitions.h`: Header file containing necessary definitions and macros used in the project
- `battery_monitor.c`: Source file that handles battery monitoring functionality.
- `battery_monitor.h`: Header file for `battery_monitor.c`, defining functions and variables related to battery monitoring.
- `BT_Handler.c`: Source file for handling Bluetooth communication.
- `BT_Handler.h`: Header file for `BT_Handler.c`, defining functions and variables related to Bluetooth communication.
- `NVS_Handler.c`: Source file for handling Non-Volatile Storage (NVS) operations.
- `NVS_Handler.h`: Header file for `NVS_Handler.c`, defining functions and variables related to NVS operations.
- `blufi_init.c`: Source file for initializing Bluetooth Low Energy (BLE) functionalities using Blufi.
- `blufi_security.c`: Source file that handles security aspects of the Blufi protocol.
- `blufi.h`: Header file defining functions and variables used in `blufi_init.c` and `blufi_security.c`.
- `http_client.c`: Source file for managing HTTP client operations.
- `http_client.h`: Header file for `http_client.c`, defining functions and variables related to HTTP client operations.
- `SHTC3.c`: Source file for interfacing with the SHTC3 sensor, which measures temperature and humidity.
- `SHTC3.h`: Header file for `SHTC3.c`, defining functions and variables related to the SHTC3 sensor.
- `wifi_client.c`: Source file for managing Wi-Fi client operations.
- `wifi_client.h`: Header file for `wifi_client.c`, defining functions and variables related to Wi-Fi client operations.
- `Kconfig.projbuild`: Configuration file for the project used to define build settings
- `CMakeLists.txt`: Here the components is registered

## Usage
Once the project is flashed to the ESP32, it will start running automatically. 
For changing variables as the server URL, the wakeupTime, the time it is in bluetooth mode, battery level update time or any GPIO ports, you must use the `definitions.h`file 
