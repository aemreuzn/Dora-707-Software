#ifndef GLOBALS_H
#define GLOBALS_H
#include "config.h"

struct OtoAyar {
    bool aktif;
    int yon;
    float tepe, vadi;
    unsigned long sonPozUs;
    unsigned long baslangicMs;
    int dongu;
    float genlikTop, periyotTop;
    int olcum;
};

struct Eksen {
    int dirPin, pulPin;
    float Kp, Ki, Kd;
    float filtreli, oncekiHata, toplamHata;
    float hedefGuc, anlikGuc;
    int sonDirDurum;
    unsigned long adimAralikUs, sonAdimUs;
    long  adim;
    float adimPerDerece, isaret, minAci, maxAci;
    OtoAyar at;
};

struct AtisYasak {
    bool aktif;
    float pMin, pMax, tMin, tMax;
};

enum Mod { MOD_OTONOM = 0, MOD_MANUEL = 1 };

extern Eksen eksenX;
extern Eksen eksenY;
extern AtisYasak atisYasak[ATIS_YASAK_SAYISI];

extern int hedefX, hedefY, hedefZ_CM;
extern float dinamikMerkezY;
extern bool hedefVar, yeniKameraVerisi;

extern unsigned long sonVeriZamani, sonPidUs;
extern bool sistemKurulu, otomatikAtisAktif, hedefKilitlendi, yenidenKilitGerekli;
extern unsigned long kilitBaslangicMs, sonAtisMs, atisBaslangicMs, atisSureMs, atisSayaci;
extern bool atisYapiliyor;

extern Mod mod;
extern float joyX, joyY;
extern unsigned long sonJoyMs;
extern float toleransPx;

#endif
