#include "globals.h"

Eksen eksenX = {12, 14, 0.4, 0.005, 0.0, 640.0, 0.0, 0.0, 0, 0, -1, 0, 0, 0, 8.889,  1, -60, 60, {}};
Eksen eksenY = {27, 26, 0.3, 0.005, 0.0, 360.0, 0.0, 0.0, 0, 0, -1, 0, 0, 0, 8.889, -1,  -5, 60, {}};
AtisYasak atisYasak[ATIS_YASAK_SAYISI] = {};

int hedefX = 640, hedefY = 360, hedefZ_CM = 1000;
float dinamikMerkezY = SABIT_MERKEZ_Y;
bool hedefVar = false;
bool yeniKameraVerisi = false;

unsigned long sonVeriZamani = 0;
unsigned long sonPidUs = 0;

bool sistemKurulu = false;
bool otomatikAtisAktif = false;
bool hedefKilitlendi = false;
unsigned long kilitBaslangicMs = 0;
unsigned long sonAtisMs = 0;
unsigned long atisBaslangicMs = 0;
unsigned long atisSureMs = 80;
bool atisYapiliyor = false;
unsigned long atisSayaci = 0;
bool yenidenKilitGerekli = false;

Mod mod = MOD_MANUEL;
float joyX = 0.0f, joyY = 0.0f;
unsigned long sonJoyMs = 0;
float toleransPx = 3.0f;
