#include <SPI.h>
#include <MFRC522.h>
#include <VirtualWire.h>

#define RFID_SS_PIN 10  // RFID module slave select pin
#define RF_TX_PIN 12    // RF transmitter pin
#define RF_RX_PIN 11    // RF receiver pin
#define SIGNAL_PIN 9    // Pin to control signal lights

MFRC522 mfrc522(RFID_SS_PIN, 9);  // Create MFRC522 instance

void setup() {
  Serial.begin(9600);
  mfrc522.PCD_Init();  // Initialize RFID module
  vw_set_tx_pin(RF_TX_PIN);  // Set RF transmitter pin
  vw_set_rx_pin(RF_RX_PIN);  // Set RF receiver pin
  vw_setup(2000);  // Bits per second
  pinMode(SIGNAL_PIN, OUTPUT);  // Set signal pin as output
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // RFID card detected, read its UID
    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; ++i) {
      uid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      uid += String(mfrc522.uid.uidByte[i], HEX);
    }

    // Send UID via RF communication
    char msg[uid.length()];
    uid.toCharArray(msg, uid.length() + 1);
    vw_send((uint8_t *)msg, strlen(msg));
    vw_wait_tx();  // Wait until the whole message is gone

    // Actuate signal based on received command
    // Example: Toggle the signal light
    digitalWrite(SIGNAL_PIN, !digitalRead(SIGNAL_PIN));

    delay(1000);  // Delay to avoid multiple reads
  }
}
