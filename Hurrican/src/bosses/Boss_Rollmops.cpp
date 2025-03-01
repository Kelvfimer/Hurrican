// --------------------------------------------------------------------------------------
// Der rollende Mops kam in die Küche, und stahl dem Turri seine Eier
// --------------------------------------------------------------------------------------

#include "Boss_Rollmops.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerRollmops::GegnerRollmops(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::EINFLIEGEN;
    Energy = 4000;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = false;
    TestBlock = false;
    OwnDraw = true;
    SmokeCount = 0.0f;
    ShotDelay = 0.0f;
    GunWinkel = 0.0f;
    HookX = 0.0f;
    HookY = 0.0f;
    Schwung = 0.0f;
    SchwungDir = 0.0f;
    ShotCount = 0.0f;
    Delay = 0.0f;

    Rollen.LoadImage("rollmops_rollen.png", 650, 260, 130, 130, 5, 2);
    Aufklappen.LoadImage("rollmops_aufklappen.png", 390, 390, 130, 130, 3, 3);
    Gun.LoadImage("rollmops_gun.png", 53, 52, 53, 52, 1, 1);

    for (int i = 0; i < NUM_KETTENGLIEDER; i++) {
        Gegner.PushGegner(0, 0, KETTENGLIED, 0, 0, false);
        pKettenTeile[i] = reinterpret_cast<GegnerKettenglied *>(Gegner.pEnd);
        pKettenTeile[i]->pParent = this;
    }
}

// --------------------------------------------------------------------------------------
// Winkel zum Spieler ausrechnen
// --------------------------------------------------------------------------------------

void GegnerRollmops::CalcGunWinkel() {

    float ydiv = (pAim->ypos + 40) - (yPos + 60.0f);
    if (ydiv == 0.0f)
        ydiv = 0.00001f;

    // DKS - Optimized this a bit and converted atan to atanf:
    /* if (xPos < Value1 + 320.0f)
    {
        xdiv = (pAim->xpos + 35) - (xPos + 50);
        neww = (float)atan(ydiv / xdiv) * 180.0f / PI;
    }
    else
    {
        xdiv = (pAim->xpos + 35) - (xPos + 50);
        neww = -(float)atan(ydiv / xdiv) * 180.0f / PI;
    } */
    float xdiv = (pAim->xpos + 35) - (xPos + 50.0f);
    float neww = RadToDeg(atanf(ydiv / xdiv));

    if (xPos >= (Value1 + 320.0f)) {
        neww *= -1.0f;
    }

    neww = std::clamp(neww, -30.0f, 90.0f);

    if (GunWinkel < neww)
        GunWinkel += Timer.sync(5.0f);

    if (GunWinkel > neww)
        GunWinkel -= Timer.sync(5.0f);
}

// --------------------------------------------------------------------------------------
// Eigene Draw Funktion
// --------------------------------------------------------------------------------------

void GegnerRollmops::DoDraw() {
    bool mirrored = false;

    // Je nach Handlung andere Grafik rendern
    //
    switch (Handlung) {
        case GEGNER::AUSSPUCKEN: {

            float x = ((xPos + 65.0f - 9.0f) - HookX) / NUM_KETTENGLIEDER;
            float y = ((yPos + 65.0f) - HookY) / NUM_KETTENGLIEDER;

            for (int i = 0; i < NUM_KETTENGLIEDER; i++) {
                // Position setzen
                pKettenTeile[i]->xPos = HookX + i * x;
                pKettenTeile[i]->yPos = HookY + i * y;

                // Winkel setzen
                // DKS - used new macros:
                // pKettenTeile[i]->AnimCount = 360.0f - (Schwung * 180.0f / PI);
                pKettenTeile[i]->AnimCount = 360.0f - RadToDeg(Schwung);
            }

            Rollen.RenderSprite(xPos - TileEngine.XOffset,
                                yPos - TileEngine.YOffset, AnimPhase, 0xFFFFFFFF, mirrored);

        } break;

        case GEGNER::LAUFEN:
        case GEGNER::LAUFEN_LINKS:
        case GEGNER::LAUFEN_RECHTS:
        case GEGNER::LAUFEN_LINKS2:
        case GEGNER::LAUFEN_RECHTS2:
        case GEGNER::SPECIAL:
        case GEGNER::SPECIAL2:
        case GEGNER::SPECIAL3:
        case GEGNER::WARTEN:
        case GEGNER::EXPLODIEREN: {
            Rollen.RenderSprite(xPos - TileEngine.XOffset,
                                yPos - TileEngine.YOffset, AnimPhase, 0xFFFFFFFF, mirrored);
        } break;

        case GEGNER::INIT:
        case GEGNER::STEHEN: {
            pGegnerGrafix[GegnerArt]->RenderSpriteScaled(xPos - TileEngine.XOffset + 16.0f,
                                                         yPos - TileEngine.YOffset, 120, 120,
                                                         AnimPhase, 0xFFFFFFFF);
        } break;

        case GEGNER::OEFFNEN:
        case GEGNER::SCHLIESSEN: {
            mirrored = (xPos < Value1 + 320.0f);

            Aufklappen.RenderSprite(xPos - TileEngine.XOffset,
                                    yPos - TileEngine.YOffset, AnimPhase, 0xFFFFFFFF, mirrored);
        } break;

        case GEGNER::VERFOLGEN: {
            mirrored = (xPos < Value1 + 320.0f);

            float xoff = 0.0f;

            if (mirrored)
                xoff = 65.0f;

            // Knarre rendern
            Gun.RenderSpriteRotatedOffset(xPos - TileEngine.XOffset + 6.0f + xoff,
                                          yPos - TileEngine.YOffset + 38.0f, GunWinkel, 53, 20,
                                          0xFFFFFFFF, mirrored);

            // Mops ohne Knarre rendern
            Aufklappen.RenderSprite(xPos - TileEngine.XOffset,
                                    yPos - TileEngine.YOffset, 8, 0xFFFFFFFF, mirrored);
        } break;
    }
}

// --------------------------------------------------------------------------------------
// Neue Aktion festlegen
// --------------------------------------------------------------------------------------

void GegnerRollmops::NeueAktion() {
    GunAusfahren(true);
}

// --------------------------------------------------------------------------------------
// Aufklappen zum Schiessen
// --------------------------------------------------------------------------------------

void GegnerRollmops::GunAusfahren(bool Auf) {
    xSpeed = 0.0f;
    ySpeed = 0.0f;
    xAcc = 0.0f;
    yAcc = 0.0f;

    SoundManager.PlayWave(100, 128, 14000, SOUND::STEAM);

    if (Auf == true) {
        Handlung = GEGNER::OEFFNEN;

        AnimPhase = 0;
        AnimStart = 0;
        AnimEnde = 9;
        HitsToTake = Energy;
    } else {
        Handlung = GEGNER::SCHLIESSEN;

        AnimPhase = 7;
        AnimStart = 7;
        AnimEnde = 0;
        Delay = 0.0f;
    }

    AnimCount = 0.0f;
    AnimSpeed = 1.2f;
}

// --------------------------------------------------------------------------------------
// Abhopsen am Boden
// --------------------------------------------------------------------------------------

void GegnerRollmops::Abhopsen(float mul) {
    if (ySpeed > 0.0f && yPos > Value2 + RENDERHEIGHT - 100.0f - 40.0f) {
        yPos = Value2 + RENDERHEIGHT - 100.0f - 40.0f;

        SoundManager.PlayWave(100, 128, 8000 + GetRandom(2000), SOUND::LANDEN);

        if (mul != -0.6f)
            SoundManager.PlayWave(40, 128, 10000 + GetRandom(1000), SOUND::KLONG);

        ySpeed *= mul;

        for (int i = 0; i < 10; i++)
            PartikelSystem.PushPartikel(xPos + 10.0f + static_cast<float>(GetRandom(80)), yPos + 100.0f, SNOWFLUSH);

        if (ySpeed > -1.0f) {
            ySpeed = 0.0f;
            yAcc = 0.0f;
        }
    }
}

// --------------------------------------------------------------------------------------
// Rundherum schiessen
// single: true  = einzelschuss rundherum
//		   false = alle gleichzeitig
// --------------------------------------------------------------------------------------

void GegnerRollmops::RoundShot(bool single) {
    static float WinkelCount = 0;

    if (single) {
        WinkelUebergabe = WinkelCount;
        // DKS - support sin/cos lookup table & deg/rad versions of sin/cos
        /* Projectiles.PushProjectile(xPos + 50 + (float)sin(WinkelCount / 180.0f * PI) * 40.0f,
                                     yPos + 50 - (float)cos(WinkelCount / 180.0f * PI) * 40.0f, EISZAPFENSHOT); */
        Projectiles.PushProjectile(xPos + 50.0f + sin_deg(WinkelCount) * 40.0f,
                                   yPos + 50.0f - cos_deg(WinkelCount) * 40.0f, EISZAPFENSHOT);
        WinkelCount += 30;

        if (WinkelCount >= 360)
            WinkelCount = 0;
    } else
        for (WinkelCount = static_cast<float>(GetRandom(30)); WinkelCount < 360.0f; WinkelCount += 30) {
            WinkelUebergabe = WinkelCount;
            // DKS - support sin/cos lookup table & deg/rad versions of sin/cos
            /* Projectiles.PushProjectile(xPos + 50 + (float)sin(WinkelCount / 180.0f * PI) * 50.0f,
                                         yPos + 50 - (float)cos(WinkelCount / 180.0f * PI) * 50.0f, EISZAPFENSHOT); */
            Projectiles.PushProjectile(xPos + 50.0f + sin_deg(WinkelCount) * 50.0f,
                                       yPos + 50.0f - cos_deg(WinkelCount) * 50.0f, EISZAPFENSHOT);
        }

    SoundManager.PlayWave(50, 128, 16000 + GetRandom(2000), SOUND::STONEFALL);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerRollmops::DoKI() {
    // Energie anzeigen
    if (Handlung != GEGNER::INIT && Handlung != GEGNER::EXPLODIEREN)
        HUD.ShowBossHUD(4000, Energy);

    // Levelausschnitt auf den Mops zentrieren, sobald dieser sichtbar wird
    if (TileEngine.Zustand == TileStateEnum::SCROLLBAR) {
        TileEngine.ScrollLevel(static_cast<float>(Value1), static_cast<float>(Value2), TileStateEnum::SCROLLTOLOCK);

        // Mops aus Screen bringen
        xPos = Value1 + RENDERWIDTH;
        // yPos = Value2 + 100.0f;
        SoundManager.FadeSong(MUSIC::STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
    }

    // Boss blinkt nicht so lange wie die restlichen Gegner
    if (DamageTaken > 0.0f)
        DamageTaken -= Timer.sync(100.0f);  // Rotwerden langsam ausfaden lassen
    else
        DamageTaken = 0.0f;  // oder ganz anhalten

    if (Delay > 0.0f) {
        Delay -= Timer.sync(1.0f);
    } else
        // Je nach Handlung richtig verhalten
        switch (Handlung) {
            // warten bis Kamera am richtigen Fleck ist, dann reinhopsen lassen
            case GEGNER::EINFLIEGEN: {
                // Kamera an der richtigen Position?
                if (TileEngine.Zustand == TileStateEnum::LOCKED) {
                    // dann reinhopsen
                    xSpeed = -10.0f;
                    yAcc = 4.0f;
                    Handlung = GEGNER::INIT;
                    AnimEnde = 19;
                    AnimSpeed = 0.25f;
                    //				NeueAktion();

                    // Zwischenboss-Musik abspielen, sofern diese noch nicht gespielt wird
                    // DKS - Added function SongIsPlaying() to SoundManagerClass:
                    if (!SoundManager.SongIsPlaying(MUSIC::BOSS))
                        SoundManager.PlaySong(MUSIC::BOSS, false);
                }
            } break;

            // Schneekugel hopst rein
            case GEGNER::INIT: {
                SimpleAnimation();

                AnimSpeed += Timer.sync(0.02f);

                Abhopsen();

                // ausrollen
                if (ySpeed == 0.0f) {
                    xAcc = 1.0f;
                    AnimSpeed += Timer.sync(0.02f);

                    // ausgerollt? Dann kette ausfahren
                    if (xSpeed >= 0.0f) {
                        xAcc = 0.0f;
                        Handlung = GEGNER::STEHEN;
                        HookX = xPos + 65 - 9;
                        HookY = yPos;
                        Schwung = 0.0f;
                        SchwungDir = 0.0f;
                        ShotDelay = 5.0f;
                    }
                }
            } break;

            // Haken nach oben schiessen
            case GEGNER::AUSSPUCKEN: {
                static bool shot = false;

                if (AnimCount > 0.0f)
                    AnimCount -= Timer.sync(1.0f);

                HookY -= Timer.sync(40.0f);

                if (HookY < Value2 + 20) {
                    // evtl schiessen
                    if (yPos < Value2 + 130 && !shot) {
                        RoundShot(false);
                        shot = true;
                    }

                    HookY = static_cast<float>(Value2) + 20;

                    // DKS - This was already commented out in original source:
                    // xPos = pPlayer->xpos;

                    xPos = static_cast<float>(Value1) + 320.0f - 65.0f + sin(Schwung) * 300.0f;
                    yPos = static_cast<float>(Value2) - 500.0f + cos(Schwung * 0.75f) * 830.0f;

                    SchwungDir += Timer.sync(0.16f);
                    if (SchwungDir > TWO_PI)
                        SchwungDir = 0.0f;

                    Schwung = sin(SchwungDir);

                    AnimPhase = -static_cast<int>(((static_cast<float>(Value1 + RENDERWIDTH)) - xPos) / 200.0f);

                    if (AnimPhase < 0)
                        AnimPhase += 10;

                    if (((SchwungDir > PI - 0.2f && SchwungDir < PI + 0.2f) ||

                         (SchwungDir > TWO_PI - 0.2f && SchwungDir < TWO_PI + 0.2f)) &&
                        GetRandom(2) == 0) {
                        shot = false;
                        PartikelSystem.PushPartikel(xPos + 60.0f + static_cast<float>(GetRandom(20)),
                                                    yPos + 120.0f, LONGFUNKE);

                        PartikelSystem.PushPartikel(xPos + 60.0f + static_cast<float>(GetRandom(20)),
                                                    yPos + 100.0f, FUNKE);

                        // DKS - Added function WaveIsPlaying() to SoundManagerClass:
                        if (!SoundManager.WaveIsPlaying(SOUND::KLONG))
                            SoundManager.PlayWave(50, 128, 14000 + GetRandom(2000), SOUND::KLONG);
                    }
                }

                // abgeschossen?
                if (Energy < 2000.0f) {
                    Handlung = GEGNER::WARTEN;
                    ySpeed = -20.0f;
                    yAcc = 6.0f;
                    AnimSpeed = 10.0f;

                    // Kette fliegt weg
                    for (int i = 0; i < NUM_KETTENGLIEDER; i++)
                        pKettenTeile[i]->Handlung = GEGNER::SPECIAL;
                }
            } break;

            case GEGNER::LAUFEN_LINKS:
            case GEGNER::LAUFEN_LINKS2: {
                // Speed begrenzen
                if (xSpeed < -60.0f)
                    xSpeed = -60.0f;

                // Grenze überschritten, ab der abgebremst wird?
                if (xPos + 65.0f < static_cast<float>(Value1 + 135))
                    xAcc = 19.0f;

                // in der Mitte stoppen und abspringen
                if (Handlung == GEGNER::LAUFEN_LINKS2 && ySpeed == 0.0f) {
                    if (xPos + 65.0f < static_cast<float>(Value1 + 380))
                        xAcc = 23.0f;

                    if (xSpeed > 0.0f) {
                        xSpeed = 0.0f;
                        ySpeed = 0.0f;
                        xAcc = 0.0f;
                        yAcc = 10.0f;
                        Handlung = GEGNER::SPECIAL2;
                        ShotCount = 2.0f;
                        ySpeed = -77.0f;
                        break;
                    }
                }

                // Animphase anpassen
                AnimCount -= Timer.sync(xSpeed * 0.1f);

                if (AnimCount > AnimSpeed) {
                    AnimCount = 0.0f;
                    AnimPhase++;
                }

                if (AnimPhase > AnimEnde)
                    AnimPhase = 0;

                // Richtung umdrehen?
                if (xSpeed > 0.0f) {
                    Handlung = GEGNER::LAUFEN_RECHTS;
                    ShotDelay -= 1.0f;

                    // abspringen?
                    if (static_cast<int>(ShotDelay) % 3 == 0) {
                        xSpeed = 12.0f;
                        ySpeed = -60.0f;
                        yAcc = 15.0f;
                    }

                    if (ShotDelay <= 0.0f) {
                        GunAusfahren(true);
                    }
                }

                Abhopsen(-0.4f);

            } break;

            case GEGNER::LAUFEN_RECHTS:
            case GEGNER::LAUFEN_RECHTS2: {
                // Speed begrenzen
                if (xSpeed > 60.0f)
                    xSpeed = 60.0f;

                // Grenze überschritten, ab der abgebremst wird?
                if (xPos + 65.0f > static_cast<float>(Value1 + 505))
                    xAcc = -19.0f;

                // in der Mitte stoppen und abspringen
                if (Handlung == GEGNER::LAUFEN_RECHTS2 && ySpeed == 0.0f) {
                    if (xPos + 65.0f > static_cast<float>(Value1 + 300))
                        xAcc = -23.0f;

                    if (xSpeed < 0.0f) {
                        xSpeed = 0.0f;
                        ySpeed = 0.0f;
                        xAcc = 0.0f;
                        yAcc = 8.0f;
                        Handlung = GEGNER::SPECIAL3;
                        ShotCount = 2.0f;
                        ySpeed = -65.0f;
                        break;
                    }
                }

                // Animphase anpassen
                AnimCount += Timer.sync(xSpeed * 0.1f);

                if (AnimCount > AnimSpeed) {
                    AnimCount = 0.0f;
                    AnimPhase--;
                }

                if (AnimPhase < 0)
                    AnimPhase = AnimEnde;

                // Richtung umdrehen?
                if (xSpeed < 0.0f) {
                    Handlung = GEGNER::LAUFEN_LINKS;
                    ShotDelay -= 1.0f;

                    // abspringen?
                    if (static_cast<int>(ShotDelay) % 3 == 0) {
                        xSpeed = -16.0f;
                        ySpeed = -60.0f;
                        yAcc = 12.0f;
                    }

                    if (ShotDelay <= 0.0f) {
                        GunAusfahren(true);
                    }
                }

                Abhopsen(-0.4f);

            } break;

            case GEGNER::STEHEN: {
                ShotDelay -= Timer.sync(1.0f);

                if (ShotDelay <= 0.0f) {
                    ShotDelay = 0.0f;
                    Handlung = GEGNER::AUSSPUCKEN;
                    AnimPhase = 0;
                    AnimStart = 0;
                    AnimEnde = 10;
                    AnimSpeed = 0.5f;
                    //				xSpeed = -5.0f;

                    SoundManager.PlayWave(100, 128, 7000, SOUND::LANDEN);
                    SoundManager.PlayWave(100, 128, 9000, SOUND::LANDEN);

                    SoundManager.PlayWave(80, 128, 11025, SOUND::STEAM);

                    int i;
                    for (i = 0; i < 30; i++)
                        PartikelSystem.PushPartikel(xPos + 10.0f + static_cast<float>(GetRandom(80)),
                                                    yPos + 10.0f + static_cast<float>(GetRandom(80)), SNOWFLUSH);

                    for (i = 0; i < 8; i++)
                        Projectiles.PushProjectile(xPos + 10.0f + static_cast<float>(GetRandom(80)),
                                                   yPos + 10.0f + static_cast<float>(GetRandom(80)), SNOWBOMBSMALL);
                }
            } break;

            case GEGNER::LAUFEN: {
                Abhopsen(-0.4f);
                SimpleAnimation();
            } break;

            case GEGNER::WARTEN: {
                Abhopsen(-0.4f);

                if (ySpeed == 0.0f)
                    AnimSpeed -= Timer.sync(1.0f);

                if (AnimSpeed <= 0.0f) {
                    ShotDelay = 5.0f;

                    Handlung = GEGNER::LAUFEN_LINKS;

                    xSpeed = -60.0f;
                    xAcc = 0.0f;
                    AnimPhase = 0;
                    AnimEnde = 9;
                    AnimStart = 0;
                    AnimSpeed = 1.0f;
                }
            } break;

            //
            case GEGNER::OEFFNEN:
            case GEGNER::SCHLIESSEN: {
                bool fertig = false;

                // auf/zuklappen
                AnimCount += Timer.sync(1.0f);

                if (AnimCount > AnimSpeed) {
                    AnimCount = 0.0f;

                    if (Handlung == GEGNER::OEFFNEN) {
                        AnimPhase++;

                        if (AnimPhase >= AnimEnde - 1) {
                            fertig = true;
                            Delay = 10.0f;
                            GunWinkel = 0.0f;
                        }
                    } else {
                        AnimPhase--;

                        if (AnimPhase <= AnimEnde)
                            fertig = true;
                    }
                }

                if (fertig) {
                    AnimPhase = AnimEnde;

                    // schiessen?
                    if (Handlung == GEGNER::OEFFNEN) {
                        ShotCount = static_cast<float>(GetRandom(15) + 15);
                        Handlung = GEGNER::VERFOLGEN;
                        Destroyable = true;
                    }
                    // wieder rollen
                    else {
                        Destroyable = false;

                        ShotDelay = 5.0f;
                        xAcc = 0.0f;
                        AnimPhase = 0;
                        AnimEnde = 9;
                        AnimStart = 0;
                        AnimSpeed = 1.0f;

                        if (xPos > Value1 + 320.0f) {
                            if (Energy < 1000.0f)
                                Handlung = GEGNER::LAUFEN_LINKS2;
                            else
                                Handlung = GEGNER::LAUFEN_LINKS;

                            xSpeed = -40.0f;
                        } else {
                            if (Energy < 1000.0f)
                                Handlung = GEGNER::LAUFEN_RECHTS2;
                            else
                                Handlung = GEGNER::LAUFEN_RECHTS;

                            xSpeed = 40.0f;
                        }
                    }
                }
            } break;

            // Mit der Kanone zielen
            case GEGNER::VERFOLGEN: {
                CalcGunWinkel();

                // schiessen
                ShotDelay -= Timer.sync(1.0f);
                if (ShotDelay < 0.0f) {
                    SoundManager.PlayWave(50, 128, 16000 + GetRandom(2000), SOUND::STONEFALL);

                    float xoff = 0.0f;

                    if (xPos > Value1 + 320.0f)
                        WinkelUebergabe = 270.0f - GunWinkel;
                    else {
                        WinkelUebergabe = GunWinkel + 90.0f;
                        xoff = -70.0f;
                    }

                    // DKS - support sin/cos lookup table & deg/rad versions of sin/cos
                    /*
                    PartikelSystem.PushPartikel(xPos + xoff + 85 + (float)sin(WinkelUebergabe / 180.0f * PI) * 105.0f,
                                                  yPos + 46 - (float)cos(WinkelUebergabe / 180.0f * PI) * 90.0f,
                    SNOWFLUSH); Projectiles.PushProjectile(xPos + xoff + 90 + (float)sin(WinkelUebergabe / 180.0f * PI)
                    * 105.0f, yPos + 46 - (float)cos(WinkelUebergabe / 180.0f * PI) * 90.0f, EISZAPFENSHOT);
                    */
                    PartikelSystem.PushPartikel(xPos + xoff + 85.0f + sin_deg(WinkelUebergabe) * 105.0f,
                                                yPos + 46.0f - cos_deg(WinkelUebergabe) * 90.0f, SNOWFLUSH);
                    Projectiles.PushProjectile(xPos + xoff + 90.0f + sin_deg(WinkelUebergabe) * 105.0f,
                                               yPos + 46.0f - cos_deg(WinkelUebergabe) * 90.0f, EISZAPFENSHOT);

                    ShotDelay = 3.0f;
                    ShotCount -= 1.0f;

                    if (ShotCount <= 0.0f || (HitsToTake - Energy) > 100.0f)
                        GunAusfahren(false);
                }
            } break;

            case GEGNER::SPECIAL2: {
                Abhopsen(-0.3f);

                if (ySpeed == 0.0f) {
                    ShotDelay = 3.0f;
                    Handlung = GEGNER::LAUFEN_LINKS;
                    xSpeed = -30.0f;
                    xAcc = -10.0f;
                }

                if (yPos < Value2 + 260.0f) {
                    ShotCount -= Timer.sync(1.0f);

                    if (ShotCount <= 0.0f) {
                        RoundShot(false);
                        ShotCount = 4.0f;
                    }
                }
            } break;

            // hochspringen, kreiseln und in alle Richtungen schiessen
            case GEGNER::SPECIAL3: {
                Abhopsen(-0.3f);

                if (ySpeed == 0.0f) {
                    ShotDelay = 3.0f;
                    Handlung = GEGNER::LAUFEN_RECHTS;
                    xSpeed = 30.0f;
                    xAcc = 10.0f;
                }

                if (yPos < Value2 + 250.0f) {
                    ShotCount -= Timer.sync(1.0f);

                    if (ShotCount <= 0.0f) {
                        RoundShot(true);
                        ShotCount = 0.5f;
                    }
                }
            } break;

            case GEGNER::EXPLODIEREN: {
                Abhopsen(-0.3f);

                // animieren
                AnimCount += Timer.sync(1.0f);

                while (AnimCount > AnimSpeed) {
                    SoundManager.PlayWave(100, 128, 8000 + GetRandom(4000), SOUND::EXPLOSION3);

                    PartikelSystem.PushPartikel(xPos - 20.0f + static_cast<float>(GetRandom(140)),
                                                yPos - 20.0f + static_cast<float>(GetRandom(140)),
                                                EXPLOSION_MEDIUM2 + GetRandom(1));

                    if (GetRandom(3) == 0)
                        PartikelSystem.PushPartikel(xPos + static_cast<float>(GetRandom(100)),
                                                    yPos + static_cast<float>(GetRandom(100)), SPLITTER);

                    AnimCount -= AnimSpeed;
                    AnimSpeed -= 0.01f;

                    AnimPhase++;

                    if (AnimPhase > AnimEnde)
                        AnimPhase = 0;

                    if (AnimSpeed <= 0.1f)
                        Energy = -500.0f;
                }
            } break;

            default:
                break;
        }  // switch

    // Schnee beim Rollen?
    if (Handlung == GEGNER::LAUFEN_RECHTS || Handlung == GEGNER::LAUFEN_LINKS || Handlung == GEGNER::LAUFEN ||
        Handlung == GEGNER::SPECIAL) {
        if (ySpeed == 0.0f)
            SmokeCount -= Timer.sync(1.0f);

        if (SmokeCount < 0.0f) {
            PartikelSystem.PushPartikel(xPos + 10.0f + static_cast<float>(GetRandom(80)),
                                        yPos + 80.0f + static_cast<float>(GetRandom(10)), SNOWFLUSH);
            SmokeCount = 0.8f;
        }
    }

    // Testen, ob der Spieler den Boss berührt hat
    GegnerRect[GegnerArt].top = 20;
    GegnerRect[GegnerArt].left = 20;
    GegnerRect[GegnerArt].right = 80;
    GegnerRect[GegnerArt].bottom = 80;
    TestDamagePlayers(Timer.sync(20.0f));

    // Keine Energie mehr? Dann explodieren
    if (Energy <= 0.0f && Energy > -500.0f && Handlung != GEGNER::EXPLODIEREN) {
        Handlung = GEGNER::EXPLODIEREN;
        xSpeed = 0.0f;
        ySpeed = 0.0f;
        xAcc = 0.0f;

        Energy = 1.0f;
        Destroyable = false;
        AnimSpeed = 1.0f;
        ShotDelay = 1.5f;

        AnimPhase = 0;
        AnimEnde = 9;
        AnimStart = 0;

        // Endboss-Musik ausfaden und abschalten
        SoundManager.FadeSong(MUSIC::BOSS, -2.0f, 0, false);
    }
}

// --------------------------------------------------------------------------------------
// Rollmops explodiert
// --------------------------------------------------------------------------------------

void GegnerRollmops::GegnerExplode() {
    SoundManager.PlayWave(100, 128, 8000 + GetRandom(4000), SOUND::EXPLOSION2);

    for (int i = 0; i < 5; i++)
        PartikelSystem.PushPartikel(xPos - 20.0f + static_cast<float>(GetRandom(140)),
                                    yPos - 20.0f + static_cast<float>(GetRandom(140)), EXPLOSION_TRACE);

    for (int i = 0; i < 150; i++)
        PartikelSystem.PushPartikel(xPos - 20.0f + static_cast<float>(GetRandom(140)),
                                    yPos - 20.0f + static_cast<float>(GetRandom(140)), WATERFLUSH_HIGH);

    ShakeScreen(5.0f);

    Player[0].Score += 5000;

    ScrolltoPlayeAfterBoss();
}
