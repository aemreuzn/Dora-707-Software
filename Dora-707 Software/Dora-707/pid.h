#ifndef PID_H
#define PID_H
#include "globals.h"

void otoAyarBaslat(Eksen &e, const char *ad);
void roleAdimi(Eksen &e, float hata);
void pidVeHizHesapla(Eksen &e, int hedef, float merkez, float dt, bool yeniVeri);

#endif
