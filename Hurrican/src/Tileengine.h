// Datei : Tileengine.h

// --------------------------------------------------------------------------------------
//
// 2D Tile-Engine f�r Hurrican
// bestehend aus einem Vordergrund-Layer in verschiedenen Helligkeitsstufen
//
// (c) 2002 J�rg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _TileEngine_h_
#define _TileEngine_h_

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#if defined(PLATFORM_DIRECTX)
#include <d3dx8.h>
#endif
#include "DX8Graphics.h"
#include "DX8Sprite.h"
//#include "lightmap.h"     //DKS - all light-map related code disabled
#include "CDragonHack.h"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

//----- Flags f�r den Blockwert

#define BLOCKWERT_WAND				1		// Solide Wand
#define BLOCKWERT_GEGNERWAND		2		// Wand nur f�r Gegner
#define BLOCKWERT_PLATTFORM			4		// Plattform
#define BLOCKWERT_LIGHT				8		// Licht bei Objekten ver�ndern
#define BLOCKWERT_VERDECKEN			16		// Spieler und Objekte verdecken
#define BLOCKWERT_ANIMIERT_BACK		32		// Animiert Hintergrund
#define BLOCKWERT_ANIMIERT_FRONT	64		// Animiert Overlay
#define BLOCKWERT_WASSER			128		// Wasser (macht platsch :D )
#define BLOCKWERT_SCHADEN			256		// Schaden
#define BLOCKWERT_FLIESSBANDL		512		// Fliessband Links
#define BLOCKWERT_FLIESSBANDR		1024	// Fliessband Rechts
#define BLOCKWERT_WENDEPUNKT		2048	// Bewegte Plattformen umdrehen lassen
#define BLOCKWERT_DESTRUCTIBLE		4096	// Zerst�rbare Wand
#define BLOCKWERT_MOVELINKS			8192	// Textur nach links bewegen
#define BLOCKWERT_OVERLAY_LIGHT	   16384	// Overlay nimmt Licht an
#define BLOCKWERT_SUMPF			   32768	// Einsinken
#define BLOCKWERT_EIS			   65536	// Eis, auf dem man ausrutscht
#define BLOCKWERT_MOVEVERTICAL	  131072	// Vertikale Texturbewegung
#define BLOCKWERT_WASSERFALL	  262144	// Wasserfall
#define BLOCKWERT_MOVERECHTS	  524288	// Textur nach rechts bewegen

#define BLOCKWERT_SCHRAEGE_L	 2097152	// Schr�ge Rechts
#define BLOCKWERT_SCHRAEGE_R	 4194304	// Schr�ge Rechts
#define BLOCKWERT_LIQUID		 8388608	// Fl�ssigkeit (Wasser, S�ure, Lava, Magens�ure)

//--- Animationsgeschwindigkeit der animierten Level-Tiles

#define TILEANIM_SPEED      0.8f

//--- Werte zur Levelgr�sse

#define	TILESIZE_X			20						// Gr�sse eines
#define	TILESIZE_Y			20						// einzelnen Tiles
#define TILESETSIZE_X		256.0f					// Gr�sse eines
#define TILESETSIZE_Y		256.0f					// Tilesets

#define SCREENSIZE_X		32						// Leveltiles pro Screen
#define SCREENSIZE_Y		24

#define MAX_LEVELSIZE_X		1024					// Gesamtgr�sse des Level
#define MAX_LEVELSIZE_Y		1600

#define MAX_TILESETS		64						// Maximalzahl der Tilesets
#define INCLUDE_ZEROTILE	1						// Tile 0,0 im Tileset mit verwenden ?

//----- Gr�sse des nicht scrollbaren Bereichs

#define	SCROLL_BORDER_EXTREME_LEFT		0
#define	SCROLL_BORDER_EXTREME_TOP		0
#define	SCROLL_BORDER_EXTREME_RIGHT		570
#define	SCROLL_BORDER_EXTREME_BOTTOM	400

#define	SCROLL_BORDER_HORIZ		0
#define	SCROLL_BORDER_TOP		50
#define	SCROLL_BORDER_BOTTOM	50


#define	LOOK_BORDER_TOP			60
#define	LOOK_BORDER_BOTTOM		400

//--- Zust�nde der Tileengine

#define	ZUSTAND_SCROLLBAR		0		// Spieler kann Level scrollen durch laufen
#define ZUSTAND_SCROLLTO		1		// Level Scrollt zu bestimmter Position
#define ZUSTAND_SCROLLTOLOCK	2		// Level Scrollt zu bestimmter Position und lockt dann
#define ZUSTAND_SCROLLTOPLAYER	4		// Level Scrollt zum Spieler
#define ZUSTAND_LOCKED			5		// Level sitzt an bestimmter Position fest

//--- LiightMaps
//DKS - all light-map related code disabled:
//#define MAX_LIGHTMAPS			5
//
//#define LIGHTMAP_BLITZ			0
//#define LIGHTMAP_EXPLOSION		1
//#define LIGHTMAP_GOLEMSHOT		2
//#define LIGHTMAP_LILA			3

// --------------------------------------------------------------------------------------
// Strukturen
// --------------------------------------------------------------------------------------

// Struktur f�r ein Level Tile wie es aus dem Level geladen wird
//
//DKS - Made binary sizes of member vars explicit here to make loading/saving binary files more robust:
struct LevelTileLoadStruct
{
    uint8_t  TileSetBack;              // Back  aus welchem Tileset ?
    uint8_t  TileSetFront;             // Front aus welchem Tileset ?
    uint8_t  BackArt;                  // Tile im Hintergrund
    uint8_t  FrontArt;                 // Tile im Vordergrund
    uint8_t  Red, Green, Blue, Alpha;  // Farbwert des Tiles
    uint32_t Block;                    // Blockierungsart (siehe #defines)
};

// Struktur f�r die Farbwerte von einem Level Tile
//
struct LevelTileSaveStruct
{
    D3DCOLOR		Color[4];							// Farbwert des Tiles (Alle vier Ecken)
};


// Struktur f�r ein Level Tile wie es im Level vorkommt (wie beim Laden, nur noch mit Extra Farben f�r alle Ecken)
//
struct LevelTileStruct
{
    unsigned char	TileSetBack;						// Back  aus welchem Tileset ?
    unsigned char	TileSetFront;						// Front aus welchem Tileset ?
    unsigned char   BackArt;							// Tile im Hintergrund
    unsigned char	FrontArt;							// Tile im Vordergrund
    unsigned char	Red, Green, Blue, Alpha;			// Farbwert des Tiles
    D3DCOLOR		Color[4];							// Farbwert des Tiles (Alle vier Ecken)
    unsigned int	Block;								// Blockierungsart (siehe #defines)
    bool			move_v1, move_v2, move_v3, move_v4; // Die Ecken eines Tiles bei der Wasseranim bewegen?
};

// --------------------------------------------------------------------------------------
// Struktur f�r ein aus dem Level zu ladendes Objekte
// --------------------------------------------------------------------------------------

//DKS - Made binary sizes of member vars and padding bytes explicit here to make
//      loading/saving binary files more robust:
struct LevelObjectStruct
{
    uint32_t ObjectID;               // Welche Objekt ID ?
    int32_t  XPos;                   // x-Position
    int32_t  YPos;                   // y-Position
    uint8_t  ChangeLight;            // Umgebungslicht annehmen ?
    uint8_t  Skill;                  // 0 = Easy, 1 = Medium, 2 = Hard
    uint8_t  PADDING_CHUNK_1[2];     // 2 PADDING BYTES
    int32_t  Value1;                 // Werte f�r diverse Trigger
    int32_t  Value2;                 // Werte f�r diverse Trigger
};

// --------------------------------------------------------------------------------------
// Level-Datei Header
// --------------------------------------------------------------------------------------

//DKS - Made binary sizes of member vars and padding bytes explicit here to make
//      loading/saving binary files more robust:
struct FileHeader
{
    char            Kennung[46];                    // Level-Kennung
    char            Beschreibung[100];              // Level-Beschreibung
    char            BackgroundFile[24];             // Dateiname des Hintergrundes
    char            ParallaxAFile [24];             // Dateiname des 1. Parallax Layers
    char            ParallaxBFile [24];             // Dateiname des 2. Parallax Layers
    char            CloudFile     [24];             // Dateiname des WolkenLayers
    uint8_t         PADDING_CHUNK_1[2];             // 2 PADDING  BYTES
    uint32_t        Timelimit;                      // aktuelles Zeitlimit des Levels
    uint8_t         UsedTilesets;                   // Anzahl der Tilesets
    char            SetNames[64][16];               // Namen der benutzten Sets
    uint8_t         PADDING_CHUNK_2[3];             // 3 PADDING BYTES
    uint32_t        SizeX, SizeY;                   // Gr��e des Levels
    uint32_t        NumObjects;                     // Anzahl der Objekte
    uint8_t         ScrollBackground;               // Hintergrundbild srollen oder statisch ?
    uint8_t         PADDING_CHUNK_3[3];             // 3 PADDING BYTES
};

// --------------------------------------------------------------------------------------
// Anhang am File nach den Level Daten
// damit nicht alle levels nochmal konvertiert werden, h�nge ich einfach alle dinge, die
// noch ins level format reinm�ssen, dahinter =)
// --------------------------------------------------------------------------------------

//DKS - Made binary sizes of member vars and padding bytes explicit here to make
//      loading/saving binary files more robust:
struct FileAppendix
{
    char    Songs[2][30];             // Namen der benutzten Songs (Stage und Boss)
    int32_t UsedPowerblock;           // ID der benutzten Powerblock Art
    char    Col1[8], Col2[8];         // Farben f�r Liquid
    uint8_t Taschenlampe;
    uint8_t PADDING_CHUNK_1[3];       // 3 padding bytes
};

// --------------------------------------------------------------------------------------
// Infos f�r die Schwabbelverscheibung eines einzelnen Vertex
// --------------------------------------------------------------------------------------

struct SchwabbelPos
{
    float lo, ro, lu, ru;
};


// --------------------------------------------------------------------------------------
// Unions
// --------------------------------------------------------------------------------------

// Union f�r r,g,b,a Zugriff in D3DCOLOR

union FarbUnion
{
    D3DCOLOR color;

    struct
    {
        unsigned char b;
        unsigned char g;
        unsigned char r;
        unsigned char a;
    } farbStruct;
};

struct Vector2D
{
    float x;
    float y;
};

// --------------------------------------------------------------------------------------
// Klassendeklaration
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// TileEngine Klasse
// --------------------------------------------------------------------------------------

class TileEngineClass
{
private:
    float			TileAnimCount;							// Animations-Z�hler und
    float			CloudMovement;
    int				TileAnimPhase;							// Phase der Tile Animation
    float			ScrollSpeedX, ScrollSpeedY;				// Scrollspeed
    VERTEX2D		TilesToRender[40*30*6];				// Alle zu rendernden Leveltiles
    VERTEX2D		v1, v2, v3, v4;							// Vertices zum Sprite rendern
    unsigned char	LoadedTilesets;							// Anzahl geladener Sets
    float			WaterPos;								// Position in der WaterListe f�r die Wasseroberfl�che
    LevelTileStruct Tiles[MAX_LEVELSIZE_X]  // Array mit Leveldaten
                         [MAX_LEVELSIZE_Y];

    //DKS - Lightmap code in original game was never used and all related code has now been disabled:
    //CLightMap		lightmaps[MAX_LIGHTMAPS];

public:
    FileAppendix	DateiAppendix;							// Anhang der Level-Datei
    bool			IsElevatorLevel;
    bool			MustCenterPlayer;
    int				ColR1, ColG1, ColB1;					// Farben in RGB
    int				ColR2, ColG2, ColB2;
    int				ColR3, ColG3, ColB3, ColA3;

    //DKS - Disabled this, was only ever filled with zeroes and had no effect.
    //float			SinList[4096];							// Sinus Liste zum Schwabbeln des Alienlevels

    float			SinList2[4096];							// Sinus Liste zum Schwabbeln des Wasserhintergrunds
    float			WaterList[4096];						// Sinus Liste zum Schwabbeln der Oberfl�che
    RECT			TileRects[144];							// vorberechnete Tile Ausschnitte
    char			Beschreibung[100];						// Beschreibung des Levels
    int				Zustand;								// Aktueller Zustand
    float			ScrolltoX, ScrolltoY;					// Lock-Werte
    float			SpeedX, SpeedY;							// Speed for ScrollTo Funktion
    bool			bDrawShadow;							// Taschenlampen Shatten im Alien Level rendern?
    int				MaxBlocks;
    int				MaxOneUps;
    int				MaxSecrets;
    int				MaxDiamonds;

    //DKS - See above note regarding SinList: disabled SinPos (unused)
    //float			SinPos;									// Position in der SinusListe f�r das AlienLevel
    float			SinPos2;								// Position in der SinusListe f�r den Wasserhintergrund

    bool							bScrollBackground;		// Hintegrundbild scrollen ?

    //DKS - Lightmap code in original game was never used and all related code has now been disabled:
    //LevelTileSaveStruct				OriginalTiles[MAX_LEVELSIZE_X]  // Array mit Leveldaten merken (f�r Lightmaps)
    //[MAX_LEVELSIZE_Y];

    DirectGraphicsSprite			TileGfx[MAX_TILESETS];	// Tilegrafiken
    DirectGraphicsSprite			Background;				// Hintergrund
    DirectGraphicsSprite			ParallaxLayer[6];		// Anzahl der Layer
    DirectGraphicsSprite			CloudLayer;				// Wolkenlayer
    DirectGraphicsSprite			GameOver;				// GameOver Symbol
    DirectGraphicsSprite			Wasserfall[2];			// Wasserfall Grafiken
    DirectGraphicsSprite			LiquidGfx[2];				// Fl�ssigkeit
    float							WasserU[9];				// vorberechnete TexturKoordinaten f�r das Wasser TU
    float							WasserV[9];				// vorberechnete TexturKoordinaten f�r das Wasser TV
    DirectGraphicsSprite			Shadow;					// Schatten im Alien Level
    float							WasserfallOffset;		// Wasserfall Offset
    float							XOffset, YOffset;		// Scrolloffset des Levels
    float							NewXOffset, NewYOffset; // Neue Scrolloffsets, falls das Level von einem Gegner gelockt ist
    double							Timelimit;				// Zeitlimit
    double							TimelimitSave;			// uspr�ngliches Zeitlimit
    int								LEVELSIZE_X;			// Gr�sse des Levels
    int								LEVELSIZE_Y;			// in Tiles
    float							LEVELPIXELSIZE_X;		// Levelgr�sse in Pixeln
    float							LEVELPIXELSIZE_Y;		// (f�r XOffset und YOffset)

    CDragonHack	    *pDragonHack;


    // Vorberechnung f�rs Levelrendern
    int 							RenderPosX;				// Bereich des Levels, der gerendert wird
    int 							RenderPosY;
    int 							RenderPosXTo;
    int 							RenderPosYTo;
    int								xLevel;
    int								yLevel;
    int								xTileOffs;
    int								yTileOffs;
    float							xScreen;
    float							yScreen;

    TileEngineClass(void);									// Konstruktor
    ~TileEngineClass(void);									// Destruktor

    //DKS - Added initialization function that will load the sprites.
    //      This was necessary since making TileEngineClass a global
    //      static in Main.cpp instead of a dyanmically-allocated pointer.
    //      The class constructor therefore should never load sprites by
    //      itself, since graphics system should be initialized first.
    void LoadSprites();

    void ClearLevel(void);									// Level freigeben
    bool LoadLevel(char Filename[100]);						// Level laden
    void InitNewLevel(int xSize, int ySize);				// Neues Level initialisieren
    void SetScrollSpeed(float xSpeed, float ySpeed);		// neue Scrollspeed setzen
    void CalcRenderRange		(void);						// Bereiche berechnen, die gerendert werden sollen
    void DrawBackground			(void);							// Hintergrund Layer zeichnen
    void DrawBackLevel			(void);							// Level hintergrund anzeigen
    void DrawFrontLevel			(void);							// Level vordergrund anzeigen
    void DrawBackLevelOverlay	(void);							// Boden Tiles, die verdecken
    void DrawOverlayLevel		(void);							// Sonstige, die verdecken
    //DKS - this was an empty function in the original code, disabling it:
    //void DrawSpecialLayer		(void);							// Spezielle Sachen wie der Drache
    void DrawWater				(void);							// Wasser Planes rendern
    void CheckBounds			(void);
    void UpdateLevel			(void);							// Level evtl scrollen usw
    void ScrollLevel		(float x,  float y,  int neu,
                             float sx = 10.0f, float sy = 10.0f);// Screen scrollen
    int	 BlockSlopes		(float &x, float &y, float &xo, float &yo, RECT rect, float ySpeed, bool resolve = false);
    int	 BlockRechts		(float &x, float &y, float &xo, float &yo, RECT rect, bool resolve = false);
    int	 BlockLinks			(float &x, float &y, float &xo, float &yo, RECT rect, bool resolve = false);
    int	 BlockOben			(float &x, float &y, float &xo, float &yo, RECT rect, bool resolve = false);
    int	 BlockUnten			(float &x, float &y, float &xo, float &yo, RECT rect, bool resolve = false);
    int	 BlockUntenNormal	(float &x, float &y, float &xo, float &yo, RECT rect);

    void ResolveLinks		(float &x, float &y, float &xo, float &yo, RECT rect);

    bool BlockDestroyRechts		(float &x, float &y, float &xo, float &yo, RECT rect);
    bool BlockDestroyLinks		(float &x, float &y, float &xo, float &yo, RECT rect);
    bool BlockDestroyOben		(float &x, float &y, float &xo, float &yo, RECT rect);
    bool BlockDestroyUnten		(float &x, float &y, float &xo, float &yo, RECT rect);
    void MakeBordersLookCool	(int x, int y);

    //DKS - x,y parameters did not need to be references and are now value params:
    bool CheckDestroyableWalls(float x, float y,			// Schuss auf Zerst�rbare
                               float xs, float ys,			// W�nde testen
                               RECT rect);

    void ExplodeWall (int x, int y);						// Wand an x/y explodieren lassen
    void ExplodeWalls(int x, int y);						// Wand an x/y und alle angrenzenden W�nde
    // explodieren lassen

    D3DCOLOR LightValue(float x, float y, RECT rect, bool forced);	// Helligkeit an Stelle x/y

    void ComputeCoolLight (void);							// Coole   Lightberechnung

    //DKS - This function was never actually used in the original game and is now disabled:
    //      (Only place was as a console command in Console.cpp)
    //void ComputeShitLight (void);							// Billige Lightberechnung

    void DrawShadow		  (void);							// Schatten im Alien Level zeichnen


    //DKS - The original code had a return as line 5 of DrawLightmap, making it a stub.
    //      When I tried removing it, I saw why: the lightmaps colors' don't match what
    //      you'd expect and they appear in places off-center from where they should be.
    //      I have now disabled the function and all four calls to it elsewhere in the code,
    //      and all related code.
    //void ClearLightMaps	  (void);							// alle LightMaps entfernen und Ursprungshelligkeit wieder herstellen
    //void DrawLightmap	  (int Map, float x, float y, int alpha);	// LightMap an x/y mit alpha addieren

    void WertAngleichen	  (float &nachx, float &nachy, float vonx, float vony);
    bool LineHitsLine(const Vector2D p,
                      const Vector2D u,
                      const Vector2D q,
                      const Vector2D v,
                      Vector2D &pHit);
    
    //DKS - Added bounds-checked accessor for Tiles[][] array for debugging purposes:
    LevelTileStruct& TileAt(const int i, const int j)
    {
#ifdef _DEBUG
        if (i >= MAX_LEVELSIZE_X || i < 0 || j >= MAX_LEVELSIZE_Y || j < 0) {
            Protokoll.WriteText( true, "-> Error: Out of bounds in TileEngineClass::TileAt():\n"
                    "\tparam i: %d\tLower bound: %d\tUpper bound: %d\n"
                    "\tparam j: %d\tLower bound: %d\tUpper bound: %d\n",
                    i, 0, MAX_LEVELSIZE_X-1, j, 0, MAX_LEVELSIZE_Y-1 );
            exit(1);    //WriteText above should do this for us (first param==true)
        }
#endif
        return Tiles[i][j];
    }   
};

// --------------------------------------------------------------------------------------
// Externals
// --------------------------------------------------------------------------------------

extern	TileEngineClass		TileEngine;
extern	D3DCOLOR			Col1, Col2, Col3;
extern  bool				DrawDragon;
extern  float				ShadowAlpha;

#endif

