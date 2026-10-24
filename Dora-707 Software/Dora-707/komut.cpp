#include "komut.h"
#include "globals.h"
#include "guvenlik.h"
#include "pid.h"
#include "motor.h"
#include <cstdio>
#include <cstring>

typedef void (*KomutIsleyici)(char*);

struct KomutSatiri {
    const char* anahtar;
    KomutIsleyici fonk;
    bool tamEslesme;
};

void hndl_Hedef(char* p) {
    int x, y, z;
    if (sscanf(p, "<%d,%d,%d>", &x, &y, &z) == 3) {
        if (x < 0 || x > (int)GORUNTU_GENISLIK || y < 0 || y > (int)GORUNTU_YUKSEKLIK) {
            Serial.println("HATA: Hedef paketi goruntu disinda."); return;
        }
        bool yeniBulundu = !hedefVar;
        sonVeriZamani = millis();
        hedefX = x; hedefY = y; hedefZ_CM = (z <= 10) ? 10 : z;
        dinamikMerkezGuncelle();
        hedefVar = true; yeniKameraVerisi = true;
        if (yeniBulundu && mod == MOD_OTONOM) hedefYenidenBulundu();
    }
}

void hndl_Yok(char* p) { sonVeriZamani = millis(); hedefVar = false; }

void hndl_Joy(char* p) {
    float a, b;
    if (sscanf(p, "JOY,%f,%f", &a, &b) == 2) {
        sonJoyMs = millis();
        if (mod == MOD_MANUEL) {
            joyX = constrain(a, -100.0f, 100.0f);
            joyY = constrain(b, -100.0f, 100.0f);
        }
    }
}

void hndl_Arm(char* p) {
    if (!baglantiHazir()) { Serial.println("HATA: ARM reddedildi."); return; }
    sistemKurulu = true; hedefKilitlendi = false; kilitBaslangicMs = 0;
    Serial.println("UYARI: SILAH KURULDU (ARMED)!");
}

void hndl_Disarm(char* p) {
    sistemKurulu = false; atisDurdur();
    Serial.println("BILGI: SILAH EMNIYETTE (DISARMED).");
}

void hndl_OtoAtes1(char* p) {
    if (mod == MOD_OTONOM) {
        otomatikAtisAktif = true; hedefKilitlendi = false; kilitBaslangicMs = 0;
        Serial.println("BILGI: Otomatik atis AKTIF.");
    } else Serial.println("HATA: Yalnizca OTONOM modda.");
}
void hndl_OtoAtes0(char* p) { otomatikAtisAktif = false; Serial.println("BILGI: Oto atis PASIF."); }

void hndl_Ates(char* p) { manuelAtisTetikle(); }

void hndl_AtesSure(char* p) {
    int i;
    if (sscanf(p, "ATESSURE,%d", &i) == 1) {
        if (atisYapiliyor) Serial.println("HATA: Atis suruyor.");
        else if (i >= (int)ATIS_SURE_MIN_MS && i <= (int)ATIS_SURE_MAX_MS) {
            atisSureMs = (unsigned long)i; Serial.printf("BILGI: Sure = %lu ms\n", atisSureMs);
        }
    }
}

void hndl_Mod1(char* p) { modDegistir(MOD_MANUEL); }
void hndl_Mod0(char* p) { modDegistir(MOD_OTONOM); }

void hndl_Atx(char* p) {
    if (mod != MOD_OTONOM) Serial.println("HATA: OTONOM mod lazim.");
    else if (!hedefVar) Serial.println("HATA: Hedef lazim.");
    else otoAyarBaslat(eksenX, "Pan (X)");
}
void hndl_Aty(char* p) {
    if (mod != MOD_OTONOM) Serial.println("HATA: OTONOM mod lazim.");
    else if (!hedefVar) Serial.println("HATA: Hedef lazim.");
    else otoAyarBaslat(eksenY, "Tilt (Y)");
}

void hndl_Pidx(char* p) {
    float a,b,c;
    if (sscanf(p + 5, "%f,%f,%f", &a, &b, &c) == 3) {
        if (eksenX.at.aktif) Serial.println("HATA: Oto-ayar suruyor.");
        else { eksenX.Kp=a; eksenX.Ki=b; eksenX.Kd=c; eksenX.toplamHata=0; Serial.println("PID X ayarlandi."); }
    }
}
void hndl_Pidy(char* p) {
    float a,b,c;
    if (sscanf(p + 5, "%f,%f,%f", &a, &b, &c) == 3) {
        if (eksenY.at.aktif) Serial.println("HATA: Oto-ayar suruyor.");
        else { eksenY.Kp=a; eksenY.Ki=b; eksenY.Kd=c; eksenY.toplamHata=0; Serial.println("PID Y ayarlandi."); }
    }
}

void hndl_Sifir(char* p) {
    if (sistemKurulu) Serial.println("HATA: DISARM lazim.");
    else { eksenX.adim = 0; eksenY.adim = 0; Serial.println("Sifirlandi."); }
}

void hndl_Durum(char* p) {
    Serial.printf("Pan: %.1f, Tilt: %.1f | Kilit: %d\n", aciDerece(eksenX), aciDerece(eksenY), hedefKilitlendi);
}

void hndl_Yardim(char* p) { Serial.println("KOMUTLAR: <x,y,z> | YOK | JOY | ARM | DISARM..."); }

void hndl_Yc(char* p) {
    if (sistemKurulu) Serial.println("HATA: DISARM lazim.");
    else { for(int k=0; k<ATIS_YASAK_SAYISI; k++) atisYasak[k].aktif = false; Serial.println("Temizlendi."); }
}

void hndl_Yh(char* p) {
    float a,b,c,d;
    if (sscanf(p, "YH,%f,%f,%f,%f", &a, &b, &c, &d) == 4) {
        eksenX.minAci=a; eksenX.maxAci=b; eksenY.minAci=c; eksenY.maxAci=d; Serial.println("Limit OK.");
    }
}

void hndl_Ya(char* p) {
    int i; float a,b,c,d;
    if (sscanf(p, "YA,%d,%f,%f,%f,%f", &i, &a, &b, &c, &d) == 5 && i >= 0 && i < ATIS_YASAK_SAYISI) {
        atisYasak[i] = {true, a, b, c, d}; Serial.println("Yasak bolge OK.");
    }
}

KomutSatiri komutlar[] = {
    {"<", hndl_Hedef, false},
    {"YOK", hndl_Yok, true},
    {"JOY,", hndl_Joy, false},
    {"ARM", hndl_Arm, true},
    {"DISARM", hndl_Disarm, true},
    {"OTO_ATES,1", hndl_OtoAtes1, true},
    {"OTO_ATES,0", hndl_OtoAtes0, true},
    {"ATES", hndl_Ates, true},
    {"ATESSURE,", hndl_AtesSure, false},
    {"MOD,1", hndl_Mod1, true},
    {"MOD,0", hndl_Mod0, true},
    {"ATX", hndl_Atx, true},
    {"ATY", hndl_Aty, true},
    {"PIDX,", hndl_Pidx, false},
    {"PIDY,", hndl_Pidy, false},
    {"SIFIR", hndl_Sifir, true},
    {"DURUM", hndl_Durum, true},
    {"YARDIM", hndl_Yardim, true},
    {"YC", hndl_Yc, true},
    {"YH,", hndl_Yh, false},
    {"YA,", hndl_Ya, false}
};

void komutIsle(char *paket) {
    for (auto& cmd : komutlar) {
        if (cmd.tamEslesme) {
            if (strcmp(paket, cmd.anahtar) == 0) { cmd.fonk(paket); return; }
        } else {
            if (strncmp(paket, cmd.anahtar, strlen(cmd.anahtar)) == 0) { cmd.fonk(paket); return; }
        }
    }
    Serial.printf("HATA: Bilinmeyen komut: %s\n", paket);
}
