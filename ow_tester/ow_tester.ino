#include <OneWire.h>

// --- KONFIGURACE ---
#define ONE_WIRE_BUS 4 // GPIO4 pro OneWire sbernici
#define BAUD_RATE 115200

// ROLE ZARIZENI: Odkomentujte POUZE jednu roli pro kazdou jednotku.
//#define DEVICE_ROLE ROLE_MASTER
#define DEVICE_ROLE ROLE_SLAVE 

#define ROLE_MASTER 1
#define ROLE_SLAVE  2
// -------------------

OneWire oneWire(ONE_WIRE_BUS); 

// --- SLAVE STATUS & ADRESA ---
// Kdyz pouzivate dve zarizeni 1:1, neni adresa kriticka, 
// ale pro formalni protokol ji muze mit Slave definovanou.
const byte SLAVE_ADDRESS[] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}; 
// Tato fiktivni adresa se v tomto prikladu nepouziva, je pouze ilustrativni.
// -----------------------------


// --- FUNKCE PRO ZAPIS ---
// Master/Slave komunikuji vzdy jen po dotazu (Master -> Slave)
void sendMessage(String message) {
  // Reset sbernici provadi Master
  if (DEVICE_ROLE == ROLE_MASTER) {
    if (oneWire.reset()) {
      oneWire.write(0xCC); // Skip ROM command (pro zjednoduseni 1:1 komunikace)
      delay(1); // Cekani
      // Odeslani delky
      byte msgLen = message.length();
      oneWire.write(msgLen);
      
      // Odeslani zprávy
      for (int i = 0; i < msgLen; i++) {
        oneWire.write(message.charAt(i));
      }
      
      Serial.print("[MASTER SEND] ");
      Serial.println(message);
    } else {
      Serial.println("[MASTER ERROR] OneWire reset failed (Slave not present?)");
    }
  }
}

// --- FUNKCE PRO CTENI ---
// Master ocekava, ze po dotazu odpovi Slave
String receiveMessage() {
  String received = "";
  
  if (DEVICE_ROLE == ROLE_MASTER) {
    // Master ceka na odpoved po dotazu.
    // Pro Half-Duplex se vyuziva reset a nasledne cteni.
    if (oneWire.reset()) {
      oneWire.write(0xCC); // Skip ROM
      
      // Cteni delky
      byte msgLen = oneWire.read();
      
      if (msgLen > 0 && msgLen < 128) { 
        // Cteni zprávy
        for (int i = 0; i < msgLen; i++) {
          char c = oneWire.read();
          received += c;
        }
        
        Serial.print("[MASTER RECV] ");
        Serial.println(received);
      }
    }
  }
  return received;
}


// --- INICIALIZACE ---
void setup() {
  Serial.begin(BAUD_RATE);
  
  // Aktivace interniho PULL-UPu
  pinMode(ONE_WIRE_BUS, INPUT_PULLUP); // Aktivuje interni pull-up
  
  // Inicializace OneWire
  oneWire.reset_search();
  Serial.printf("OneWire Initialized on GPIO%d. Role: %s\n", 
                ONE_WIRE_BUS, 
                (DEVICE_ROLE == ROLE_MASTER ? "MASTER" : "SLAVE"));
  
  delay(1000); 
  
  if (DEVICE_ROLE == ROLE_MASTER) {
    sendMessage("Master is starting up.");
  } else {
    Serial.println("Slave is ready to listen for Master's request.");
  }
}


// --- HLAVNI SMYCKA ---
void loop() {
  if (DEVICE_ROLE == ROLE_MASTER) {
    // ----------------------------------------------------
    // MASTER ARBITRAZ: Master RIDI komunikaci
    // ----------------------------------------------------
    static unsigned long lastRequest = 0;
    
    if (millis() - lastRequest > 2000) { // Dotaz kazde 2 sekundy
      Serial.println("\n[MASTER] Sending STATUS request...");
      
      // 1. Master odesle dotaz na STATUS
      sendMessage("STATUS");
      
      // 2. Master ocekava odpoved (zde je potreba casova synchronizace)
      // V realnem kodu byste zde cekal kratsi dobu a pak cist data.
      delay(50); 
      
      String response = receiveMessage();
      
      if (response.length() > 0 && response.startsWith("ACK: STATUS")) {
        Serial.println("[MASTER] Slave ACK received!");
      }
      
      lastRequest = millis();
    }
    delay(50); // Master loops delay
  } else { 
    // ----------------------------------------------------
    // SLAVE ARBITRAZ: Slave Ceka na reset (dotaz) Mastera
    // ----------------------------------------------------
    
    // Testuje, zda Master prave neresetoval sbernici (coz znaci dotaz)
    if (oneWire.reset()) {
      // Precti prikaz (zde budeme pro zjednoduseni ocekavat jen 0xCC + delka + data)
      byte command = oneWire.read(); 
      
      if (command == 0xCC) { // Skip ROM
        
        // Precteni délky zprávy (jednoduche, ale funguje)
        byte msgLen = oneWire.read();
        
        if (msgLen > 0 && msgLen < 128) {
          String incomingMessage = "";
          for (int i = 0; i < msgLen; i++) {
            incomingMessage += (char)oneWire.read();
          }
          
          Serial.print("[SLAVE RECV] Command: ");
          Serial.println(incomingMessage);
          
          // Okamzita odpoved Slave
          if (incomingMessage.startsWith("STATUS")) {
            // Zde by mela Slave jednotka zmenit smer pinu na TX a poslat odpoved
            // Knihovna OneWire toto zajistuje pres OneWire.write() po resetu a cteni.
            oneWire.reset(); 
            oneWire.write(0xCC); // Opet Skip ROM
            delay(1); 
            
            String response = "Slave OK";
            oneWire.write(response.length());
            for (int i = 0; i < response.length(); i++) {
              oneWire.write(response.charAt(i));
            }
            Serial.println("[SLAVE SEND] Slave OK");
          }
        }
        else {
          Serial.println("[SLAVE ERROR] Invalid message length received: " + String(msgLen));
      }
    }
    delay(1); // Slave loops delay
  }
}
}