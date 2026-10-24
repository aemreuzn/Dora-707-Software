#include "motor.h"
#include <cmath>

float aciDerece(const Eksen &e) {
    return e.isaret * (float)e.adim / e.adimPerDerece;
}

bool hareketIzinli(const Eksen &e, int yon) {
    float aci = aciDerece(e);
    float dir = e.isaret * yon;
    float sonraki = aci + dir / e.adimPerDerece;
    if (dir > 0 && sonraki > e.maxAci) return false;
    if (dir < 0 && sonraki < e.minAci) return false;
    return true;
}

void rampaVeAralik(Eksen &e, float dt) {
    if (e.at.aktif) {
        e.anlikGuc = e.hedefGuc;
    } else {
        float maxGucDegisim = (IVME_SPS2 / HIZ_MAX_SPS * 100.0f) * dt;
        if (e.hedefGuc > e.anlikGuc) {
            e.anlikGuc += min(maxGucDegisim, e.hedefGuc - e.anlikGuc);
        } else if (e.hedefGuc < e.anlikGuc) {
            e.anlikGuc -= min(maxGucDegisim, e.anlikGuc - e.hedefGuc);
        }
    }

    float mutlakGuc = fabs(e.anlikGuc);
    if (mutlakGuc < 1.0) {
        e.adimAralikUs = 0;
    } else {
        float sps = HIZ_MIN_SPS + (HIZ_MAX_SPS - HIZ_MIN_SPS) * (mutlakGuc / 100.0f);
        e.adimAralikUs = (unsigned long)(1e6 / sps);
    }
}

void motorSur(Eksen &e) {
    if (e.adimAralikUs == 0) return;

    int yon = (e.anlikGuc > 0) ? 1 : -1;
    if (!hareketIzinli(e, yon)) {
        e.toplamHata = 0;
        e.anlikGuc = 0;
        e.adimAralikUs = 0;
        return;
    }

    unsigned long simdi = micros();
    if (simdi - e.sonAdimUs < e.adimAralikUs) return;

    int dirPinDurum = (yon > 0) ? HIGH : LOW;
    if (e.sonDirDurum != dirPinDurum) {
        digitalWrite(e.dirPin, dirPinDurum);
        e.sonDirDurum = dirPinDurum;
        delayMicroseconds(10);
    }

    digitalWrite(e.pulPin, HIGH);
    delayMicroseconds(PULSE_GENISLIK_US);
    digitalWrite(e.pulPin, LOW);

    e.adim += yon;
    e.sonAdimUs += e.adimAralikUs;
}
