// --------------------------------------------------------------------------------------
// Der Riesen Piranha
//
// Schwimmt von einer Beckenseite zur anderen und spuckt wenn der Spieler in der Nähe ist
// kleine Piranhas
// --------------------------------------------------------------------------------------

#include "Boss_RiesenPiranha.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerRiesenPiranha::GegnerRiesenPiranha(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    AnimStart = 0;
    AnimEnde = 4;
    AnimSpeed = 2.0f;
    xSpeed = -7.0;
    ySpeed = 0.0f;
    xAcc = 0.0f;
    yAcc = 0.0f;
    Energy = 3000;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerRiesenPiranha::DoKI() {
    // Animieren
    if (AnimEnde > 0)  // Soll überhaupt anmiert werden ?
    {
        AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
        if (AnimCount > AnimSpeed)  // Grenze überschritten ?
        {
            AnimCount = 0;              // Dann wieder auf Null setzen
            AnimPhase++;                // Und nächste Animationsphase
            if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
                AnimPhase = AnimStart;  // Dann wieder von vorne beginnen
        }
    }  // animieren

    // Per Zufall Bläschen erzeugen
    if (GetRandom(10) == 0)
        PartikelSystem.PushPartikel(xPos + 10.0f + static_cast<float>(GetRandom(230)),
                                    yPos + 10.0f + static_cast<float>(GetRandom(190)), BUBBLE);

    // Nach links bzw rechts auf Kollision prüfen und dann ggf umkehren
    if (BlickRichtung == DirectionEnum::LINKS)
        if (blockl & BLOCKWERT_WAND || blockl & BLOCKWERT_GEGNERWAND) {
            xSpeed = 0;
            Handlung = GEGNER::DREHEN;
            AnimPhase = 4;
            AnimStart = 0;
            AnimEnde = 8;
        }

    if (BlickRichtung == DirectionEnum::RECHTS)
        if (blockr & BLOCKWERT_WAND || blockr & BLOCKWERT_GEGNERWAND) {
            xSpeed = 0;
            xPos -= 1.0f;
            Handlung = GEGNER::DREHEN;
            AnimPhase = 4;
            AnimStart = 0;
            AnimEnde = 8;
        }

    // Spieler im Wasser und in Sichtweite ?
    if (pAim->InLiquid == true && PlayerAbstand() <= 300) {
        // ggf kleinen Piranha ausspucken
        if (AnimPhase == 0 && AnimCount == 0.0f && GetRandom(2) == 0) {
            Gegner.PushGegner(xPos + 110.0f, yPos + 100.0f, PIRANHA, 0, 0, ChangeLight);
        }

        if (pAim->ypos - 70 < yPos &&  // Spieler oberhalb oder
            !(blocko & BLOCKWERT_WAND) && !(blocko & BLOCKWERT_GEGNERWAND))
            yPos -= Timer.sync(6.0f);

        if (pAim->ypos - 70 > yPos &&  // unterhalb des Piranhas
            !(blocku & BLOCKWERT_WAND) && !(blocku & BLOCKWERT_GEGNERWAND))
            yPos += Timer.sync(6.0f);  // Dann auf ihn zu schwimmen
    }

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER::LAUFEN:  // Piranha schwimmt rum
        {
            if (pAim->InLiquid == true &&  // Spieler im Wasser und
                PlayerAbstand() <= 300)    // in Sichtweite ?
            {
                // Piranha links oder rechts am Spieler vorbei ?
                // Dann umdrehen und weiter verfolgen
                if (Handlung == GEGNER::LAUFEN) {
                    if ((BlickRichtung == DirectionEnum::LINKS && pAim->xpos > xPos + GegnerRect[GegnerArt].right - 20) ||

                        (BlickRichtung == DirectionEnum::RECHTS && pAim->xpos + pAim->CollideRect.right < xPos)) {
                        xSpeed = 0;
                        Handlung = GEGNER::DREHEN;
                        AnimPhase = 4;
                        AnimStart = 0;
                        AnimEnde = 8;
                    }
                }
            } else {
                if (BlickRichtung == DirectionEnum::LINKS)
                    xSpeed = -7.0f;
                else
                    xSpeed = 7.0f;
            }
        } break;

        case GEGNER::DREHEN:  // Piranha dreht sich um
        {
            if (AnimPhase == AnimStart) {
                Handlung = GEGNER::LAUFEN;
                AnimEnde = 4;
                AnimStart = 0;
                AnimPhase = 0;

                if (BlickRichtung == DirectionEnum::LINKS)  // Ab jetzt in die andere Richtung schwimmen
                {
                    BlickRichtung = DirectionEnum::RECHTS;
                    xSpeed = 7.0f;
                } else {
                    BlickRichtung = DirectionEnum::LINKS;
                    xSpeed = -7.0f;
                }
            }
        } break;

        default:
            break;
    }  // switch

    // Testen, ob der Spieler den Piranha berührt hat
    TestDamagePlayers(Timer.sync(5.0f));
}

// --------------------------------------------------------------------------------------
// Riesen Piranha explodiert
// --------------------------------------------------------------------------------------

void GegnerRiesenPiranha::GegnerExplode() {
    // Fetzen erzeugen
    for (int i = 0; i < 8; i++)
        PartikelSystem.PushPartikel(xPos + 64.0f + static_cast<float>(GetRandom(64)),
                                    yPos + 64.0f + static_cast<float>(GetRandom(64)), PIRANHATEILE2);

    // und noch n paar Luftblässchen dazu
    for (int i = 0; i < 100; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(GetRandom(256)),
                                    yPos + 10.0f + static_cast<float>(GetRandom(210)), BUBBLE);

    // Blutwolken dazu
    for (int i = 0; i < 70; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(GetRandom(256)),
                                    yPos + 10.0f + static_cast<float>(GetRandom(210)), PIRANHABLUT);

    // und Extra Leben rauslassen
    Gegner.PushGegner(xPos + 110.0f, yPos + 100.0f, ONEUP, 0, 0, ChangeLight);

    // SoundManager.PlayWave(100, 128, -rand()%2000+11025, SOUND::EXPLOSION1);	// Sound ausgeben

    Player[0].Score += 4000;
}
