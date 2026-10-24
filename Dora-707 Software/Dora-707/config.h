#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

const int ATIS_PIN = 32;
const int ATIS_AKTIF_SEVIYE = HIGH;
const int ATIS_PASIF_SEVIYE = (ATIS_AKTIF_SEVIYE == HIGH) ? LOW : HIGH;

const float MERKEZ_X = 640.0;
const float SABIT_MERKEZ_Y = 360.0;
const float GORUNTU_GENISLIK = MERKEZ_X * 2.0f;
const float GORUNTU_YUKSEKLIK = SABIT_MERKEZ_Y * 2.0f;
const float KAMERA_NAMLU_FARKI_CM = 10.0;
const float DERECE_BASINA_PIKSEL_Y = 60.0 / 720.0;

const float ALFA = 0.4;
const unsigned long PID_PERIYOT_US = 2000;
const unsigned int  PULSE_GENISLIK_US = 5;

const float HIZ_MIN_SPS = 50.0;
const float HIZ_MAX_SPS = 3500.0;
const float IVME_SPS2   = 8000.0;

const float I_MAX_GUC = 20.0;
const float OLU_BOLGE_PX = 3.0f;

const unsigned long KILITLENME_SURESI_MS = 150;
const unsigned long ATIS_BEKLEME_MS      = 500;
const unsigned long ATIS_SURE_MIN_MS     = 20;
const unsigned long ATIS_SURE_MAX_MS     = 300;
const unsigned long WATCHDOG_MS          = 500;

const unsigned long MANUEL_ZAMAN_ASIMI_MS = 300;
const float MANUEL_MAX_GUC = 25.0f;
const float JOY_ISARET_X = 1.0f;
const float JOY_ISARET_Y = -1.0f;
const bool OTONOM_OPERATOR_HEARTBEAT = false;
const bool TEKRAR_ATIS_ICIN_YENIDEN_KILIT = false;

const float ROLE_GUC      = 6.0;
const float ROLE_HISTEREZ = 8.0;
const int   ROLE_OLCUM    = 5;
const unsigned long AT_ZAMAN_ASIMI_MS = 30000;

const int   ATIS_YASAK_SAYISI = 4;
const float ATIS_MARJ_DERECE  = 1.0;

#endif
