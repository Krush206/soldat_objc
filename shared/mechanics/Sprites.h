/* Sprites — C port of shared/mechanics/Sprites.pas
 * (interface contract). Pascal `object` types become C structs and
 * their methods become C functions named <Type>_<method>. */

#ifndef SPRITES_H
#define SPRITES_H

#include "Parts.h"
#include "Anims.h"
#include "MapFile.h"
#include "PolyMap.h"
#include "../network/Net.h"
#include "Weapons.h"
#include "Constants.h"
#include "Vector.h"

#define MAX_SPRITES MAX_PLAYERS
#define MAX_BULLETS 254
#define MAX_SPARKS 558
#define MAX_THINGS 90

#define SURFACECOEFX 0.970f
#define SURFACECOEFY 0.970f
#define CROUCHMOVESURFACECOEFX 0.850f
#define CROUCHMOVESURFACECOEFY 0.970f
#define STANDSURFACECOEFX 0.000f
#define STANDSURFACECOEFY 0.000f
#define GRENADE_SURFACECOEF 0.880f
#define SPARK_SURFACECOEF 0.700f

#define PART_RADIUS 7
#define FLAG_PART_RADIUS 10
#define SPRITE_RADIUS 16
#define M79GRENADE_EXPLOSION_RADIUS 64
#define FRAGGRENADE_EXPLOSION_RADIUS 85
#define AFTER_EXPLOSION_RADIUS 50
#define CLUSTERGRENADE_EXPLOSION_RADIUS 35
#define BASE_RADIUS 75
#define TOUCHDOWN_RADIUS 28
#define SPRITE_COL_RADIUS 3

#define FLAG_HOLDING_FORCEUP (-14)
#define FLAG_STAND_FORCEUP (-16)

#define BULLETALPHA 110
#define MAXPATHCOUNT 50

#define SLIDELIMIT 0.2f
#define MAX_VELOCITY 11
#define BULLETTIME_MINDISTANCE 320
#define FLAGTHROW_POWER 4.225f

#define NORMAL_DEATH 1
#define BRUTAL_DEATH 2
#define HEADCHOP_DEATH 3

#define POS_STAND 1
#define POS_CROUCH 2
#define POS_PRONE 4

typedef int TSpriteIndexes[MAX_SPRITES + 1];     /* Pascal 1..MAX_SPRITES */
typedef float TSpriteDistances[MAX_SPRITES + 1];

typedef struct TControl {
    bool Left, Right, Up, Down;
    bool Fire, Jetpack, ThrowNade, ChangeWeapon;
    bool ThrowWeapon, Reload, Prone, FlagThrow;
    short MouseAimX, MouseAimY, MouseDist;
} TControl;

typedef struct TBotData {
    int FavWeapon;
    char *Friend;
    int Accuracy, GrenadeFreq;
    unsigned char DeadKill;
    int WaypointTimeoutCounter, WaypointTimeout;
    int ChatFreq;
    char *ChatKill, *ChatDead, *ChatLowHealth, *ChatSeeEnemy, *ChatWinning;
    unsigned char PissedOff;
    unsigned char PathNum;
    unsigned char TargetNum;
    bool GoThing;
    int CurrentWaypoint, NextWaypoint, OldWaypoint, WaypointTime;
    int LastWaypoint;
    unsigned char Use;
    int OnePlaceCount;
    unsigned char Camper;
    int CampTimer;
    unsigned char FallSave;
} TBotData;

typedef struct TBackgroundState {
    unsigned char BackgroundStatus;
    short BackgroundPoly;
    bool BackgroundTestResult;
} TBackgroundState;

void TBackgroundState_BackgroundTest(TBackgroundState *self, unsigned short poly);
void TBackgroundState_BackgroundTestBigPolyCenter(TBackgroundState *self,
    TVector2 pos);
short TBackgroundState_BackgroundFindCurrentPoly(TBackgroundState *self,
    TVector2 pos);
void TBackgroundState_BackgroundTestPrepare(TBackgroundState *self);
void TBackgroundState_BackgroundTestReset(TBackgroundState *self);

typedef struct TSprite {
    bool Active, DeadMeat, Dummy;
    unsigned char Style;
    unsigned char Num;
    unsigned char Visible;
    bool OnGround, OnGroundForLaw;
    bool OnGroundLastFrame;
    bool OnGroundPermanent;
    short Direction, OldDirection;
    float Health;
    unsigned char HoldedThing;
    int FlagGrabCooldown;
    float AimDistCoef;
    unsigned char Fired;
    unsigned char Alpha;
    float JetsCountReal;
    short JetsCount, JetsCountPrev;
    unsigned char WearHelmet;
    unsigned char HasCigar;
    bool CanMercy;
    short RespawnCounter, CeaseFireCounter;
    unsigned char SelWeapon;
    int BonusStyle, BonusTime;
    int MultiKillTime, MultiKills;
    float Vest;
    int IdleTime;
    signed char IdleRandom;
    unsigned char BurstCount;
    unsigned char Position;
    unsigned char OnFire;
    unsigned char ColliderDistance;
    int DeadCollideCount;
    int DeadTime;
    unsigned char Para, Stat;
    short UseTime;
    bool HalfDead;
    float LastWeaponHM, LastWeaponSpeed;
    unsigned char LastWeaponStyle;
    unsigned short LastWeaponFire, LastWeaponReload;
    ParticleSystem Skeleton;
    TAnimation LegsAnimation, BodyAnimation;
    TControl Control;
    TGun *Weapon, *SecondaryWeapon, *TertiaryWeapon;
    bool GrenadeCanThrow;
    TBotData Brain;
    TPlayer *Player;
    bool IsPlayerObjectOwner;
    bool Typing;
    bool AutoReloadWhenCanFire;
    bool CanAutoReloadSpas;
    TBackgroundState BGState;
    /* client branch */
    int GattlingSoundChannel2, ReloadSoundChannel;
    int JetsSoundChannel, GattlingSoundChannel;
    bool OldDeadMeat;
    bool Muted;
    bool DontDrop;
    TVector2 NextPush[MAX_PUSHTICK + 1]; /* Pascal 0..MAX_PUSHTICK */
    unsigned short BulletCount;
} TSprite;
typedef TSprite *PSprite;

void TSprite_Update(TSprite *self);
void TSprite_Kill(TSprite *self);
void TSprite_Die(TSprite *self, int how, int who, int where, int what,
    TVector2 impact);
int TSprite_DropWeapon(TSprite *self);
void TSprite_LegsApplyAnimation(TSprite *self, TAnimation anim, int curr);
void TSprite_BodyApplyAnimation(TSprite *self, TAnimation anim, int curr);
void TSprite_MoveSkeleton(TSprite *self, float x1, float y1, bool fromZero);
bool TSprite_CheckMapCollision(TSprite *self, float x, float y, int area);
bool TSprite_CheckRadiusMapCollision(TSprite *self, float x, float y,
    bool hasCollided);
bool TSprite_CheckMapVerticesCollision(TSprite *self, float x, float y, float r,
    bool hasCollided);
bool TSprite_CheckSkeletonMapCollision(TSprite *self, int i, float x, float y);
void TSprite_HandleSpecialPolyTypes(TSprite *self, int polyType, TVector2 pos);
void TSprite_ApplyWeaponByNum(TSprite *self, unsigned char wnum, unsigned char gun,
    int ammo, bool restorePrimaryState);
void TSprite_ApplyWeaponByNumDefault(TSprite *self, unsigned char wnum,
    unsigned char gun);
void TSprite_HealthHit(TSprite *self, float amount, int who, int where, int what,
    TVector2 impact);
void TSprite_FreeControls(TSprite *self);
void TSprite_CheckOutOfBounds(TSprite *self);
void TSprite_CheckSkeletonOutOfBounds(TSprite *self);
void TSprite_Respawn(TSprite *self);
void TSprite_Parachute(TSprite *self, TVector2 a);
void TSprite_ChangeTeam(TSprite *self, int team);
void TSprite_Fire(TSprite *self);
void TSprite_ThrowFlag(TSprite *self);
void TSprite_ThrowGrenade(TSprite *self);
float TSprite_GetMoveacc(TSprite *self);
TVector2 TSprite_GetCursorAimDirection(TSprite *self);
TVector2 TSprite_GetHandsAimDirection(TSprite *self);
bool TSprite_IsSolo(TSprite *self);
bool TSprite_IsNotSolo(TSprite *self);
bool TSprite_IsInTeam(TSprite *self);
bool TSprite_IsSpectator(TSprite *self);
bool TSprite_IsNotSpectator(TSprite *self);
bool TSprite_IsInSameTeam(TSprite *self, TSprite *otherPlayer);
bool TSprite_IsNotInSameTeam(TSprite *self, TSprite *otherPlayer);
bool TSprite_CanRespawn(TSprite *self, bool deadMeatBeforeRespawn);

int CreateSprite(TVector2 sPos, TVector2 sVelocity, unsigned char sStyle,
    unsigned char n, TPlayer *player, bool transferOwnership);
bool TeamCollides(int poly, int team, bool bullet);

extern int SpriteMapColCount;
extern bool wasReloading;
extern int MySprite;

extern TSprite Sprite[MAX_SPRITES + 1];

#endif /* SPRITES_H */