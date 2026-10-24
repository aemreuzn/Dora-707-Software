#ifndef MOTOR_H
#define MOTOR_H
#include "globals.h"

float aciDerece(const Eksen &e);
bool hareketIzinli(const Eksen &e, int yon);
void rampaVeAralik(Eksen &e, float dt);
void motorSur(Eksen &e);

#endif
