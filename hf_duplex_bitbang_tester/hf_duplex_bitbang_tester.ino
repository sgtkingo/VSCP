/**
 * Bit-bang 1-wire UART přes OPEN-DRAIN GPIO (ESP32).
 * - MASTER/SLAVE společný kód (přepíná se makrem)
 * - Protokol: "?TEXT\n"
 * - Bus = 1 vodič přes 1k na každý ESP32 + externí pull-up 4k7–10k
 *
 * Fyzika:
 *   digitalWrite(LOW)  -> stáhne linku k zemi (dominantní 0)
 *   digitalWrite(HIGH) -> uvolní linku, stáhne ji pull-up (recesivní 1)
 *
 * HW:
 *   ESP1 GPIO4 ---1k---+
 *                      +--- sběrnice --- pull-up 4k7–10k na 3V3
 *   ESP2 GPIO4 ---1k---+
 *   společná GND
 */

#include <Arduino.h>

// ------------------------ ROLE ------------------------
#define ROLE_MASTER 1
#define ROLE_SLAVE  2

#define NODE_ROLE ROLE_MASTER     // <- TADY přepni na ROLE_SLAVE pro SLAVE
//#define NODE_ROLE ROLE_SLAVE

// ------------------------ BUS -------------------------
#define BUS_PIN 4

// Soft-UART parametry (bit-bang)
#define BB_BAUD_RATE 9600UL
#define BIT_US (1000000UL / BB_BAUD_RATE)

// Master perioda dotazu
#define MASTER_REQUEST_PERIOD_MS 1000UL

// ======================================================
// 1) NÍZKOÚROVŇOVÉ FUNKCE – OPEN-DRAIN LOGIKA
// ======================================================

// Uvolnění sběrnice (log. 1)
inline void busRelease() {
    digitalWrite(BUS_PIN, HIGH);   // v open-drain = HIGH = "nechávám být"
    delay(1);                  // krátká pauza pro ustálení
}

// Stažení sběrnice (log. 0)
inline void busPullLow() {
    digitalWrite(BUS_PIN, LOW);
    delay(1);                  // krátká pauza pro ustálení
}

// Čtení úrovně na sběrnici
inline bool busRead() {
    busRelease();                  // zajistí, že čteme uvolněný stav
    return (digitalRead(BUS_PIN) == HIGH);
}

// ======================================================
// 2) BIT-BANG SEND/RECV
// ======================================================

// Odešle 1 bit
void sendBit(bool bit) {
    if (bit) busRelease();
    else     busPullLow();
    delayMicroseconds(BIT_US);
}

// Odešle 1 byte – formát 8N1
void sendByte(uint8_t b) {
    // Start bit = 0
    sendBit(0);

    // 8 datových bitů (LSB první)
    for (uint8_t i = 0; i < 8; i++) {
        sendBit((b >> i) & 0x01);
    }

    // Stop bit = 1
    sendBit(1);
}

// Přijme 1 byte (blokuje max timeout)
bool recvByte(uint8_t &out, uint32_t timeoutMs) {
    out = 0;

    // Čekáme na start bit (pád z 1 na 0)
    uint32_t t0 = millis();
    while (busRead()) {
        if (millis() - t0 > timeoutMs) return false;
        delayMicroseconds(10);
    }

    // Jsme v start bitu → čekáme 1.5 bitu doprostřed prvního datového bitu
    delayMicroseconds(BIT_US + BIT_US/2);

    // Čteme 8 datových bitů
    for (uint8_t i = 0; i < 8; i++) {
        bool bit = busRead();
        if (bit) out |= (1 << i);
        delayMicroseconds(BIT_US);
    }

    // Stop bit (ignorujeme hodnotu, jen počkáme)
    delayMicroseconds(BIT_US);

    return true;
}

// ======================================================
// 3) VYSOKOÚROVŇOVÝ PROTOKOL – "?TEXT\n"
// ======================================================

// Odešle celou zprávu
void sendMessage(const String &payload) {
    String frame = "?" + payload + "\n";

    Serial.print("[BB TX] ");
    Serial.print(frame);

    for (size_t i = 0; i < frame.length(); i++) {
        sendByte((uint8_t)frame[i]);
    }

    busRelease();
}

// Přijme zprávu ve formátu "?XXXX\n" → out="XXXX"
bool receiveMessage(String &out, uint32_t timeoutMs) {
    out = "";
    bool started = false;

    uint32_t t0 = millis();
    while (millis() - t0 < timeoutMs) {

        uint8_t b;
        if (!recvByte(b, timeoutMs)) continue;

        char c = (char)b;

        if (!started) {
            if (c == '?') {
                started = true;
            }
        } else {
            if (c == '\n') return true;
            out += c;
        }
    }

    return false;
}

// ======================================================
// 4) SETUP
// ======================================================

void setup() {
    Serial.begin(115200);
    delay(200);

    // OPEN-DRAIN režim
    pinMode(BUS_PIN, OUTPUT_OPEN_DRAIN);
    busRelease();

    Serial.println("\n========================================");
#if NODE_ROLE == ROLE_MASTER
    Serial.println("ROLE = MASTER");
#else
    Serial.println("ROLE = SLAVE");
#endif
    Serial.println("BIT-BANG 1-WIRE UART, OPEN-DRAIN");
    Serial.print("GPIO = "); Serial.println(BUS_PIN);
    Serial.print("BAUD = "); Serial.println(BB_BAUD_RATE);
    Serial.println("========================================\n");
}

// ======================================================
// 5) MAIN LOOP – MASTER / SLAVE
// ======================================================

void loop() {

#if NODE_ROLE == ROLE_MASTER
    // ------------------- MASTER -------------------
    static uint32_t last = 0;

    if (millis() - last > MASTER_REQUEST_PERIOD_MS) {

        String req = "STATUS";
        Serial.print("[MASTER] Sending: ");
        Serial.println(req);
        sendMessage(req);

        // MASTER čeká na odpověď
        String resp;
        if (receiveMessage(resp, 500)) {
            Serial.print("[MASTER] Response: ");
            Serial.println(resp);
        } else {
            Serial.println("[MASTER] Timeout");
        }

        last = millis();
    }

#else
    // -------------------- SLAVE --------------------
    String req;

    if (receiveMessage(req, 200)) {

        Serial.print("[SLAVE] Received: ");
        Serial.println(req);

        if (req == "STATUS") {

            String resp = "ONLINE_OK";
            delay(5);               // krátká pauza, aby MASTER přešel do RX

            Serial.print("[SLAVE] Sending: ");
            Serial.println(resp);
            sendMessage(resp);
        }
    }

#endif

    delay(5);
}