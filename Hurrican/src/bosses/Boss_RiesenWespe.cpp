// --------------------------------------------------------------------------------------
// Die Riesen Wespe
//
// fliegt durch die Gegend und schiesst (weiss noch nicht, was)
// --------------------------------------------------------------------------------------

#include "Boss_RiesenWespe.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerRiesenWasp::GegnerRiesenWasp(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    Energy = 125;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    AnimEnde = 5;
    AnimSpeed = 1.0f;
    BlickRichtung = DirectionEnum::LINKS;
    ShotDelay = 10.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerRiesenWasp::DoKI() {
    // animieren
    AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
    if (AnimCount > AnimSpeed)  // Grenze überschritten ?
    {
        AnimCount = 0;  // Dann wieder auf Null setzen
        AnimPhase++;    // Und nächste Animationsphase

        if (AnimPhase > AnimEnde)  // Animation von zu Ende	?
        {
            AnimPhase = AnimStart;
        }
    }

    // in richtige Richtung kucken
    //
    if (xSpeed > 0.0f)
        BlickRichtung = DirectionEnum::RECHTS;
    else
        BlickRichtung = DirectionEnum::LINKS;

    // in Richtung Spieler fliegen
    //
    if (Handlung != GEGNER::FALLEN) {
        if (xPos + 50 < pAim->xpos + 35)
            xAcc = 2.5f;
        if (xPos + 50 > pAim->xpos + 35)
            xAcc = -2.5f;

        if (yPos < pAim->ypos - 100)
            yAcc = 1.5f;
        if (yPos > pAim->ypos - 100)
            yAcc = -1.5f;

        xSpeed = std::clamp(xSpeed, -22.0f, 22.0f);
        ySpeed = std::clamp(ySpeed, -10.0f, 10.0f);

        // An der Wand umdrehen
        //
        if ((blocko & BLOCKWERT_WAND || blocko & BLOCKWERT_GEGNERWAND) && ySpeed < 0.0f)
            ySpeed *= 1.0f;
        if ((blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_GEGNERWAND) && ySpeed > 0.0f)
            ySpeed *= 1.0f;
        if ((blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND) && xSpeed < 0.0f)
            xSpeed *= 1.0f;
        if ((blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND) && xSpeed > 0.0f)
            xSpeed *= 1.0f;
    }

    // evtl schiessen lassen
    //
    if (PlayerAbstand() < 500 && PlayerAbstand() > 100 &&
        ((BlickRichtung == DirectionEnum::RECHTS && xPos + 50 < pAim->xpos + 35) ||
         (BlickRichtung == DirectionEnum::LINKS && xPos + 50 > pAim->xpos + 35))) {
        ShotDelay -= Timer.sync(1.0f);

        if (ShotDelay <= 0.0f) {
            ShotDelay = 10.0f;

            SoundManager.PlayWave(100, 128, 8000 + GetRandom(1000), SOUND::CANON);
            Projectiles.PushProjectile(xPos + 25.0f + static_cast<float>(Direction::asInt(BlickRichtung) * 56),
                                       yPos + 51.0f, SUCHSCHUSS2);

            for (int i = 0; i < 5; i++)
                PartikelSystem.PushPartikel(xPos + 50.0f + static_cast<float>(GetRandom(5) + Direction::asInt(BlickRichtung) * 60),
                                            yPos + 75.0f + static_cast<float>(GetRandom(5)),
                                            SMOKE3);
        }
    }

    // Je nach Handlung richtig verhalten
    //
    switch (Handlung) {
        // Wespe stürzt ab
        //
        case GEGNER::FALLEN: {
            // An die Wand gekracht ?
            if (blockl & BLOCKWERT_WAND || blockr & BLOCKWERT_WAND || blocko & BLOCKWERT_WAND ||
                blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM)
                Energy = 0.0f;

            // Grenze der Fallgeschwindigkeit
            if (ySpeed > 35.0f)
                yAcc = 0.0f;

            // Wespe rauchen lassen
            if (AnimCount >= 0.5f) {
                AnimCount = 0.0f;
                PartikelSystem.PushPartikel(xPos + 30.0f + static_cast<float>(GetRandom(80)),
                                            yPos + 30.0f + static_cast<float>(GetRandom(50)), SMOKE);
                PartikelSystem.PushPartikel(xPos + 30.0f + static_cast<float>(GetRandom(80)),
                                            yPos + 30.0f + static_cast<float>(GetRandom(50)), SMOKE3);
            }
        } break;
    }

    // Testen, ob der Spieler die Wespe berührt hat
    TestDamagePlayers(Timer.sync(5.0f));

    // Wespe stürzt ab ?
    //
    if (Energy <= 0.0f && Handlung != GEGNER::FALLEN) {
        Handlung = GEGNER::FALLEN;
        AnimCount = 0.0f;
        Energy = 50.0f;
        ySpeed = 1.0f;
        yAcc = 2.0f;
        xAcc = 0.0f;
    }
}

// --------------------------------------------------------------------------------------
// RiesenWasp explodiert
// --------------------------------------------------------------------------------------

void GegnerRiesenWasp::GegnerExplode() {
    // Explosionen erzeugen
    for (int i = 0; i < 10; i++)
        PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(GetRandom(110)),
                                    yPos - 30.0f + static_cast<float>(GetRandom(60)), EXPLOSION_BIG);

    for (int i = 0; i < 20; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(GetRandom(110)),
                                    yPos - 10.0f + static_cast<float>(GetRandom(60)), EXPLOSION_MEDIUM2);

    for (int i = 0; i < 200; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(GetRandom(110)),
                                    yPos + static_cast<float>(GetRandom(50)), ROCKETSMOKE);

    for (int i = 0; i < 10; i++)
        PartikelSystem.PushPartikel(xPos + static_cast<float>(GetRandom(100)),
                                    yPos + 30.0f + static_cast<float>(GetRandom(50)), SPLITTER);

    SoundManager.PlayWave(100, 128, 11025 + GetRandom(2000), SOUND::EXPLOSION1);  // Sound ausgeben

    Player[0].Score += 250;
}
