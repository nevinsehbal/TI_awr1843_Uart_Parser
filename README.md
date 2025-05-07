# TI_awr1843_Uart_Stream_Parser

Since TI AWR1843Boost documentation is not fully complete, these AWR1843 UART Signal Decoder codes are provided for developers. The signal carries information based on "Medium Range Radar Sensor" configuration, but may be used for other configurations with some parameter alterations.

Detailed info about the UART Packet is provided at the AWR1843.hpp file
Make sure that your sensor setup is successful (compatible sdk versions w/ your configuration etc.)
Then, when you execute the main.cpp file, meaningful data stream outputs on the terminal.

Compile command:

```bash
g++ -std=c++17 -Wall -Wextra AWR1843.cpp main.cpp -o awr1843_parser
```

For further help on the setup of the device, contact me: nssoyuslu@gmail.com
