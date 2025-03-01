// --------------------------------------------------------------------------------------
// Die kleine Drone
//
// Umkreist den Spieler und Ballert auf ihn
// --------------------------------------------------------------------------------------

#include "Gegner_FieseDrone.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFieseDrone::GegnerFieseDrone(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    Energy = 70;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    xSpeed = 0.0f;
    ySpeed = 0.0f;
    yAcc = 0.0f;
    AnimSpeed = 1.5f;
    AnimEnde = 12;
    ShotDelay = static_cast<float>(GetRandom(12) + 12);
    SmokeDelay = 0.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFieseDrone::DoKI() {
    static int flamex = 0;

    // Animieren
    //
    AnimCount += SpeedFaktor;  // Animationscounter weiterzählen
    if (Handlung == GEGNER::LAUFEN) {
        if (AnimCount > AnimSpeed)  // Grenze überschritten ?
        {
            AnimCount = 0;              // Dann wieder auf Null setzen
            AnimPhase++;                // Und nächste Animationsphase
            if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
                AnimPhase = AnimStart;  // Dann wieder von vorne beginnen
        }
    } else if (Handlung == GEGNER::DREHEN) {
        if (AnimCount > AnimSpeed)  // Grenze überschritten ?
        {
            AnimCount = 0;             // Dann wieder auf Null setzen
            AnimPhase++;               // Und nächste Animationsphase
            if (AnimPhase > AnimEnde)  // Animation von zu Ende	?
            {
                AnimCount = 0.0f;
                Handlung = GEGNER::DREHEN2;
                BlickRichtung = Direction::invert(BlickRichtung);
                xPos -= Direction::asInt(BlickRichtung) * 5.0f;
                AnimPhase = AnimEnde;
            }
        }
    } else if (Handlung == GEGNER::DREHEN2) {
        if (AnimCount > AnimSpeed)  // Grenze überschritten ?
        {
            AnimCount = 0;        // Dann wieder auf Null setzen
            AnimPhase--;          // Und nächste Animationsphase
            if (AnimPhase <= 13)  // Animation von zu Ende	?
            {
                AnimPhase = 0;  // Dann wieder von vorne beginnen
                AnimStart = 0;
                AnimEnde = 12;
                Handlung = GEGNER::LAUFEN;
                AnimSpeed = 1.5f;
            }
        }
    }

    // in Richtung Spieler fliegen
    //
    if (xPos + 30.0f < pAim->xpos + 35.0f)
        xAcc = 1.0f;
    if (xPos + 30.0f > pAim->xpos + 35.0f)
        xAcc = -1.0f;

    if (yPos < pAim->ypos - 100.0f)
        yAcc = 1.0f;
    if (yPos > pAim->ypos - 100.0f)
        yAcc = -1.0f;

    xSpeed = std::clamp(xSpeed, -10.0f, 10.0f);
    ySpeed = std::clamp(ySpeed, -8.0f, 8.0f);

    if (Handlung == GEGNER::LAUFEN && PlayerAbstand() < 600) {
        if (ShotDelay > 0.0f)
            ShotDelay -= Timer.sync(1.0f);
        else {
            ShotDelay = static_cast<float>(GetRandom(12) + 12);

            float ydiv = (pAim->ypos + 40.0f) - (yPos + 20.0f);

            if (ydiv == 0.0f)
                ydiv = 0.00001f;

            float xdiv = (pAim->xpos + 35.0f) - (xPos + 20.0f);

            // DKS - Use new Rad/Deg macros - converted to float:
            // newwinkel = (float)atan(xdiv / ydiv) * 180.0f / D3DX_PI + 180.0f;
            float newwinkel = RadToDeg(atanf(xdiv / ydiv)) + 180.0f;

            if (xdiv >= 0.0f && ydiv >= 0.0f)
                newwinkel = newwinkel;
            else if (xdiv > 0.0f && ydiv < 0.0f)
                newwinkel = 180.0f + newwinkel;
            else if (xdiv < 0.0f && ydiv > 0)
                newwinkel = 360.0f + newwinkel;
            else if (xdiv < 0.0f && ydiv < 0.0f)
                newwinkel = 180.0f + newwinkel;

            WinkelUebergabe = 360.0f - newwinkel;

            Projectiles.PushProjectile(xPos + 20.0f, yPos + 20.0f, FLUGLASER);
            PartikelSystem.PushPartikel(xPos + 20.0f, yPos + 20.0f, SMOKE);
            SoundManager.PlayWave(50, 128, 25000 + GetRandom(5000), SOUND::LASERSHOT);
        }
    }

    if ((blocko & BLOCKWERT_WAND || blocko & BLOCKWERT_GEGNERWAND) && ySpeed < 0.0f)
        ySpeed *= 1.0f;
    if ((blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_GEGNERWAND) && ySpeed > 0.0f)
        ySpeed *= 1.0f;
    if ((blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND) && xSpeed < 0.0f)
        xSpeed *= 1.0f;
    if ((blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND) && xSpeed > 0.0f)
        xSpeed *= 1.0f;

    switch (Handlung) {
        // In der Luft rumdümpeln
        //
        case GEGNER::LAUFEN: {
            if ((xPos + 30.0f < pAim->xpos + 35.0f && BlickRichtung == DirectionEnum::LINKS) ||
                (xPos + 30.0f > pAim->xpos + 35.0f && BlickRichtung == DirectionEnum::RECHTS)) {
                Handlung = GEGNER::DREHEN;
                AnimPhase = 13;
                AnimStart = 13;
                AnimCount = 0.0f;
                AnimEnde = 17;
                AnimSpeed = 0.5f;
            }

        } break;
    }

    // Rauch
    //
    if (Handlung == GEGNER::LAUFEN) {
        if (BlickRichtung == DirectionEnum::LINKS)
            flamex = 31;
        else
            flamex = -12;
    } else {
        if (BlickRichtung == DirectionEnum::LINKS)
            flamex = 31 - (AnimPhase - 13) * 5;
        else
            flamex = -12 + (AnimPhase - 13) * 5;
    }

    SmokeDelay -= Timer.sync(1.0f);

    while (SmokeDelay < 0.0f) {
        SmokeDelay += 0.7f;

        PartikelSystem.PushPartikel(xPos + static_cast<float>(flamex), yPos + 28.0f, SMOKE);
    }

    // Testen, ob der Spieler die Drone berührt hat
    TestDamagePlayers(Timer.sync(1.0f));
}

// --------------------------------------------------------------------------------------
// FieseDrone explodiert
// --------------------------------------------------------------------------------------

void GegnerFieseDrone::GegnerExplode() {
    for (int i = 0; i < 5; i++)
        PartikelSystem.PushPartikel(xPos - 25.0f + static_cast<float>(GetRandom(40)),
                                    yPos - 25.0f + static_cast<float>(GetRandom(40)), EXPLOSION_MEDIUM2);
    for (int i = 0; i < 20; i++)
        PartikelSystem.PushPartikel(xPos + 10.0f + static_cast<float>(GetRandom(30)),
                                    yPos + 10.0f + static_cast<float>(GetRandom(30)), LASERFUNKE2);

    SoundManager.PlayWave(100, 128, -GetRandom(2000) + 11025, SOUND::EXPLOSION1);  // Sound ausgeben

    Player[0].Score += 80;
}
