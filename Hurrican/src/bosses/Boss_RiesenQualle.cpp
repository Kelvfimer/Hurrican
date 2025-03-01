// --------------------------------------------------------------------------------------
// Die riesen Qualle
//
// Schwabbelt unter Wasser rum in Richtung Spieler
// --------------------------------------------------------------------------------------

#include "Boss_RiesenQualle.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerRiesenQualle::GegnerRiesenQualle(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::LAUFEN;
    AnimStart = 0;
    AnimEnde = 6;
    AnimSpeed = 2.0f;
    BlickRichtung = DirectionEnum::LINKS;
    xSpeed = 0.0f;
    ySpeed = -8.0f;
    yAcc = 0.4f;
    Energy = 3000;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerRiesenQualle::DoKI() {
    // animieren
    AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
    if (AnimCount > AnimSpeed)  // Grenze überschritten ?
    {
        AnimCount = 0;             // Dann wieder auf Null setzen
        AnimPhase++;               // Und nächste Animationsphase
        if (AnimPhase > AnimEnde)  // Animation von zu Ende	?
            AnimPhase = AnimStart;
    }

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER::SPRINGEN:  // Nach oben abgestossen zum Schwimmen
        {
            // Geschwindigkeits-Begrenzung
            if (ySpeed > 0.0f)
                yAcc = 0.25f;

            if (ySpeed > 8.0f)
                yAcc = 0.0f;

            if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
            {
                // Dann absinken
                AnimPhase = 0;
                AnimEnde = 0;
                AnimStart = 0;
                Handlung = GEGNER::LAUFEN;
            }
        } break;

        case GEGNER::FALLEN: {
            if (PlayerAbstand() < 200)  // spieler verfolgen ?
            {
                AnimPhase = 0;
                Handlung = GEGNER::LAUFEN;
                yAcc = 0.0f;
            }
        } break;

        case GEGNER::LAUFEN:  // Qualle sinkt ab
        {
            // Nach oben abstossen
            if (yPos - 20.0f > pAim->ypos || blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_GEGNERWAND) {
                // Bläschen erzeugen
                PartikelSystem.PushPartikel(xPos + 20.0f, yPos + 30.0f, BUBBLE);

                // rechts / links bewegen
                if (xPos < pAim->xpos)
                    xSpeed = 1.0f;
                else
                    xSpeed = -1.0f;

                AnimPhase = 1;
                AnimEnde = 6;
                AnimCount = 0.0f;
                ySpeed = -8.0f;
                yAcc = 0.4f;
                Handlung = GEGNER::SPRINGEN;
            }
        } break;

        default:
            break;
    }  // switch

    // Testen, ob der Spieler die Qualle berührt hat
    TestDamagePlayers(Timer.sync(5.0f));
}

// --------------------------------------------------------------------------------------
// Riesen Qualle explodiert
// --------------------------------------------------------------------------------------

void GegnerRiesenQualle::GegnerExplode() {
    // Fetzen erzeugen
    for (int i = 0; i < 8; i++)
        PartikelSystem.PushPartikel(xPos + 64.0f + static_cast<float>(GetRandom(64)),
                                    yPos + 64.0f + static_cast<float>(GetRandom(64)), PIRANHATEILE2);

    // und noch n paar Luftbläschen dazu
    for (int i = 0; i < 100; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(GetRandom(256)),
                                    yPos + 10.0f + static_cast<float>(GetRandom(210)), BUBBLE);

    // Blutwolken dazu
    for (int i = 0; i < 70; i++)
        PartikelSystem.PushPartikel(xPos - 10.0f + static_cast<float>(GetRandom(256)),
                                    yPos + 10.0f + static_cast<float>(GetRandom(210)), PIRANHABLUT);

    // SoundManager.PlayWave(100, 128, -rand()%2000+11025, SOUND::EXPLOSION1);	// Sound ausgeben

    Player[0].Score += 4000;
}
