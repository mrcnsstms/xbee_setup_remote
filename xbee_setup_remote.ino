#include <SoftwareSerial.h>

#define WAKE_PIN     3
#define printSerial ss
#define xbeeSerial Serial

SoftwareSerial ss(7,8);

// destination address
#define ADDR1 0x00
#define ADDR2 0x13
#define ADDR3 0xA2
#define ADDR4 0x00
#define ADDR5 0x41
#define ADDR6 0x25
#define ADDR7 0xA4
#define ADDR8 0x79

// 0x7E SOM
// 0x00 length MSB
// 0x13 length LSB (excludes first 3 bytes and checksum)
uint8_t tx_array[] = {0x7E, 0x00, 0x13, 
                      0x10, 0x00, ADDR1, ADDR2, ADDR3, ADDR4, ADDR5, ADDR6, ADDR7, ADDR8, 
                      0xFF, 0xFE, 0x00, 0x00, '1', '2', '3', '4', '5', 
                      0xD1}; // checksum

void setup() 
{
  Serial.begin(19200);
  ss.begin(19200);
  pinMode(WAKE_PIN, INPUT);

  // payload
  tx_array[17] = 127;
  uint16_t sensor = 500;
  tx_array[19] = sensor & 0xFF; // low
  tx_array[20] = sensor >> 8;   // high
  tx_array[21] = '!';

  // calculate checksum
  uint8_t cs = Get_checksum(tx_array, sizeof(tx_array));
  tx_array[sizeof(tx_array) - 1] = cs;
}

void loop() 
{
  bool wake = digitalRead(WAKE_PIN);
  if(wake)
  {  
    for(int i = 0; i < sizeof(tx_array); i++)
    {
      printSerial.print(tx_array[i], HEX);
      printSerial.print(",");
    }
    printSerial.println();
    delay(1000);
    Serial.write(tx_array, sizeof(tx_array));
    delay(3000);
  }
}

uint8_t Get_checksum(const uint8_t frame[], const uint8_t length)
{
  long sum = 0;
  for(int i = 3; i < (length - 1); i++){
    sum += frame[i];
  }
  uint8_t check_sum = 0xFF - (sum & 0xFF);
  return check_sum;
}
