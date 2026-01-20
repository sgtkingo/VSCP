/**
 * Jednoduchý half-duplex protokol přes UART mezi dvěma ESP32 na JEDNOM vodiči.
 * - Textově orientovaný protokol, každá zpráva začíná znakem '?' a končí '\n'
 * - Společný kód pro MASTER i SLAVE, role se vybírá makrem NODE_ROLE
 *
 * HW zapojení (1-drát):
 *   MASTER_TX ---1k---+
 *                      \
 *                       +---- sběrnice ----+---1k--- SLAVE_TX
 *                      /                   |
 *   MASTER_RX ---------+                   +--- SLAVE_RX

 *                                         |
 *                                       5k pull-up na 3V3
 *   společná GND
 */

#include <Arduino.h>
#include <HardwareSerial.h>

// ---------------------- KONFIGURACE ROLE ----------------------

#define ROLE_MASTER 1
#define ROLE_SLAVE  2

//#define NODE_ROLE ROLE_MASTER   // <-- TADY PŘEPÍNEJ MASTER / SLAVE
#define NODE_ROLE ROLE_SLAVE

// ---------------------- KONFIGURACE REŽIMU ----------------------

// ONE_WIRE_MODE = 1 => TX a RX jsou elektricky svázané (echo na RX)
#define ONE_WIRE_MODE 1

// ---------------------- KONFIGURACE UART ----------------------

#define BUS_BAUD_RATE 115200
#define BUS_RX_PIN    16    // RX pin ESP32 pro sběrnici
#define BUS_TX_PIN    17    // TX pin ESP32 pro sběrnici

#define MASTER_REQUEST_PERIOD_MS 1000

HardwareSerial Bus(1);

// ---------------------- PROTOKOL ----------------------

/**
 * Odešle jednu textovou zprávu přes sběrnici.
 * Zpráva je automaticky zabalena do formátu:
 *   "?" + payload + "\n"
 *
 * Např. payload "STATUS" -> po drátu jde "?STATUS\n"
 */
void sendMessage(const String &payload) {
    String frame = "?";
    frame += payload;
    frame += "\n";

    Bus.print(frame);
    Bus.flush();  // počká na fyzické odeslání

    Serial.print("[TX] ");
    Serial.print(frame);  // frame obsahuje '\n'
}

/**
 * Přečte jednu kompletní zprávu z UART sběrnice.
 *
 * Formát:
 *   ?TEXT\n  -> out = "TEXT"
 */
bool receiveMessage(String &out, uint32_t timeoutMs) {
    out = "";
    Bus.setTimeout(timeoutMs);

    // Přečteme až po '\n'. Pokud (ještě) nic nepřišlo, funkce blokuje max timeoutMs.
    String raw = Bus.readStringUntil('\n');
    if (raw.length() == 0) return false;   // timeout / nic nepřišlo

    int idx = raw.indexOf('?');
    if (idx < 0) return false;             // nevalidní rámec

    out = raw.substring(idx + 1);          // vše za '?'
    return true;
}

/**
 * Vyházení echa po právě odeslané zprávě v 1-drát režimu.
 * Master i Slave při vysílání „slyší sami sebe“ -> echo je v RX bufferu.
 * Funkce:
 *   - krátce počká, než se všechny bajty dostanou do RX
 *   - pak vše v RX bufferu vyčistí
 */
void flushEchoAfterSend(uint32_t waitMs = 3) {
#if ONE_WIRE_MODE
    uint32_t start = millis();
    // krátké okno, během kterého průběžně vysypáváme RX (echo)
    while (millis() - start < waitMs) {
        while (Bus.available() > 0) {
            Bus.read();
        }
        // mikro pauza, ať nesedíme v tight loopu
        delay(1);
    }
#endif
}

// ---------------------- SETUP ----------------------

void setup() {
    Serial.begin(115200);
    delay(100);

    Bus.begin(BUS_BAUD_RATE, SERIAL_8N1, BUS_RX_PIN, BUS_TX_PIN);

    Serial.println();
    Serial.println("============================================");
#if NODE_ROLE == ROLE_MASTER
    Serial.println("[NODE] ROLE = MASTER");
#elif NODE_ROLE == ROLE_SLAVE
    Serial.println("[NODE] ROLE = SLAVE");
#else
    Serial.println("[NODE] ROLE = UNKNOWN (CHYBNA HODNOTA NODE_ROLE)");
#endif

#if ONE_WIRE_MODE
    Serial.println("[MODE] ONE-WIRE UART (TX/RX svazane pres odpory)");
#else
    Serial.println("[MODE] CLASSIC 2-WIRE UART (TX<->RX krizene)");
#endif

    Serial.print("[UART] RX pin = ");
    Serial.print(BUS_RX_PIN);
    Serial.print(", TX pin = ");
    Serial.println(BUS_TX_PIN);
    Serial.println("[UART] Bus inicializovan, pripraven ke komunikaci.");
    Serial.println("============================================");
    Serial.println();
}

// ---------------------- LOOP ----------------------

void loop() {

#if NODE_ROLE == ROLE_MASTER

    // ---------- MASTER LOGIKA ----------
    static uint32_t lastRequest = 0;

    if (millis() - lastRequest >= MASTER_REQUEST_PERIOD_MS) {
        String request = "STATUS";

        Serial.print("[MASTER] Sending request: ");
        Serial.println(request);
        sendMessage(request);

#if ONE_WIRE_MODE
        // Vyházej echo, které se masterovi vrátilo přes 1-drát
        flushEchoAfterSend(5);   // ~5 ms je více než dost na odeslaný rámec
#endif

        // Nyní master čeká na odpověď od slave
        String response;
        bool ok = receiveMessage(response, 500);  // timeout 500 ms

        if (ok) {
            Serial.print("[MASTER] Response received: ");
            Serial.println(response);
        } else {
            Serial.println("[MASTER] Timeout waiting for response");
        }

        lastRequest = millis();
    }

    delay(10);  // stabilizace smyčky

#elif NODE_ROLE == ROLE_SLAVE

    // ---------- SLAVE LOGIKA ----------
    String request;
    bool got = receiveMessage(request, 100);   // krátký timeout, loop běží dál

    if (got) {
        Serial.print("[SLAVE] Request received: ");
        Serial.println(request);

        String response;

        if (request == "STATUS") {
            response = "ONLINE_OK";
        } else {
            Serial.println("[SLAVE] Unknown command, no response.");
            delay(5);
            return;
        }

        // Malé zpoždění, aby master stihl vyházet echo po svém vysílání
#if ONE_WIRE_MODE
        delay(10);
#endif

        Serial.print("[SLAVE] Sending response: ");
        Serial.println(response);
        sendMessage(response);
#if ONE_WIRE_MODE
        // Slave si může také vyházet svoje echo, čistě pro pořádek
        flushEchoAfterSend(3);
#endif
    }

    delay(5);

#else

    Serial.println("[ERROR] NODE_ROLE is not ROLE_MASTER or ROLE_SLAVE!");
    delay(1000);

#endif
}
