#ifndef SCRIPTPLAYER_H
#define SCRIPTPLAYER_H

#include <stdint.h>
#include <stdbool.h>

/* Forward declarations of related types */
struct TSprite;
struct TPlayer;
struct TVector2;
struct TScriptWeapon;
struct TScriptPrimaryPlayerWeapon;
struct TScriptSecondaryPlayerWeapon;
struct TScriptActiveFlag;
struct TScriptActiveObject;

/* Enums */
typedef enum {
    TJoinNormal = 0,
    TJoinSilent = 1
} TJoinType;

typedef enum {
    TKickNoResponse = 0,
    TKickNoCheatResponse = 1,
    TKickChangeTeam = 2,
    TKickPing = 3,
    TKickFlooding = 4,
    TKickConsole = 5,
    TKickConnectionCheat = 6,
    TKickCheat = 7,
    TKickLeft = 8,
    TKickVoted = 9,
    TKickAC = 10,
    TKickSilent = 11
} TKickReason;

/* Callback types */
typedef void (*TOnFlagGrab)(struct TScriptActivePlayer *Player,
                            struct TScriptActiveFlag *Flag,
                            uint8_t Team,
                            bool GrabbedInBase);
typedef void (*TOnFlagReturn)(struct TScriptActivePlayer *Player,
                              struct TScriptActiveFlag *Flag,
                              uint8_t Team);
typedef void (*TOnFlagScore)(struct TScriptActivePlayer *Player,
                             struct TScriptActiveFlag *Flag,
                             uint8_t Team);
typedef void (*TOnFlagDrop)(struct TScriptActivePlayer *Player,
                            struct TScriptActiveFlag *Flag,
                            uint8_t Team,
                            bool Thrown);
typedef void (*TOnKitPickup)(struct TScriptActivePlayer *Player,
                             struct TScriptActiveObject *Kit);
typedef struct TVector2 (*TOnBeforeRespawn)(struct TScriptActivePlayer *Player);
typedef void (*TOnAfterRespawn)(struct TScriptActivePlayer *Player);
typedef float (*TOnDamage)(struct TScriptActivePlayer *Shooter,
                           struct TScriptActivePlayer *Victim,
                           float Damage,
                           uint8_t BulletID);
typedef void (*TOnKill)(struct TScriptActivePlayer *Killer,
                        struct TScriptActivePlayer *Victim,
                        uint8_t WeaponType);
typedef void (*TOnWeaponChange)(struct TScriptActivePlayer *Player,
                                struct TScriptWeapon *Primary,
                                struct TScriptWeapon *Secondary);
typedef bool (*TOnVoteMapStart)(struct TScriptActivePlayer *Player,
                                const char *Map);
typedef bool (*TOnVoteKickStart)(struct TScriptActivePlayer *Player,
                                 struct TScriptActivePlayer *Victim,
                                 const char *Reason);
typedef void (*TOnVoteMap)(struct TScriptActivePlayer *Player,
                           const char *Map);
typedef void (*TOnVoteKick)(struct TScriptActivePlayer *Player,
                            struct TScriptActivePlayer *Victim);
typedef void (*TOnSpeak)(struct TScriptActivePlayer *Player,
                         const char *Text);
typedef bool (*TOnCommand)(struct TScriptActivePlayer *Player,
                           const char *Command);

/* Base player structure */
typedef struct TScriptPlayer {
    struct TSprite *FSpritePtr;
    struct TScriptWeapon *FPrimary;
    struct TScriptWeapon *FSecondary;

    /* Property getters / setters */
    uint8_t (*GetTeam)(struct TScriptPlayer *Self);
    const char *(*GetName)(struct TScriptPlayer *Self);
    bool (*GetAlive)(struct TScriptPlayer *Self);
    float (*GetHealth)(struct TScriptPlayer *Self);
    void (*SetHealth)(struct TScriptPlayer *Self, float Health);
    float (*GetVest)(struct TScriptPlayer *Self);
    void (*SetVest)(struct TScriptPlayer *Self, float Vest);
    struct TScriptPrimaryPlayerWeapon *(*GetPrimary)(struct TScriptPlayer *Self);
    struct TScriptSecondaryPlayerWeapon *(*GetSecondary)(struct TScriptPlayer *Self);
    uint32_t (*GetShirtColor)(struct TScriptPlayer *Self);
    uint32_t (*GetPantsColor)(struct TScriptPlayer *Self);
    uint32_t (*GetSkinColor)(struct TScriptPlayer *Self);
    uint32_t (*GetHairColor)(struct TScriptPlayer *Self);
    const char *(*GetFavouriteWeapon)(struct TScriptPlayer *Self);
    void (*SetFavouriteWeapon)(struct TScriptPlayer *Self, const char *Weapon);
    uint8_t (*GetChosenSecondaryWeapon)(struct TScriptPlayer *Self);
    const char *(*GetFriend)(struct TScriptPlayer *Self);
    void (*SetFriend)(struct TScriptPlayer *Self, const char *Friend);
    uint8_t (*GetAccuracy)(struct TScriptPlayer *Self);
    void (*SetAccuracy)(struct TScriptPlayer *Self, uint8_t Accuracy);
    bool (*GetShootDead)(struct TScriptPlayer *Self);
    void (*SetShootDead)(struct TScriptPlayer *Self, bool ShootDead);
    int (*GetGrenadeFrequency)(struct TScriptPlayer *Self);
    void (*SetGrenadeFrequency)(struct TScriptPlayer *Self, int Frequency);
    bool (*GetCamping)(struct TScriptPlayer *Self);
    void (*SetCamping)(struct TScriptPlayer *Self, bool Camping);
    uint8_t (*GetOnStartUse)(struct TScriptPlayer *Self);
    void (*SetOnStartUse)(struct TScriptPlayer *Self, uint8_t Thing);
    uint8_t (*GetHairStyle)(struct TScriptPlayer *Self);
    uint8_t (*GetHeadgear)(struct TScriptPlayer *Self);
    uint8_t (*GetChain)(struct TScriptPlayer *Self);
    uint8_t (*GetChatFrequency)(struct TScriptPlayer *Self);
    void (*SetChatFrequency)(struct TScriptPlayer *Self, uint8_t Frequency);
    const char *(*GetChatKill)(struct TScriptPlayer *Self);
    void (*SetChatKill)(struct TScriptPlayer *Self, const char *Message);
    const char *(*GetChatDead)(struct TScriptPlayer *Self);
    void (*SetChatDead)(struct TScriptPlayer *Self, const char *Message);
    const char *(*GetChatLowHealth)(struct TScriptPlayer *Self);
    void (*SetChatLowHealth)(struct TScriptPlayer *Self, const char *Message);
    const char *(*GetChatSeeEnemy)(struct TScriptPlayer *Self);
    void (*SetChatSeeEnemy)(struct TScriptPlayer *Self, const char *Message);
    const char *(*GetChatWinning)(struct TScriptPlayer *Self);
    void (*SetChatWinning)(struct TScriptPlayer *Self, const char *Message);
    bool (*GetAdmin)(struct TScriptPlayer *Self);
    void (*SetAdmin)(struct TScriptPlayer *Self, bool SetAsAdmin);
    bool (*GetDummy)(struct TScriptPlayer *Self);
    void (*SetDummy)(struct TScriptPlayer *Self, bool Dummy);

    void (*Destroy)(struct TScriptPlayer *Self);
} TScriptPlayer;

/* New player (not yet spawned) */
typedef struct TScriptNewPlayer {
    TScriptPlayer base; /* inheritance */

    void (*SetName)(struct TScriptNewPlayer *Self, const char *Name);
    void (*SetTeam)(struct TScriptNewPlayer *Self, uint8_t Team);
    void (*SetHealth)(struct TScriptNewPlayer *Self, float Health);
    struct TScriptWeapon *(*GetPrimary)(struct TScriptNewPlayer *Self);
    void (*SetPrimary)(struct TScriptNewPlayer *Self, struct TScriptWeapon *Primary);
    struct TScriptWeapon *(*GetSecondary)(struct TScriptNewPlayer *Self);
    void (*SetSecondary)(struct TScriptNewPlayer *Self, struct TScriptWeapon *Secondary);
    void (*SetShirtColor)(struct TScriptNewPlayer *Self, uint32_t Color);
    void (*SetPantsColor)(struct TScriptNewPlayer *Self, uint32_t Color);
    void (*SetSkinColor)(struct TScriptNewPlayer *Self, uint32_t Color);
    void (*SetHairColor)(struct TScriptNewPlayer *Self, uint32_t Color);
    void (*SetHairStyle)(struct TScriptNewPlayer *Self, uint8_t Style);
    void (*SetHeadgear)(struct TScriptNewPlayer *Self, uint8_t Headgear);
    void (*SetChain)(struct TScriptNewPlayer *Self, uint8_t Chain);
    bool (*GetDummy)(struct TScriptNewPlayer *Self);
    void (*SetDummy)(struct TScriptNewPlayer *Self, bool Dummy);

    struct TScriptNewPlayer *(*Create)(void);
    void (*Destroy)(struct TScriptNewPlayer *Self);
} TScriptNewPlayer;

/* Active player (in‑game) */
typedef struct TScriptActivePlayer {
    TScriptPlayer base; /* inheritance */

    uint8_t FID;

    /* Event callbacks */
    TOnFlagGrab OnFlagGrab;
    TOnFlagReturn OnFlagReturn;
    TOnFlagScore OnFlagScore;
    TOnFlagDrop OnFlagDrop;
    TOnKitPickup OnKitPickup;
    TOnBeforeRespawn OnBeforeRespawn;
    TOnAfterRespawn OnAfterRespawn;
    TOnDamage OnDamage;
    TOnKill OnKill;
    TOnWeaponChange OnWeaponChange;
    TOnVoteMapStart OnVoteMapStart;
    TOnVoteKickStart OnVoteKickStart;
    TOnVoteMap OnVoteMap;
    TOnVoteKick OnVoteKick;
    TOnSpeak OnSpeak;
    TOnCommand OnCommand;

    /* Property getters / setters */
    int (*GetKills)(struct TScriptActivePlayer *Self);
    void (*SetKills)(struct TScriptActivePlayer *Self, int Kills);
    int (*GetDeaths)(struct TScriptActivePlayer *Self);
    void (*SetDeaths)(struct TScriptActivePlayer *Self, int Deaths);
    int (*GetPing)(struct TScriptActivePlayer *Self);
    void (*SetTeam)(struct TScriptActivePlayer *Self, uint8_t Team);
    bool (*GetActive)(struct TScriptActivePlayer *Self);
    const char *(*GetIP)(struct TScriptActivePlayer *Self);
    uint16_t (*GetPort)(struct TScriptActivePlayer *Self);
    float (*GetVelX)(struct TScriptActivePlayer *Self);
    float (*GetVelY)(struct TScriptActivePlayer *Self);
    bool (*GetMuted)(struct TScriptActivePlayer *Self);
    void (*SetMuted)(struct TScriptActivePlayer *Self, bool Muted);
    int (*GetJets)(struct TScriptActivePlayer *Self);
    uint8_t (*GetGrenades)(struct TScriptActivePlayer *Self);
    float (*GetX)(struct TScriptActivePlayer *Self);
    float (*GetY)(struct TScriptActivePlayer *Self);
    int16_t (*GetMouseAimX)(struct TScriptActivePlayer *Self);
    void (*SetMouseAimX)(struct TScriptActivePlayer *Self, int16_t AimX);
    int16_t (*GetMouseAimY)(struct TScriptActivePlayer *Self);
    void (*SetMouseAimY)(struct TScriptActivePlayer *Self, int16_t AimY);
    bool (*GetFlagger)(struct TScriptActivePlayer *Self);
    int (*GetTime)(struct TScriptActivePlayer *Self);
    bool (*GetOnGround)(struct TScriptActivePlayer *Self);
    bool (*GetProne)(struct TScriptActivePlayer *Self);
    bool (*GetHuman)(struct TScriptActivePlayer *Self);
    int8_t (*GetDirection)(struct TScriptActivePlayer *Self);
    uint8_t (*GetFlags)(struct TScriptActivePlayer *Self);
    const char *(*GetHWID)(struct TScriptActivePlayer *Self);
#if defined(STEAM)
    int64_t (*GetSteamID)(struct TScriptActivePlayer *Self);
#endif
    bool (*GetKeyUp)(struct TScriptActivePlayer *Self);
    void (*SetKeyUp)(struct TScriptActivePlayer *Self, bool Pressed);
    bool (*GetKeyLeft)(struct TScriptActivePlayer *Self);
    void (*SetKeyLeft)(struct TScriptActivePlayer *Self, bool Pressed);
    bool (*GetKeyRight)(struct TScriptActivePlayer *Self);
    void (*SetKeyRight)(struct TScriptActivePlayer *Self, bool Pressed);
    bool (*GetKeyShoot)(struct TScriptActivePlayer *Self);
    void (*SetKeyShoot)(struct TScriptActivePlayer *Self, bool Pressed);
    bool (*GetKeyJetpack)(struct TScriptActivePlayer *Self);
    void (*SetKeyJetpack)(struct TScriptActivePlayer *Self, bool Pressed);
    bool (*GetKeyGrenade)(struct TScriptActivePlayer *Self);
    void (*SetKeyGrenade)(struct TScriptActivePlayer *Self, bool Pressed);
    bool (*GetKeyChangeWeap)(struct TScriptActivePlayer *Self);
    void (*SetKeyChangeWeap)(struct TScriptActivePlayer *Self, bool Pressed);
    bool (*GetKeyThrow)(struct TScriptActivePlayer *Self);
    void (*SetKeyThrow)(struct TScriptActivePlayer *Self, bool Pressed);
    bool (*GetKeyReload)(struct TScriptActivePlayer *Self);
    void (*SetKeyReload)(struct TScriptActivePlayer *Self, bool Pressed);
    bool (*GetKeyCrouch)(struct TScriptActivePlayer *Self);
    void (*SetKeyCrouch)(struct TScriptActivePlayer *Self, bool Pressed);
    bool (*GetKeyProne)(struct TScriptActivePlayer *Self);
    void (*SetKeyProne)(struct TScriptActivePlayer *Self, bool Pressed);
    bool (*GetKeyFlagThrow)(struct TScriptActivePlayer *Self);
    void (*SetKeyFlagThrow)(struct TScriptActivePlayer *Self, bool Pressed);
    void (*SetWeaponActive)(struct TScriptActivePlayer *Self, uint8_t ID, bool Active);

    /* Methods */
    bool (*Ban)(struct TScriptActivePlayer *Self, int Time, const char *Reason);
    void (*Say)(struct TScriptActivePlayer *Self, const char *Text, uint8_t MsgType);
    void (*Damage)(struct TScriptActivePlayer *Self, uint8_t Shooter, float Damage);
    void (*Kill)(struct TScriptActivePlayer *Self);
    void (*BigText)(struct TScriptActivePlayer *Self,
                    uint8_t Layer, const char *Text, int Delay,
                    int32_t Color, float Scale, int X, int Y);
    void (*WorldText)(struct TScriptActivePlayer *Self,
                      uint8_t Layer, const char *Text, int Delay,
                      int32_t Color, float Scale, float X, float Y);
    void (*ForceWeapon)(struct TScriptActivePlayer *Self,
                        struct TScriptWeapon *Primary,
                        struct TScriptWeapon *Secondary);
    void (*ForwardTo)(struct TScriptActivePlayer *Self,
                      const char *TargetIP, uint16_t TargetPort,
                      const char *Message);
    void (*GiveBonus)(struct TScriptActivePlayer *Self, uint8_t BType);
    bool (*Kick)(struct TScriptActivePlayer *Self, TKickReason Reason);
    void (*PlaySound)(struct TScriptActivePlayer *Self,
                      const char *Name, float X, float Y);
    void (*Move)(struct TScriptActivePlayer *Self, float X, float Y);
    void (*SetVelocity)(struct TScriptActivePlayer *Self, float VelX, float VelY);
    void (*Tell)(struct TScriptActivePlayer *Self, const char *Text);
    void (*WriteConsole)(struct TScriptActivePlayer *Self,
                         const char *Text, int32_t Color);
    void (*ChangeTeam)(struct TScriptActivePlayer *Self,
                       uint8_t NewTeam, TJoinType JoinType);

    struct TScriptActivePlayer *(*Create)(struct TSprite *Sprite, uint8_t ID);
    void (*Destroy)(struct TScriptActivePlayer *Self);
} TScriptActivePlayer;

/* API registration */
typedef struct TScriptPlayerAPI {
    void (*CompilerRegister)(void *Compiler);
    void (*RuntimeRegisterApi)(void *Exec);
} TScriptPlayerAPI;

#endif /* SCRIPTPLAYER_H */