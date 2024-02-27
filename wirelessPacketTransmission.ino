// Team 7 - Riley Koppang, Iohane Lowry, Tofunmi Ojo
#include <RF24.h>

RF24 radio(7, 8);                       // Create the radio object with (CE, CSN)
const byte addr[6] = "bcast";           // Address for radio communication
unsigned char hello[32];                // Hello Packet to be transmitted
const unsigned long nTimeLimit = 6000;  // Time limit to consider a neighbor inactive
unsigned long lastWrite = 0;            // Timestamp of the last transmission
unsigned long lastPrint = 0;            // Timestamp of the last neighbor print
unsigned long lastCheck = 0;
unsigned char buffer[32];  // Receive Buffer for incoming packets
bool exists;               // Flag to check if a neighbor exists
int numberOfNeighbours;    // Number of known neighbors
int waitTime = 0;          // Initialize waitTime with 0 (used for random wait times)

// Struct to store neighbor information
struct NeighbourInfo {
  byte address;  // Neighbor's address
  byte twoHop[15];
  unsigned long lastSeen;  // Timestamp of the last time a packet was received from the neighbor
};

NeighbourInfo neighbours[20];  // Array to store neighbor information

void setup() {
  Serial.begin(9600);
  radioSetup();
}

// Initialize radio settings and the Hello Packet
void radioSetup() {
  if (radio.begin()) {
    radio.setAutoAck(false);              // Disable auto acknowledgment of received packets
    Serial.println("Radio Hardware OK");  // Print message indicating radio hardware is functional
    radio.openWritingPipe(addr);          // Set the address for transmitting data
    radio.openReadingPipe(0, addr);       // Set the address for reading data
    radio.setPALevel(RF24_PA_MAX);        // Set the power amplification level of the radio module to maximum
    hello[0] = 0x50;                      // Set the type for the Hello Packet
    hello[1] = 0x7;                       // Set the team address
  } else {
    Serial.println("Radio Hardware Not Responding");  // Print message indicating radio hardware is not responsive
  }
}

// Receiving Hello Packets From Other Groups
void listening() {
  radio.startListening();                 // Start listening for incoming packets
  if (radio.available()) {                // Check if there's a packet available
    Serial.println("Received Packet");    // Print a message indicating the receipt of a packet
    radio.read(&buffer, sizeof(buffer));  // Read the packet into the buffer
    exists = false;                       // Initialize the flag to check if a neighbor exists
    if (buffer[0] >> 4 != 5) {
      Serial.println("Disregarding Non-Hello Packet");  // Print a message indicating a non-Hello Packet is being disregarded
    } else {
      hello[2] = numberOfNeighbours;
      for (int i = 0; i < numberOfNeighbours; i++) {  // Iterate through known neighbors
        hello[3 + i] = neighbours[i].address;

        if (neighbours[i].address == buffer[1]) {  // Check if the neighbor's address matches the received address
          exists = true;                           // Set the flag to indicate the neighbor exists
          neighbours[i].lastSeen = millis();       // Update the lastSeen timestamp for the neighbor
          if (buffer[2] > 0) {
            for (int j = 0; j <= buffer[2]; j++) {
              neighbours[i].twoHop[j] = buffer[3 + j];
              //Serial.println(buffer[2+j]); //test output to make sure im getting the other's neighbours
            }
          }
          break;  // Exit the loop since the neighbor has been found
        }
      }
      if (!exists) {
        neighbours[numberOfNeighbours].address = buffer[1];  // Store the address of a new neighbor
        neighbours[numberOfNeighbours].lastSeen = millis();  // Store the timestamp for the new neighbor
        hello[2 + numberOfNeighbours] = buffer[1];
        if (buffer[2] > 0) {
          for (int j = 0; j <= buffer[2]; j++) {
            neighbours[numberOfNeighbours].twoHop[j] = buffer[3 + j];
          }
        }
        hello[2] = numberOfNeighbours;
        numberOfNeighbours++;  // Increment the count of known neighbors
      }
    }
  }
}

// Sending the hello packet
void writing() {
  radio.stopListening();  // Stop listening to prepare for transmission
  if (radio.write(hello, sizeof(hello))) {
    // Successful transmission, no action needed
  } else {
    Serial.println("Transmission not acknowledged");  // Print a message if the transmission is not acknowledged
  }
}

// Check and remove inactive neighbors
void checkPacketTime() {
  unsigned long currentTime = millis();                       // Get the current timestamp in milliseconds
  for (int i = 0; i < numberOfNeighbours; i++) {              // Iterate through known neighbors
    if (currentTime - neighbours[i].lastSeen > nTimeLimit) {  // Check if a neighbor has exceeded the time limit
      for (int j = i; j < numberOfNeighbours; j++) {
        neighbours[j] = neighbours[j + 1];  // Shift neighbors to remove the inactive one
      }
      numberOfNeighbours--;  // Decrement the count of known neighbors
    }
  }
}

// Print out the current neighbors
void printNeighbours() {
  if (numberOfNeighbours > 0) {
    Serial.print("Number of current neighbors: ");
    Serial.println(numberOfNeighbours);
    Serial.println("Current Neighbors:");
    for (int i = 0; i < numberOfNeighbours; i++) {
      Serial.print(neighbours[i].address);  // Print the address of each known neighbor
      Serial.print(": One Hop neighbors: ");
      if (neighbours[i].twoHop[0] != 0) {
        for (int j = 0; j < neighbours[i].twoHop; j++) {
          if (neighbours[i].twoHop[j] == 0) {
            break;
          } else {
            Serial.print(neighbours[i].twoHop[j]);
            Serial.print(", ");
          }
        }
        Serial.println();
      } else {
        Serial.println("This node has no neighbours");  // Print a message indicating that there are no known neighbors
      }
    }
  } else {
    Serial.println("No neighbours");  // Print a message indicating that there are no known neighbors
  }
}

void loop() {
  long currentTime = millis();  // Get the current timestamp in milliseconds
  listening();                  // Listen for incoming packets
  if (currentTime - lastWrite >= waitTime) {
    writing();                      // Transmit the Hello Packet
    waitTime = random(1000, 5000);  // Generate a new random wait time
    lastWrite = currentTime;        // Update the last write timestamp
  }
  if (currentTime - lastPrint >= 2000) {  // Print neighbor information every 1000 milliseconds
    printNeighbours();                    // Print the information about known neighbors
    lastPrint = currentTime;              // Update the last print timestamp
  }
  if (currentTime - lastCheck >= 100) {  // Print neighbor information every 1000 milliseconds
    checkPacketTime();                   // Check and remove inactive neighbors
    lastCheck = currentTime;             // Update the last print timestamp
  }
}
