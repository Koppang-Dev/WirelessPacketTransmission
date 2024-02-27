# Wireless Packet Transmisson

The program establishes a communication protocol with other Arduino devices where they send 'hello' packets to discover and maintain information about neighbouring devices. It ensures robustness by periodically checking for inactive neighbours and updating neighbour information accordingly. The discovery signals are sent over WIFI using the RF24 library. The sent packets include a unique identifier and the list of that device's first-hop neighbours. 

# Built With
 
* [Ardruino](https://www.arduino.cc) - The hardware used
* Arduino uses a variant of C++/C language

# Installation
i. Install Arduino IDE: Download and install the Arduino IDE from the official website.
ii. Install RF24 Library: Install the RF24 library in the Arduino IDE. You can do this by navigating to Sketch -> Include Library -> Manage Libraries, then search for "RF24" and install it.
iii. Download and Upload Code: Download the source code from this repository. Open the downloaded sketch in Arduino IDE and upload it to your Arduino device.


# Usage
i. Hardware Setup: Connect the NRF24L01(+) transceiver module to your Arduino board according to the wiring specified in the source code or your hardware configuration.
ii. Compile and Upload: Compile and upload the provided sketch to your Arduino board.
iii. Monitor Serial Output: Open the serial monitor in the Arduino IDE to view debug messages and neighbor information transmitted and received by the device.

# License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

• Two other group members contributed
• This was for a class project
