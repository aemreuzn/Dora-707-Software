#include "guvenlik.h"
#include "motor.h"
#include <cmath>

bool manuelBaglantiTaze() { return (millis() - sonJoyMs) <= MANUEL_ZAMAN_ASIMI_MS; }
bool hedefVerisiTaze() { return (millis() - sonVeriZamani) <= WATCHDOG_MS; }

bool baglantiHazir() {
    if (mod == MOD_MANUEL) return manuelBaglantiTaze();
    if (!hedefVerisiTaze()) return false;
    if (OTONOM_OPERATOR_HEARTBEAT && !manuelBaglantiTaze()) return false;
    return true;
}

bool atisBolgesiIzinli() {
    float p = aciDerece(eksenX);
    float t = aciDerece(eksenY);
    for (int i = 0; i < ATIS_YASAK_SAYISI; i++) {
        if (!atisYasak[i].aktif) continue;
        if (p >= atisYasak[i].pMin - ATIS_MARJ_DERECE && p <= atisYasak[i].pMax + ATIS_MARJ_DERECE &&
            t >= atisYasak[i].tMin - ATIS_MARJ_DERECE && t <= atisYasak[i].tMax + ATIS_MARJ_DERECE) return false;
    }
    return true;
}

float dinamikToleransPx() {
    float aci = atan2(10.0f, (float)hedefZ_CM) * RAD_TO_DEG;
    float px = aci / DERECE_BASINA_PIKSEL_Y;
    return constrain(px * 0.4f, 3.0f, 15.0f);
}

void dinamikMerkezGuncelle() {
    float aci = atan2(KAMERA_NAMLU_FARKI_CM, (float)hedefZ_CM) * RAD_TO_DEG;
    dinamikMerkezY = SABIT_MERKEZ_Y + (aci / DERECE_BASINA_PIKSEL_Y);
    toleransPx = dinamikToleransPx();
}

void atisBaslat() {
    digitalWrite(ATIS_PIN, ATIS_AKTIF_SEVIYE);
    atisYapiliyor = true;
    atisBaslangicMs = millis();
    sonAtisMs = millis();
    atisSayaci++;
    yenidenKilitGerekli = TEKRAR_ATIS_ICIN_YENIDEN_KILIT;
}

void atisDurdur() {
    digitalWrite(ATIS_PIN, ATIS_PASIF_SEVIYE);
    atisYapiliyor = false;
}

void atisGuncelle() {
    if (atisYapiliyor && (millis() - atisBaslangicMs >= atisSureMs)) {
        atisDurdur();
    }
}

bool manuelAtisTetikle() {
    if (!sistemKurulu) { Serial.println("HATA: Sistem kurulmamis!"); return false; }
    if (mod == MOD_OTONOM && !hedefVerisiTaze()) { Serial.println("HATA: Hedef verisi yok"); return false; }
    if (eksenX.at.aktif || eksenY.at.aktif) { Serial.println("HATA: Oto-ayar suruyor"); return false; }
    if (!atisBolgesiIzinli()) { Serial.println("HATA: Yasak bolge!"); return false; }
    if (millis() - sonAtisMs < ATIS_BEKLEME_MS) return false;
    if (atisYapiliyor) return false;
    atisBaslat();
    Serial.println("BILGI: Manuel atis.");
    return true;
}

void otomatikAtisKontrol() {
    if (!otomatikAtisAktif || !sistemKurulu || !hedefVar || !hedefKilitlendi || !hedefVerisiTaze()) return;
    if (eksenX.at.aktif || eksenY.at.aktif || yenidenKilitGerekli || !atisBolgesiIzinli()) return;
    if (millis() - sonAtisMs < ATIS_BEKLEME_MS || atisYapiliyor) return;
    atisBaslat();
    Serial.println("OTOMATIK ATIS GERCEKLESTI!");
}

void eksenGecmisiSifirla(Eksen &e) {
    e.hedefGuc = 0; e.toplamHata = 0; e.oncekiHata = 0; e.at.aktif = false;
}

void hedefYokDurumu(float dt) {
    if (eksenX.at.aktif || eksenY.at.aktif) Serial.println("Oto-ayar iptal: hedef kayboldu");
    eksenGecmisiSifirla(eksenX); eksenGecmisiSifirla(eksenY);
    rampaVeAralik(eksenX, dt); rampaVeAralik(eksenY, dt);
    hedefKilitlendi = false; kilitBaslangicMs = 0; yenidenKilitGerekli = false;
}

void hedefYenidenBulundu() {
    eksenX.filtreli = hedefX; eksenY.filtreli = hedefY;
    eksenX.oncekiHata = eksenX.filtreli - MERKEZ_X;
    eksenY.oncekiHata = eksenY.filtreli - dinamikMerkezY;
    eksenX.toplamHata = 0; eksenY.toplamHata = 0;
}

void modDegistir(Mod yeni) {
    if (yeni == mod) return;
    mod = yeni;
    eksenGecmisiSifirla(eksenX); eksenGecmisiSifirla(eksenY);
    joyX = 0; joyY = 0; sonJoyMs = millis();
    hedefKilitlendi = false; kilitBaslangicMs = 0; yenidenKilitGerekli = false;
    otomatikAtisAktif = false;
    if (mod == MOD_OTONOM) {
        eksenX.filtreli = hedefX; eksenY.filtreli = hedefY;
        Serial.println("BILGI: OTONOM mod.");
    } else {
        Serial.println("BILGI: MANUEL mod.");
    }
}

void guvenliDurus(const char *neden) {
    if (eksenX.at.aktif || eksenY.at.aktif) Serial.println("Oto-ayar iptal: baglanti kesildi");
    eksenX.at.aktif = false; eksenY.at.aktif = false;
    eksenX.hedefGuc = 0; eksenY.hedefGuc = 0;
    eksenX.anlikGuc = 0; eksenY.anlikGuc = 0;
    eksenX.adimAralikUs = 0; eksenY.adimAralikUs = 0;
    joyX = 0; joyY = 0;
    eksenX.toplamHata = 0; eksenY.toplamHata = 0;
    eksenX.oncekiHata = 0; eksenY.oncekiHata = 0;
    atisDurdur();
    hedefKilitlendi = false; kilitBaslangicMs = 0; yenidenKilitGerekli = false;
    hedefVar = false;
    if (sistemKurulu || otomatikAtisAktif) Serial.printf("UYARI: %s, sistem DISARM edildi.\n", neden);
    sistemKurulu = false; otomatikAtisAktif = false;
    sonPidUs = micros();
}
