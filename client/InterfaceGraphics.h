/* InterfaceGraphics — C port of client/InterfaceGraphics.pas */

#ifndef INTERFACEGRAPHICS_H
#define INTERFACEGRAPHICS_H

#include "Vector.h"
#include "Sprites.h"
#include "Constants.h"

typedef struct TInterfaceRelInfo {
    int HealthBar_Rel_X, HealthBar_Rel_Y;
    int JetBar_Rel_X, JetBar_Rel_Y;
    int AmmoBar_Rel_X, AmmoBar_Rel_Y;
    int FireBar_Rel_X, FireBar_Rel_Y;
    int NadesBar_Rel_X, NadesBar_Rel_Y;
} TInterfaceRelInfo;

typedef struct TInterface {
    unsigned char Alpha;
    bool Health, Ammo, Vest, Jet, Nades, Bullets, Weapon, Fire, Team, Ping, Status;
    int HealthIco_X, HealthIco_Y;
    int HealthIco_Rotate;
    int HealthBar_X, HealthBar_Y;
    int HealthBar_Width, HealthBar_Height;
    unsigned char HealthBar_Pos;
    int HealthBar_Rotate;
    int AmmoIco_X, AmmoIco_Y;
    int AmmoIco_Rotate;
    int AmmoBar_X, AmmoBar_Y;
    int AmmoBar_Width, AmmoBar_Height;
    unsigned char AmmoBar_Pos;
    int AmmoBar_Rotate;
    int JetIco_X, JetIco_Y;
    int JetIco_Rotate;
    int JetBar_X, JetBar_Y;
    int JetBar_Width, JetBar_Height;
    unsigned char JetBar_Pos;
    int JetBar_Rotate;
    int VestBar_X, VestBar_Y;
    int VestBar_Width, VestBar_Height;
    unsigned char VestBar_Pos;
    int VestBar_Rotate;
    int Nades_X, Nades_Y;
    int Nades_Width, Nades_Height;
    unsigned char Nades_Pos;
    int Bullets_X, Bullets_Y;
    int Weapon_X, Weapon_Y;
    int FireIco_X, FireIco_Y;
    int FireIco_Rotate;
    int FireBar_X, FireBar_Y;
    int FireBar_Width, FireBar_Height;
    unsigned char FireBar_Pos;
    int FireBar_Rotate;
    int TeamBox_X, TeamBox_Y;
    int Ping_X, Ping_Y;
    int Status_X, Status_Y;
} TInterface;
typedef TInterface *PInterface;

extern char *ChatMessage[MAX_SPRITES + 1];
extern bool ChatTeam[MAX_SPRITES + 1];
extern int ChatDelay[MAX_SPRITES + 1];

extern char *BigText[MAX_BIG_MESSAGES + 1];
extern int BigDelay[MAX_BIG_MESSAGES + 1];
extern int BigX[MAX_BIG_MESSAGES + 1];
extern float BigScale[MAX_BIG_MESSAGES + 1];
extern unsigned int BigColor[MAX_BIG_MESSAGES + 1];
extern float BigPosX[MAX_BIG_MESSAGES + 1], BigPosY[MAX_BIG_MESSAGES + 1];

extern char *WorldText[MAX_BIG_MESSAGES + 1];
extern int WorldDelay[MAX_BIG_MESSAGES + 1];
extern int WorldX[MAX_BIG_MESSAGES + 1];
extern float WorldScale[MAX_BIG_MESSAGES + 1];
extern long WorldColor[MAX_BIG_MESSAGES + 1];
extern float WorldPosX[MAX_BIG_MESSAGES + 1], WorldPosY[MAX_BIG_MESSAGES + 1];

extern char *CursorText;
extern int CursorTextLength;
extern bool CursorFriendly;

extern bool FragsMenuShow, StatsMenuShow, ConInfoShow, PlayerNamesShow, MiniMapShow, NoobShow;

extern unsigned char FragsScrollMax;
extern unsigned char FragsScrollLev;
extern TVector2 _RScala;
extern TVector2 _IScala;
extern int fragx, fragy;

extern TInterface Int;
extern TInterfaceRelInfo relinfo;
extern TVector2 PixelSize;
extern double ChatInputTime;

typedef struct TInterfaceAlign {
    unsigned char Weapon;
    unsigned char Bullets;
    unsigned char HealthBar;
    unsigned char AmmoBar;
    unsigned char ReloadBar;
    unsigned char FireBar;
    unsigned char JetBar;
    unsigned char VestBar;
} TInterfaceAlign;

extern TInterfaceAlign IntAlign;

void InterfaceGraphics_LoadInterfaceArchives(const char *path, bool first_only);
void InterfaceGraphics_LoadInterfaceArchivesPath(const char *path);
bool InterfaceGraphics_LoadInterfaceData(const char *interface_name);
void InterfaceGraphics_RenderInterface(float time_elapsed, float width, float height);
void InterfaceGraphics_RenderActionSnapText(double t);
bool InterfaceGraphics_IsDefaultInterface(const char *interface_name);

#endif /* INTERFACEGRAPHICS_H */