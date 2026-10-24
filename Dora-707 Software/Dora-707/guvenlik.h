#ifndef GUVENLIK_H
#define GUVENLIK_H
#include "globals.h"

bool manuelBaglantiTaze();
bool hedefVerisiTaze();
bool baglantiHazir();
bool atisBolgesiIzinli();

float dinamikToleransPx();
void dinamikMerkezGuncelle();
void atisBaslat();
void atisDurdur();
void atisGuncelle();
bool manuelAtisTetikle();
void otomatikAtisKontrol();

void eksenGecmisiSifirla(Eksen &e);
void hedefYokDurumu(float dt);
void hedefYenidenBulundu();
void modDegistir(Mod yeni);
void guvenliDurus(const char *neden);

#endif
