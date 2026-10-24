#include "pid.h"
#include "motor.h"
#include <cmath>

void otoAyarBaslat(Eksen &e, const char *ad) {
    e.at = OtoAyar{};
    e.at.aktif = true;
    e.at.baslangicMs = millis();
    e.toplamHata = 0;
    Serial.printf("Oto-ayar basladi: %s\n", ad);
}

void roleAdimi(Eksen &e, float hata) {
    OtoAyar &a = e.at;

    if (millis() - a.baslangicMs > AT_ZAMAN_ASIMI_MS) {
        a.aktif = false;
        e.hedefGuc = 0;
        e.anlikGuc = 0;
        Serial.println("Oto-ayar zaman asimi!");
        return;
    }

    if (a.yon == 0) {
        a.yon = (hata > 0) ? 1 : -1;
        a.tepe = hata;
        a.vadi = hata;
    }

    if (a.yon > 0 && hata > a.tepe) a.tepe = hata;
    if (a.yon < 0 && hata < a.vadi) a.vadi = hata;

    if (a.yon > 0 && hata < -ROLE_HISTEREZ) {
        a.yon = -1;
        a.vadi = hata;
    } else if (a.yon < 0 && hata > ROLE_HISTEREZ) {
        a.yon = 1;
        unsigned long simdi = micros();
        if (a.sonPozUs != 0) {
            a.dongu++;
            if (a.dongu > 2) {
                a.genlikTop  += (a.tepe - a.vadi) / 2.0;
                a.periyotTop += (simdi - a.sonPozUs) / 1e6;
                a.olcum++;
            }
        }
        a.sonPozUs = simdi;
        a.tepe = hata;
    }

    e.hedefGuc = a.yon * ROLE_GUC;

    if (a.olcum >= ROLE_OLCUM) {
        float genlik = a.genlikTop / a.olcum;
        float Tu     = a.periyotTop / a.olcum;
        float etkinGenlik = sqrtf(fmaxf(genlik * genlik - ROLE_HISTEREZ * ROLE_HISTEREZ, 1.0f));
        float Ku     = (4.0f * ROLE_GUC) / (PI * etkinGenlik);
        float dt     = PID_PERIYOT_US / 1e6;

        e.Kp = 0.35 * Ku;
        e.Ki = (e.Kp / (0.5 * Tu)) * dt;
        e.Kd = 0;

        e.toplamHata = 0;
        e.oncekiHata = 0;
        e.hedefGuc = 0;
        e.anlikGuc = 0;
        a.aktif = false;

        Serial.printf("Oto-ayar bitti -> Genlik: %.2f px, Tu: %.3f s, Ku: %.4f | Kp: %.4f, Ki: %.6f\n", genlik, Tu, Ku, e.Kp, e.Ki);
    }
}

void pidVeHizHesapla(Eksen &e, int hedef, float merkez, float dt, bool yeniVeri) {
    if (yeniVeri) {
        e.filtreli = ALFA * hedef + (1.0 - ALFA) * e.filtreli;
        float hata = e.filtreli - merkez;

        if (e.at.aktif) {
            roleAdimi(e, hata);
        } else {
            if (fabs(hata) <= OLU_BOLGE_PX) {
                e.hedefGuc = 0;
                e.toplamHata = 0;
                e.oncekiHata = hata;
            } else {
                if (fabs(hata) < 30.0) {
                    e.toplamHata += hata;
                    if (e.Ki > 0.0) {
                        float lim = I_MAX_GUC / e.Ki;
                        e.toplamHata = constrain(e.toplamHata, -lim, lim);
                    }
                } else {
                    e.toplamHata = 0;
                }
                float dHata = hata - e.oncekiHata;
                float guc = e.Kp * hata + e.Ki * e.toplamHata + e.Kd * dHata;
                e.oncekiHata = hata;
                e.hedefGuc = constrain(guc, -100.0, 100.0);
            }
        }
    }
    rampaVeAralik(e, dt);
}
