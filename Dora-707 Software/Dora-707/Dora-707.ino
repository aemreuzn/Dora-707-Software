#include <Arduino.h>
#include "config.h"
#include "globals.h"
#include "motor.h"
#include "pid.h"
#include "guvenlik.h"
#include "komut.h"

char seriBuffer[64];
byte bufferIndeks = 0;
bool satirTasti = false;

void setup() {
    pinMode(ATIS_PIN, OUTPUT);
    digitalWrite(ATIS_PIN, ATIS_PASIF_SEVIYE);

    Serial.begin(115200);

    pinMode(eksenX.dirPin, OUTPUT); pinMode(eksenX.pulPin, OUTPUT);
    pinMode(eksenY.dirPin, OUTPUT); pinMode(eksenY.pulPin, OUTPUT);
    digitalWrite(eksenX.pulPin, LOW); digitalWrite(eksenY.pulPin, LOW);

    dinamikMerkezGuncelle();
    sonVeriZamani = millis();
    sonJoyMs = millis();
    sonPidUs = micros();

    Serial.println("CELIK KUBBE v2 hazir. MOD: MANUEL, ARM: HAYIR. Komutlar icin: YARDIM");
}

void loop() {
    while (Serial.available() > 0) {
        char ch = Serial.read();
        if (ch == '\n' || ch == '\r') {
            if (satirTasti) {
                Serial.println("HATA: Komut cok uzun, yok sayildi.");
                satirTasti = false; bufferIndeks = 0;
            } else if (bufferIndeks > 0) {
                seriBuffer[bufferIndeks] = '\0';
                komutIsle(seriBuffer);
                bufferIndeks = 0;
            }
        } else if (bufferIndeks < sizeof(seriBuffer) - 1) {
            seriBuffer[bufferIndeks++] = ch;
        } else {
            satirTasti = true;
        }
    }

    if (mod == MOD_OTONOM && !hedefVerisiTaze()) { guvenliDurus("hedef verisi kesildi"); return; }
    if (mod == MOD_OTONOM && OTONOM_OPERATOR_HEARTBEAT && !manuelBaglantiTaze()) { guvenliDurus("JOY kesildi"); return; }
    if (mod == MOD_MANUEL && !manuelBaglantiTaze()) { guvenliDurus("JOY kesildi"); return; }

    atisGuncelle();

    unsigned long simdiUs = micros();
    if (simdiUs - sonPidUs >= PID_PERIYOT_US) {
        float dt = min((float)(simdiUs - sonPidUs) / 1e6f, 0.01f);
        sonPidUs = simdiUs;

        if (mod == MOD_MANUEL) {
            eksenX.hedefGuc = joyX * (MANUEL_MAX_GUC / 100.0f) * JOY_ISARET_X;
            eksenY.hedefGuc = joyY * (MANUEL_MAX_GUC / 100.0f) * JOY_ISARET_Y;
            rampaVeAralik(eksenX, dt);
            rampaVeAralik(eksenY, dt);
            hedefKilitlendi = false; kilitBaslangicMs = 0;
        } else if (!hedefVar) {
            hedefYokDurumu(dt);
        } else {
            pidVeHizHesapla(eksenX, hedefX, MERKEZ_X, dt, yeniKameraVerisi);
            pidVeHizHesapla(eksenY, hedefY, dinamikMerkezY, dt, yeniKameraVerisi);
            yeniKameraVerisi = false; 

            float hataX = fabs(eksenX.filtreli - MERKEZ_X);
            float hataY = fabs(eksenY.filtreli - dinamikMerkezY);

            if (hataX <= toleransPx && hataY <= toleransPx) {
                if (kilitBaslangicMs == 0) kilitBaslangicMs = millis();
                else if (millis() - kilitBaslangicMs >= KILITLENME_SURESI_MS) hedefKilitlendi = true;
            } else {
                kilitBaslangicMs = 0; hedefKilitlendi = false; yenidenKilitGerekli = false;   
            }

            otomatikAtisKontrol();
        }
    }

    motorSur(eksenX);
    motorSur(eksenY);
}
