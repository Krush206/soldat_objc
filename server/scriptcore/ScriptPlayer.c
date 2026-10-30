#include "ScriptPlayer.h"
#include <string.h>
#include <stdlib.h>

/* Assume the following external structures are defined elsewhere:
   struct TSprite { struct TPlayer *Player; bool Active; bool DeadMeat; float Health; float Vest; ... };
   struct TPlayer { uint8_t Team; char *Name; uint32_t ShirtColor; uint32_t PantsColor; uint32_t SkinColor; uint32_t HairColor; uint8_t HairStyle; uint8_t HeadCap; uint8_t Chain; uint8_t SecWep; char *Friend; uint8_t Accuracy; uint8_t DeadKill; int GrenadeFreq; uint8_t Camper; uint8_t Use; uint8_t ChatFreq; char *ChatKill; char *ChatDead; char *ChatLowHealth; char *ChatSeeEnemy; char *ChatWinning; char *IP; uint16_t Port; uint8_t Muted; char *hwid; int64_t SteamID; int PlayTime; uint8_t Flags; uint8_t ControlMethod; ... };
   struct TVector2 { float x, y; };
   extern struct TSprite *SpriteParts[];
   extern int MAX_SPRITES;
   extern int MAX_PLAYERS;
   extern char *mutelist[];
   extern char *mutename[];
   extern int sv_maxgrenades;
   extern int PREDATORALPHA, PREDATORBONUSTIME, STARTHEALTH, BONUS_PREDATOR, BONUS_BERSERKER, BERSERKERBONUSTIME, DEFAULTVEST, FRAGGRENADE, CLUSTERGRENADE, CLUSTER_GRENADES, FLAMERBONUSTIME, BONUS_FLAMEGOD;
   extern int DefaultGuns[];
   extern struct TSprite *Guns[];
   extern bool KickPlayer(int ID, bool Ban, int Reason, int Time, const char *ReasonStr);
   extern void ServerSendStringMessage(const char *Text, int ToID, int FromID, int MsgType);
   extern void MainConsole_Console(const char *Text, int Color);
   extern void ServerSendSpecialMessage(const char *Text, int Type, int Layer, int Delay, float Scale, uint32_t Color, float X, float Y, int PlayerID);
   extern void NetworkServerFunctions_ForceWeapon(int ID, int PrimaryType, int SecondaryType, int PrimaryAmmo, int SecondaryAmmo);
   extern void ForwardClient(int ID, const char *TargetIP, uint16_t TargetPort, const char *Message);
   extern int CreateThing(struct TVector2 Pos, int Style, int Type, int Owner);
   extern void ServerThingTaken(int ThingID, int PlayerID);
   extern void NetworkServerFunctions_PlaySound(int ID, const char *Name, float X, float Y);
   extern void MovePlayer(int ID, float X, float Y);
   extern void ModifyPlayerVelocity(int ID, float VelX, float VelY);
   extern void ServerHelper_WriteConsole(int ID, const char *Text, int32_t Color);
   extern bool IsRemoteAdminIP(const char *IP);
   extern bool IsAdminIP(const char *IP);
   extern void ParseInput(const char *Cmd, int ID);
   extern void SortPlayers(void);
   extern int WeaponNameToNum(const char *Name);
   extern const char *WeaponNumToName(int Num);
*/

/* Helper macros */
#define SPRITE(self) ((self)->FSpritePtr)
#define PLAYER(self) (SPRITE(self)->Player)

/* ---------- TScriptPlayer base implementation ---------- */

static uint8_t ScriptPlayer_GetTeam(TScriptPlayer *Self) {
    return PLAYER(Self)->Team;
}
static const char *ScriptPlayer_GetName(TScriptPlayer *Self) {
    return PLAYER(Self)->Name;
}
static bool ScriptPlayer_GetAlive(TScriptPlayer *Self) {
    return SPRITE(Self)->Active && !SPRITE(Self)->DeadMeat;
}
static float ScriptPlayer_GetHealth(TScriptPlayer *Self) {
    return SPRITE(Self)->Health;
}
static void ScriptPlayer_SetHealth(TScriptPlayer *Self, float Health) {
    SPRITE(Self)->Health = Health;
}
static float ScriptPlayer_GetVest(TScriptPlayer *Self) {
    return SPRITE(Self)->Vest;
}
static void ScriptPlayer_SetVest(TScriptPlayer *Self, float Vest) {
    SPRITE(Self)->Vest = Vest;
}
static TScriptPrimaryPlayerWeapon *ScriptPlayer_GetPrimary(TScriptPlayer *Self) {
    return (TScriptPrimaryPlayerWeapon *)Self->FPrimary;
}
static TScriptSecondaryPlayerWeapon *ScriptPlayer_GetSecondary(TScriptPlayer *Self) {
    return (TScriptSecondaryPlayerWeapon *)Self->FSecondary;
}
static uint32_t ScriptPlayer_GetShirtColor(TScriptPlayer *Self) {
    return PLAYER(Self)->ShirtColor;
}
static uint32_t ScriptPlayer_GetPantsColor(TScriptPlayer *Self) {
    return PLAYER(Self)->PantsColor;
}
static uint32_t ScriptPlayer_GetSkinColor(TScriptPlayer *Self) {
    return PLAYER(Self)->SkinColor;
}
static uint32_t ScriptPlayer_GetHairColor(TScriptPlayer *Self) {
    return PLAYER(Self)->HairColor;
}
static const char *ScriptPlayer_GetFavouriteWeapon(TScriptPlayer *Self) {
    return WeaponNumToName(SPRITE(Self)->Brain.FavWeapon);
}
static void ScriptPlayer_SetFavouriteWeapon(TScriptPlayer *Self, const char *Weapon) {
    int num = WeaponNameToNum(Weapon);
    if (num == -1) num = WeaponNameToNum("Hands");
    SPRITE(Self)->Brain.FavWeapon = num;
}
static uint8_t ScriptPlayer_GetChosenSecondaryWeapon(TScriptPlayer *Self) {
    return PLAYER(Self)->SecWep;
}
static const char *ScriptPlayer_GetFriend(TScriptPlayer *Self) {
    return SPRITE(Self)->Brain.Friend;
}
static void ScriptPlayer_SetFriend(TScriptPlayer *Self, const char *Friend) {
    SPRITE(Self)->Brain.Friend = Friend;
}
static uint8_t ScriptPlayer_GetAccuracy(TScriptPlayer *Self) {
    return SPRITE(Self)->Brain.Accuracy;
}
static void ScriptPlayer_SetAccuracy(TScriptPlayer *Self, uint8_t Accuracy) {
    SPRITE(Self)->Brain.Accuracy = Accuracy;
}
static bool ScriptPlayer_GetShootDead(TScriptPlayer *Self) {
    return SPRITE(Self)->Brain.DeadKill != 0;
}
static void ScriptPlayer_SetShootDead(TScriptPlayer *Self, bool ShootDead) {
    SPRITE(Self)->Brain.DeadKill = ShootDead ? 1 : 0;
}
static int ScriptPlayer_GetGrenadeFrequency(TScriptPlayer *Self) {
    return SPRITE(Self)->Brain.GrenadeFreq;
}
static void ScriptPlayer_SetGrenadeFrequency(TScriptPlayer *Self, int Frequency) {
    SPRITE(Self)->Brain.GrenadeFreq = Frequency;
}
static bool ScriptPlayer_GetCamping(TScriptPlayer *Self) {
    return SPRITE(Self)->Brain.Camper != 0;
}
static void ScriptPlayer_SetCamping(TScriptPlayer *Self, bool Camping) {
    SPRITE(Self)->Brain.Camper = Camping ? 1 : 0;
}
static uint8_t ScriptPlayer_GetOnStartUse(TScriptPlayer *Self) {
    return SPRITE(Self)->Brain.Use;
}
static void ScriptPlayer_SetOnStartUse(TScriptPlayer *Self, uint8_t Thing) {
    SPRITE(Self)->Brain.Use = Thing;
}
static uint8_t ScriptPlayer_GetHairStyle(TScriptPlayer *Self) {
    return PLAYER(Self)->HairStyle;
}
static uint8_t ScriptPlayer_GetHeadgear(TScriptPlayer *Self) {
    return PLAYER(Self)->HeadCap;
}
static uint8_t ScriptPlayer_GetChain(TScriptPlayer *Self) {
    return PLAYER(Self)->Chain;
}
static uint8_t ScriptPlayer_GetChatFrequency(TScriptPlayer *Self) {
    return SPRITE(Self)->Brain.ChatFreq;
}
static void ScriptPlayer_SetChatFrequency(TScriptPlayer *Self, uint8_t Frequency) {
    SPRITE(Self)->Brain.ChatFreq = Frequency;
}
static const char *ScriptPlayer_GetChatKill(TScriptPlayer *Self) {
    return SPRITE(Self)->Brain.ChatKill;
}
static void ScriptPlayer_SetChatKill(TScriptPlayer *Self, const char *Message) {
    SPRITE(Self)->Brain.ChatKill = Message;
}
static const char *ScriptPlayer_GetChatDead(TScriptPlayer *Self) {
    return SPRITE(Self)->Brain.ChatDead;
}
static void ScriptPlayer_SetChatDead(TScriptPlayer *Self, const char *Message) {
    SPRITE(Self)->Brain.ChatDead = Message;
}
static const char *ScriptPlayer_GetChatLowHealth(TScriptPlayer *Self) {
    return SPRITE(Self)->Brain.ChatLowHealth;
}
static void ScriptPlayer_SetChatLowHealth(TScriptPlayer *Self, const char *Message) {
    SPRITE(Self)->Brain.ChatLowHealth = Message;
}
static const char *ScriptPlayer_GetChatSeeEnemy(TScriptPlayer *Self) {
    return SPRITE(Self)->Brain.ChatSeeEnemy;
}
static void ScriptPlayer_SetChatSeeEnemy(TScriptPlayer *Self, const char *Message) {
    SPRITE(Self)->Brain.ChatSeeEnemy = Message;
}
static const char *ScriptPlayer_GetChatWinning(TScriptPlayer *Self) {
    return SPRITE(Self)->Brain.ChatWinning;
}
static void ScriptPlayer_SetChatWinning(TScriptPlayer *Self, const char *Message) {
    SPRITE(Self)->Brain.ChatWinning = Message;
}
static bool ScriptPlayer_GetAdmin(TScriptPlayer *Self) {
    if (!SPRITE(Self)->Active) return false;
    if (IsRemoteAdminIP(PLAYER(Self)->IP)) return true;
    if (IsAdminIP(PLAYER(Self)->IP)) return true;
    return false;
}
static void ScriptPlayer_SetAdmin(TScriptPlayer *Self, bool SetAsAdmin) {
    if (SetAsAdmin) {
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "admip %s", PLAYER(Self)->IP);
        ParseInput(cmd, 255);
    } else {
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "unadm %s", PLAYER(Self)->IP);
        ParseInput(cmd, 255);
    }
}
static bool ScriptPlayer_GetDummy(TScriptPlayer *Self) {
    return SPRITE(Self)->Active &&
           (PLAYER(Self)->ControlMethod != 0) && /* assuming HUMAN == 0 */
           SPRITE(Self)->Dummy;
}
static void ScriptPlayer_SetDummy(TScriptPlayer *Self, bool Dummy) {
    SPRITE(Self)->Dummy = Dummy;
}
static void ScriptPlayer_Destroy(TScriptPlayer *Self) {
    if (Self->FPrimary) { free(Self->FPrimary); Self->FPrimary = NULL; }
    if (Self->FSecondary) { free(Self->FSecondary); Self->FSecondary = NULL; }
}

/* Initialize base vtable */
static void ScriptPlayer_InitVTable(TScriptPlayer *Self) {
    Self->GetTeam = ScriptPlayer_GetTeam;
    Self->GetName = ScriptPlayer_GetName;
    Self->GetAlive = ScriptPlayer_GetAlive;
    Self->GetHealth = ScriptPlayer_GetHealth;
    Self->SetHealth = ScriptPlayer_SetHealth;
    Self->GetVest = ScriptPlayer_GetVest;
    Self->SetVest = ScriptPlayer_SetVest;
    Self->GetPrimary = ScriptPlayer_GetPrimary;
    Self->GetSecondary = ScriptPlayer_GetSecondary;
    Self->GetShirtColor = ScriptPlayer_GetShirtColor;
    Self->GetPantsColor = ScriptPlayer_GetPantsColor;
    Self->GetSkinColor = ScriptPlayer_GetSkinColor;
    Self->GetHairColor = ScriptPlayer_GetHairColor;
    Self->GetFavouriteWeapon = ScriptPlayer_GetFavouriteWeapon;
    Self->SetFavouriteWeapon = ScriptPlayer_SetFavouriteWeapon;
    Self->GetChosenSecondaryWeapon = ScriptPlayer_GetChosenSecondaryWeapon;
    Self->GetFriend = ScriptPlayer_GetFriend;
    Self->SetFriend = ScriptPlayer_SetFriend;
    Self->GetAccuracy = ScriptPlayer_GetAccuracy;
    Self->SetAccuracy = ScriptPlayer_SetAccuracy;
    Self->GetShootDead = ScriptPlayer_GetShootDead;
    Self->SetShootDead = ScriptPlayer_SetShootDead;
    Self->GetGrenadeFrequency = ScriptPlayer_GetGrenadeFrequency;
    Self->SetGrenadeFrequency = ScriptPlayer_SetGrenadeFrequency;
    Self->GetCamping = ScriptPlayer_GetCamping;
    Self->SetCamping = ScriptPlayer_SetCamping;
    Self->GetOnStartUse = ScriptPlayer_GetOnStartUse;
    Self->SetOnStartUse = ScriptPlayer_SetOnStartUse;
    Self->GetHairStyle = ScriptPlayer_GetHairStyle;
    Self->GetHeadgear = ScriptPlayer_GetHeadgear;
    Self->GetChain = ScriptPlayer_GetChain;
    Self->GetChatFrequency = ScriptPlayer_GetChatFrequency;
    Self->SetChatFrequency = ScriptPlayer_SetChatFrequency;
    Self->GetChatKill = ScriptPlayer_GetChatKill;
    Self->SetChatKill = ScriptPlayer_SetChatKill;
    Self->GetChatDead = ScriptPlayer_GetChatDead;
    Self->SetChatDead = ScriptPlayer_SetChatDead;
    Self->GetChatLowHealth = ScriptPlayer_GetChatLowHealth;
    Self->SetChatLowHealth = ScriptPlayer_SetChatLowHealth;
    Self->GetChatSeeEnemy = ScriptPlayer_GetChatSeeEnemy;
    Self->SetChatSeeEnemy = ScriptPlayer_SetChatSeeEnemy;
    Self->GetChatWinning = ScriptPlayer_GetChatWinning;
    Self->SetChatWinning = ScriptPlayer_SetChatWinning;
    Self->GetAdmin = ScriptPlayer_GetAdmin;
    Self->SetAdmin = ScriptPlayer_SetAdmin;
    Self->GetDummy = ScriptPlayer_GetDummy;
    Self->SetDummy = ScriptPlayer_SetDummy;
    Self->Destroy = ScriptPlayer_Destroy;
}

/* ---------- TScriptNewPlayer implementation ---------- */

static void ScriptNewPlayer_SetName(TScriptNewPlayer *Self, const char *Name) {
    PLAYER(Self)->Name = Name;
}
static void ScriptNewPlayer_SetTeam(TScriptNewPlayer *Self, uint8_t Team) {
    PLAYER(Self)->Team = Team;
}
static void ScriptNewPlayer_SetHealth(TScriptNewPlayer *Self, float Health) {
    SPRITE(Self)->Health = Health;
}
static TScriptWeapon *ScriptNewPlayer_GetPrimary(TScriptNewPlayer *Self) {
    return Self->base.FPrimary;
}
static void ScriptNewPlayer_SetPrimary(TScriptNewPlayer *Self, TScriptWeapon *Primary) {
    Self->base.FPrimary = Primary;
}
static TScriptWeapon *ScriptNewPlayer_GetSecondary(TScriptNewPlayer *Self) {
    return Self->base.FSecondary;
}
static void ScriptNewPlayer_SetSecondary(TScriptNewPlayer *Self, TScriptWeapon *Secondary) {
    Self->base.FSecondary = Secondary;
}
static void ScriptNewPlayer_SetShirtColor(TScriptNewPlayer *Self, uint32_t Color) {
    PLAYER(Self)->ShirtColor = Color;
}
static void ScriptNewPlayer_SetPantsColor(TScriptNewPlayer *Self, uint32_t Color) {
    PLAYER(Self)->PantsColor = Color;
}
static void ScriptNewPlayer_SetSkinColor(TScriptNewPlayer *Self, uint32_t Color) {
    PLAYER(Self)->SkinColor = Color;
}
static void ScriptNewPlayer_SetHairColor(TScriptNewPlayer *Self, uint32_t Color) {
    PLAYER(Self)->HairColor = Color;
}
static void ScriptNewPlayer_SetHairStyle(TScriptNewPlayer *Self, uint8_t Style) {
    PLAYER(Self)->HairStyle = Style;
}
static void ScriptNewPlayer_SetHeadgear(TScriptNewPlayer *Self, uint8_t Headgear) {
    PLAYER(Self)->HeadCap = Headgear;
}
static void ScriptNewPlayer_SetChain(TScriptNewPlayer *Self, uint8_t Chain) {
    PLAYER(Self)->Chain = Chain;
}
static bool ScriptNewPlayer_GetDummy(TScriptNewPlayer *Self) {
    return SPRITE(Self)->Dummy;
}
static void ScriptNewPlayer_SetDummy(TScriptNewPlayer *Self, bool Dummy) {
    SPRITE(Self)->Dummy = Dummy;
}
static TScriptNewPlayer *ScriptNewPlayer_Create(void) {
    TScriptNewPlayer *Self = calloc(1, sizeof(TScriptNewPlayer));
    if (!Self) return NULL;
    Self->base.FSpritePtr = calloc(1, sizeof(struct TSprite));
    if (!Self->base.FSpritePtr) { free(Self); return NULL; }
    memset(Self->base.FSpritePtr, 0, sizeof(struct TSprite));
    Self->base.FPrimary = calloc(1, sizeof(TScriptWeapon)); /* placeholder */
    Self->base.FSecondary = calloc(1, sizeof(TScriptWeapon));
    /* Initialize nested objects */
    PLAYER(Self) = calloc(1, sizeof(struct TPlayer));
    SPRITE(Self)->Brain.TargetNum = 1;
    SPRITE(Self)->Brain.WaypointTimeoutCounter = 100; /* WAYPOINT_TIMEOUT_SMALL */

    ScriptPlayer_InitVTable(&Self->base);
    Self->SetName = ScriptNewPlayer_SetName;
    Self->SetTeam = ScriptNewPlayer_SetTeam;
    Self->SetHealth = ScriptNewPlayer_SetHealth;
    Self->GetPrimary = ScriptNewPlayer_GetPrimary;
    Self->SetPrimary = ScriptNewPlayer_SetPrimary;
    Self->GetSecondary = ScriptNewPlayer_GetSecondary;
    Self->SetSecondary = ScriptNewPlayer_SetSecondary;
    Self->SetShirtColor = ScriptNewPlayer_SetShirtColor;
    Self->SetPantsColor = ScriptNewPlayer_SetPantsColor;
    Self->SetSkinColor = ScriptNewPlayer_SetSkinColor;
    Self->SetHairColor = ScriptNewPlayer_SetHairColor;
    Self->SetHairStyle = ScriptNewPlayer_SetHairStyle;
    Self->SetHeadgear = ScriptNewPlayer_SetHeadgear;
    Self->SetChain = ScriptNewPlayer_SetChain;
    Self->GetDummy = ScriptNewPlayer_GetDummy;
    Self->SetDummy = ScriptNewPlayer_SetDummy;
    Self->base.Destroy = (void (*)(TScriptPlayer*))ScriptNewPlayer_Destroy;
    return Self;
}
static void ScriptNewPlayer_Destroy(TScriptNewPlayer *Self) {
    if (Self->base.FSpritePtr) {
        if (PLAYER(Self)) free(PLAYER(Self));
        free(Self->base.FSpritePtr);
    }
    if (Self->base.FPrimary) free(Self->base.FPrimary);
    if (Self->base.FSecondary) free(Self->base.FSecondary);
    free(Self);
}

/* ---------- TScriptActivePlayer implementation ---------- */

static int ScriptActivePlayer_GetKills(TScriptActivePlayer *Self) {
    return PLAYER(Self)->Kills;
}
static void ScriptActivePlayer_SetKills(TScriptActivePlayer *Self, int Kills) {
    PLAYER(Self)->Kills = Kills;
    SortPlayers();
}
static int ScriptActivePlayer_GetDeaths(TScriptActivePlayer *Self) {
    return PLAYER(Self)->Deaths;
}
static void ScriptActivePlayer_SetDeaths(TScriptActivePlayer *Self, int Deaths) {
    PLAYER(Self)->Deaths = Deaths;
    SortPlayers();
}
static int ScriptActivePlayer_GetPing(TScriptActivePlayer *Self) {
    return PLAYER(Self)->RealPing;
}
static void ScriptActivePlayer_SetTeam(TScriptActivePlayer *Self, uint8_t Team) {
    Self->ChangeTeam(Self, Team, TJoinNormal);
}
static bool ScriptActivePlayer_GetActive(TScriptActivePlayer *Self) {
    return SPRITE(Self)->Active;
}
static const char *ScriptActivePlayer_GetIP(TScriptActivePlayer *Self) {
    return PLAYER(Self)->IP;
}
static uint16_t ScriptActivePlayer_GetPort(TScriptActivePlayer *Self) {
    return PLAYER(Self)->Port;
}
static float ScriptActivePlayer_GetVelX(TScriptActivePlayer *Self) {
    return SpriteParts[Self->FID].Velocity.x;
}
static float ScriptActivePlayer_GetVelY(TScriptActivePlayer *Self) {
    return SpriteParts[Self->FID].Velocity.y;
}
static bool ScriptActivePlayer_GetMuted(TScriptActivePlayer *Self) {
    return PLAYER(Self)->Muted != 0;
}
static void ScriptActivePlayer_SetMuted(TScriptActivePlayer *Self, bool Muted) {
    if (!SPRITE(Self)->Active) return;
    if (Muted && PLAYER(Self)->Muted == 0) {
        PLAYER(Self)->Muted = 1;
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            if (mutelist[i] == NULL || mutelist[i][0] == '\0') {
                mutelist[i] = PLAYER(Self)->IP;
                mutename[i] = PLAYER(Self)->Name;
                break;
            }
        }
    } else if (!Muted && PLAYER(Self)->Muted == 1) {
        PLAYER(Self)->Muted = 0;
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            if (mutelist[i] && strcmp(mutelist[i], PLAYER(Self)->IP) == 0) {
                mutelist[i] = "";
                break;
            }
        }
    }
}
static int ScriptActivePlayer_GetJets(TScriptActivePlayer *Self) {
    return SPRITE(Self)->JetsCount;
}
static uint8_t ScriptActivePlayer_GetGrenades(TScriptActivePlayer *Self) {
    return SPRITE(Self)->TertiaryWeapon.AmmoCount;
}
static float ScriptActivePlayer_GetX(TScriptActivePlayer *Self) {
    return SpriteParts[Self->FID].Pos.x;
}
static float ScriptActivePlayer_GetY(TScriptActivePlayer *Self) {
    return SpriteParts[Self->FID].Pos.y;
}
static int16_t ScriptActivePlayer_GetMouseAimX(TScriptActivePlayer *Self) {
    return SPRITE(Self)->Control.MouseAimX;
}
static void ScriptActivePlayer_SetMouseAimX(TScriptActivePlayer *Self, int16_t AimX) {
    if (!Self->GetHuman(Self))
        SPRITE(Self)->Control.MouseAimX = AimX;
}
static int16_t ScriptActivePlayer_GetMouseAimY(TScriptActivePlayer *Self) {
    return SPRITE(Self)->Control.MouseAimY;
}
static void ScriptActivePlayer_SetMouseAimY(TScriptActivePlayer *Self, int16_t AimY) {
    if (!Self->GetHuman(Self))
        SPRITE(Self)->Control.MouseAimY = AimY;
}
static bool ScriptActivePlayer_GetFlagger(TScriptActivePlayer *Self) {
    return (SPRITE(Self)->HoldedThing > 0) && (SPRITE(Self)->HoldedThing < 4);
}
static int ScriptActivePlayer_GetTime(TScriptActivePlayer *Self) {
    return PLAYER(Self)->PlayTime;
}
static bool ScriptActivePlayer_GetOnGround(TScriptActivePlayer *Self) {
    return SPRITE(Self)->OnGround;
}
static bool ScriptActivePlayer_GetProne(TScriptActivePlayer *Self) {
    return SPRITE(Self)->Position == POS_PRONE;
}
static bool ScriptActivePlayer_GetHuman(TScriptActivePlayer *Self) {
    return SPRITE(Self)->Active && (PLAYER(Self)->ControlMethod == 0); /* HUMAN == 0 */
}
static int8_t ScriptActivePlayer_GetDirection(TScriptActivePlayer *Self) {
    return SPRITE(Self)->Direction;
}
static uint8_t ScriptActivePlayer_GetFlags(TScriptActivePlayer *Self) {
    return PLAYER(Self)->Flags;
}
static const char *ScriptActivePlayer_GetHWID(TScriptActivePlayer *Self) {
    return PLAYER(Self)->hwid;
}
#if defined(STEAM)
static int64_t ScriptActivePlayer_GetSteamID(TScriptActivePlayer *Self) {
    return (int64_t)PLAYER(Self)->SteamID;
}
#endif

/* Keys */
static bool ScriptActivePlayer_GetKeyUp(TScriptActivePlayer *Self) { return SPRITE(Self)->Control.Up; }
static void ScriptActivePlayer_SetKeyUp(TScriptActivePlayer *Self, bool Pressed) { if (!Self->GetHuman(Self)) SPRITE(Self)->Control.Up = Pressed; }
static bool ScriptActivePlayer_GetKeyLeft(TScriptActivePlayer *Self) { return SPRITE(Self)->Control.Left; }
static void ScriptActivePlayer_SetKeyLeft(TScriptActivePlayer *Self, bool Pressed) { if (!Self->GetHuman(Self)) SPRITE(Self)->Control.Left = Pressed; }
static bool ScriptActivePlayer_GetKeyRight(TScriptActivePlayer *Self) { return SPRITE(Self)->Control.Right; }
static void ScriptActivePlayer_SetKeyRight(TScriptActivePlayer *Self, bool Pressed) { if (!Self->GetHuman(Self)) SPRITE(Self)->Control.Right = Pressed; }
static bool ScriptActivePlayer_GetKeyShoot(TScriptActivePlayer *Self) { return SPRITE(Self)->Control.Fire; }
static void ScriptActivePlayer_SetKeyShoot(TScriptActivePlayer *Self, bool Pressed) { if (!Self->GetHuman(Self)) SPRITE(Self)->Control.Fire = Pressed; }
static bool ScriptActivePlayer_GetKeyJetpack(TScriptActivePlayer *Self) { return SPRITE(Self)->Control.Jetpack; }
static void ScriptActivePlayer_SetKeyJetpack(TScriptActivePlayer *Self, bool Pressed) { if (!Self->GetHuman(Self)) SPRITE(Self)->Control.JetPack = Pressed; }
static bool ScriptActivePlayer_GetKeyGrenade(TScriptActivePlayer *Self) { return SPRITE(Self)->Control.ThrowNade; }
static void ScriptActivePlayer_SetKeyGrenade(TScriptActivePlayer *Self, bool Pressed) { if (!Self->GetHuman(Self)) SPRITE(Self)->Control.ThrowNade = Pressed; }
static bool ScriptActivePlayer_GetKeyChangeWeap(TScriptActivePlayer *Self) { return SPRITE(Self)->Control.ChangeWeapon; }
static void ScriptActivePlayer_SetKeyChangeWeap(TScriptActivePlayer *Self, bool Pressed) { if (!Self->GetHuman(Self)) SPRITE(Self)->Control.ChangeWeapon = Pressed; }
static bool ScriptActivePlayer_GetKeyThrow(TScriptActivePlayer *Self) { return SPRITE(Self)->Control.ThrowWeapon; }
static void ScriptActivePlayer_SetKeyThrow(TScriptActivePlayer *Self, bool Pressed) { if (!Self->GetHuman(Self)) SPRITE(Self)->Control.ThrowWeapon = Pressed; }
static bool ScriptActivePlayer_GetKeyReload(TScriptActivePlayer *Self) { return SPRITE(Self)->Control.Reload; }
static void ScriptActivePlayer_SetKeyReload(TScriptActivePlayer *Self, bool Pressed) { if (!Self->GetHuman(Self)) SPRITE(Self)->Control.Reload = Pressed; }
static bool ScriptActivePlayer_GetKeyCrouch(TScriptActivePlayer *Self) { return SPRITE(Self)->Control.Down; }
static void ScriptActivePlayer_SetKeyCrouch(TScriptActivePlayer *Self, bool Pressed) { if (!Self->GetHuman(Self)) SPRITE(Self)->Control.Down = Pressed; }
static bool ScriptActivePlayer_GetKeyProne(TScriptActivePlayer *Self) { return SPRITE(Self)->Control.Prone; }
static void ScriptActivePlayer_SetKeyProne(TScriptActivePlayer *Self, bool Pressed) { if (!Self->GetHuman(Self)) SPRITE(Self)->Control.Prone = Pressed; }
static bool ScriptActivePlayer_GetKeyFlagThrow(TScriptActivePlayer *Self) { return SPRITE(Self)->Control.FlagThrow; }
static void ScriptActivePlayer_SetKeyFlagThrow(TScriptActivePlayer *Self, bool Pressed) { if (!Self->GetHuman(Self)) SPRITE(Self)->Control.FlagThrow = Pressed; }

static void ScriptActivePlayer_SetWeaponActive(TScriptActivePlayer *Self, uint8_t ID, bool Active) {
    if (!SPRITE(Self)->Active) return;
    NetworkServerFunctions_SetWeaponActive(Self->FID, ID, Active);
}

/* Methods */
static bool ScriptActivePlayer_Ban(TScriptActivePlayer *Self, int Time, const char *Reason) {
    return KickPlayer(Self->FID, true, KICK_CONSOLE, Time * 3600, Reason);
}
static void ScriptActivePlayer_Say(TScriptActivePlayer *Self, const char *Text, uint8_t MsgType) {
    if (!Self->GetActive(Self)) return;
    ServerSendStringMessage(Text, -1 /*ALL_PLAYERS*/, Self->FID, MsgType);
    char buf[512];
    snprintf(buf, sizeof(buf), "[%s] %s", Self->base.GetName(&Self->base), Text);
    MainConsole_Console(buf, CHAT_MESSAGE_COLOR);
}
static void ScriptActivePlayer_Damage(TScriptActivePlayer *Self, uint8_t Shooter, float Damage) {
    if (Shooter < 1 || Shooter > MAX_SPRITES) return; /* error handling omitted */
    if (!Self->GetActive(Self)) return;
    TVector2 A = {0,0};
    SPRITE(Self)->HealthHit(Damage, Shooter, 1, 1, A);
}
static void ScriptActivePlayer_Kill(TScriptActivePlayer *Self) {
    SPRITE(Self)->Die(NORMAL_DEATH, Self->FID, 1, -1, SPRITE(Self)->Skeleton.Pos[12]);
}
static void ScriptActivePlayer_BigText(TScriptActivePlayer *Self,
                                      uint8_t Layer, const char *Text, int Delay,
                                      int32_t Color, float Scale, int X, int Y) {
    if (!Self->GetActive(Self)) return;
    ServerSendSpecialMessage(Text, 1, Layer, Delay, Scale, (uint32_t)Color, (float)X, (float)Y, Self->FID);
}
static void ScriptActivePlayer_WorldText(TScriptActivePlayer *Self,
                                        uint8_t Layer, const char *Text, int Delay,
                                        int32_t Color, float Scale, float X, float Y) {
    if (!Self->GetActive(Self)) return;
    ServerSendSpecialMessage(Text, 2, Layer, Delay, Scale, (uint32_t)Color, X, Y, Self->FID);
}
static void ScriptActivePlayer_ForceWeapon(TScriptActivePlayer *Self,
                                           TScriptWeapon *Primary,
                                           TScriptWeapon *Secondary) {
    if (!Self->GetActive(Self)) return;
    NetworkServerFunctions_ForceWeapon(Self->FID, Primary->WType, Secondary->WType,
                                      Primary->Ammo, Secondary->Ammo);
}
static void ScriptActivePlayer_ForwardTo(TScriptActivePlayer *Self,
                                         const char *TargetIP, uint16_t TargetPort,
                                         const char *Message) {
    if (!Self->GetActive(Self)) return;
    ForwardClient(Self->FID, TargetIP, TargetPort, Message);
}
static void ScriptActivePlayer_GiveBonus(TScriptActivePlayer *Self, uint8_t BType) {
    if (!Self->GetActive(Self)) return;
    uint8_t PStyle = 0;
    struct TVector2 SPos = {0,0};
    switch (BType) {
        case 1: /* Predator */
            SPRITE(Self)->Alpha = PREDATORALPHA;
            SPRITE(Self)->BonusStyle = BONUS_PREDATOR;
            SPRITE(Self)->BonusTime = PREDATORBONUSTIME;
            SPRITE(Self)->Health = STARTHEALTH;
            PStyle = 20; break;
        case 2: /* Berserker */
            SPRITE(Self)->BonusStyle = BONUS_BERSERKER;
            SPRITE(Self)->BonusTime = BERSERKERBONUSTIME;
            SPRITE(Self)->Health = STARTHEALTH;
            PStyle = 21; break;
        case 3: /* Vest */
            SPRITE(Self)->Vest = DEFAULTVEST;
            PStyle = 19; break;
        case 4: /* Grenades */
            SPRITE(Self)->TertiaryWeapon = Guns[FRAGGRENADE];
            SPRITE(Self)->TertiaryWeapon.AmmoCount = sv_maxgrenades;
            PStyle = 17; break;
        case 5: /* Clusters */
            SPRITE(Self)->TertiaryWeapon = Guns[CLUSTERGRENADE];
            SPRITE(Self)->TertiaryWeapon.AmmoCount = CLUSTER_GRENADES;
            PStyle = 22; break;
        case 6: /* Flame god */
            SPRITE(Self)->BonusStyle = BONUS_FLAMEGOD;
            SPRITE(Self)->BonusTime = FLAMERBONUSTIME;
            SPRITE(Self)->Health = STARTHEALTH;
            PStyle = 18; break;
        default: return;
    }
    SPos.x = Self->GetX(Self);
    SPos.y = Self->GetY(Self);
    uint8_t OPStyle = PStyle;
    switch (PStyle) {
        case 1: PStyle = 5; break;   // Desert Eagle
        case 2: PStyle = 6; break;   // HK MP5
        case 3: PStyle = 7; break;   // AK 74
        case 4: PStyle = 8; break;   // Steyr AUG
        case 5: PStyle = 9; break;   // Spas 12
        case 6: PStyle = 10; break;  // Ruger77
        case 7: PStyle = 11; break;  // M79
        case 8: PStyle = 12; break;  // Barrett M82A1
        case 9: PStyle = 13; break;  // Minimi
        case 10: PStyle = 14; break; // Minigun
        case 11: PStyle = 4; break;  // USSOCOM
        case 12: PStyle = 25; break; // Combat Knife
        case 13: PStyle = 24; break; // Chainsaw
        case 14: PStyle = 26; break; // LAW
        case 15: PStyle = 27; break; // Stationary Gun
        case 16: PStyle = 16; break; // Medical Kit
        case 17: PStyle = 17; break; // Grenade Kit
        case 18: PStyle = 18; break; // Flamer Kit
        case 19: PStyle = 20; break; // Vest Kit
        case 20: PStyle = 19; break; // Predator Kit
        case 21: PStyle = 21; break; // Berserk Kit
        case 22: PStyle = 22; break; // Cluster Kit
        default: return;
    }
    int I = CreateThing(SPos, 255, PStyle, 255);
    if (I >= 0) {
        Thing[I].Active = true;
        if (OPStyle > 0 && OPStyle < 15)
            Thing[I].AmmoCount = (int)DefaultGuns[OPStyle].Ammo;
        ServerThingTaken(I, Self->FID);
        Thing[I].Kill();
        Thing[I].Active = false;
        ServerThingTaken(I, 255);
    }
}
static bool ScriptActivePlayer_Kick(TScriptActivePlayer *Self, TKickReason Reason) {
    if (!Self->GetActive(Self)) return false;
    int kickReason = 0;
    switch (Reason) {
        case TKickNoResponse: kickReason = KICK_NORESPONSE; break;
        case TKickNoCheatResponse: kickReason = KICK_NOCHEATRESPONSE; break;
        case TKickChangeTeam: kickReason = KICK_CHANGETEAM; break;
        case TKickPing: kickReason = KICK_PING; break;
        case TKickFlooding: kickReason = KICK_FLOODING; break;
        case TKickConsole: kickReason = KICK_CONSOLE; break;
        case TKickConnectionCheat: kickReason = KICK_CONNECTCHEAT; break;
        case TKickCheat: kickReason = KICK_CHEAT; break;
        case TKickLeft: kickReason = KICK_LEFTGAME; break;
        case TKickVoted: kickReason = KICK_VOTED; break;
        case TKickAC: kickReason = KICK_AC; break;
        case TKickSilent: kickReason = KICK_SILENT; break;
        default: return false;
    }
    return KickPlayer(Self->FID, false, kickReason, 0, NULL);
}
static void ScriptActivePlayer_PlaySound(TScriptActivePlayer *Self,
                                         const char *Name, float X, float Y) {
    NetworkServerFunctions_PlaySound(Self->FID, Name, X, Y);
}
static void ScriptActivePlayer_Move(TScriptActivePlayer *Self, float X, float Y) {
    if (!Self->GetActive(Self)) return;
    MovePlayer(Self->FID, X, Y);
}
static void ScriptActivePlayer_SetVelocity(TScriptActivePlayer *Self, float VelX, float VelY) {
    if (!Self->GetActive(Self)) return;
    ModifyPlayerVelocity(Self->FID, VelX, VelY);
}
static void ScriptActivePlayer_Tell(TScriptActivePlayer *Self, const char *Text) {
    if (!Self->GetActive(Self)) return;
    ServerSendStringMessage(Text, Self->FID, 255, MSGTYPE_PUB);
}
static void ScriptActivePlayer_WriteConsole(TScriptActivePlayer *Self,
                                            const char *Text, int32_t Color) {
    if (!Self->GetActive(Self)) return;
    ServerHelper_WriteConsole(Self->FID, Text, Color);
}
static void ScriptActivePlayer_ChangeTeam(TScriptActivePlayer *Self,
                                          uint8_t NewTeam, TJoinType JoinType) {
    if (!Self->GetActive(Self)) return;
    if (NewTeam > 5) return; /* error */
    if (JoinType == TJoinNormal)
        SPRITE(Self)->ChangeTeam(NewTeam, true, JOIN_NORMAL);
    else if (JoinType == TJoinSilent)
        SPRITE(Self)->ChangeTeam(NewTeam, true, JOIN_SILENT);
}

/* Constructor / Destructor */
static TScriptActivePlayer *ScriptActivePlayer_Create(struct TSprite *Sprite, uint8_t ID) {
    TScriptActivePlayer *Self = calloc(1, sizeof(TScriptActivePlayer));
    if (!Self) return NULL;
    Self->base.FSpritePtr = Sprite;
    Self->base.FPrimary = calloc(1, sizeof(TScriptWeapon));
    Self->base.FSecondary = calloc(1, sizeof(TScriptWeapon));
    Self->FID = ID;

    ScriptPlayer_InitVTable(&Self->base);
    /* Override SetTeam for active player */
    Self->base.SetTeam = (void (*)(TScriptPlayer*, uint8_t))ScriptActivePlayer_SetTeam;

    Self->GetKills = ScriptActivePlayer_GetKills;
    Self->SetKills = ScriptActivePlayer_SetKills;
    Self->GetDeaths = ScriptActivePlayer_GetDeaths;
    Self->SetDeaths = ScriptActivePlayer_SetDeaths;
    Self->GetPing = ScriptActivePlayer_GetPing;
    Self->SetTeam = ScriptActivePlayer_SetTeam;
    Self->GetActive = ScriptActivePlayer_GetActive;
    Self->GetIP = ScriptActivePlayer_GetIP;
    Self->GetPort = ScriptActivePlayer_GetPort;
    Self->GetVelX = ScriptActivePlayer_GetVelX;
    Self->GetVelY = ScriptActivePlayer_GetVelY;
    Self->GetMuted = ScriptActivePlayer_GetMuted;
    Self->SetMuted = ScriptActivePlayer_SetMuted;
    Self->GetJets = ScriptActivePlayer_GetJets;
    Self->GetGrenades = ScriptActivePlayer_GetGrenades;
    Self->GetX = ScriptActivePlayer_GetX;
    Self->GetY = ScriptActivePlayer_GetY;
    Self->GetMouseAimX = ScriptActivePlayer_GetMouseAimX;
    Self->SetMouseAimX = ScriptActivePlayer_SetMouseAimX;
    Self->GetMouseAimY = ScriptActivePlayer_GetMouseAimY;
    Self->SetMouseAimY = ScriptActivePlayer_SetMouseAimY;
    Self->GetFlagger = ScriptActivePlayer_GetFlagger;
    Self->GetTime = ScriptActivePlayer_GetTime;
    Self->GetOnGround = ScriptActivePlayer_GetOnGround;
    Self->GetProne = ScriptActivePlayer_GetProne;
    Self->GetHuman = ScriptActivePlayer_GetHuman;
    Self->GetDirection = ScriptActivePlayer_GetDirection;
    Self->GetFlags = ScriptActivePlayer_GetFlags;
    Self->GetHWID = ScriptActivePlayer_GetHWID;
#if defined(STEAM)
    Self->GetSteamID = ScriptActivePlayer_GetSteamID;
#endif
    Self->GetKeyUp = ScriptActivePlayer_GetKeyUp;
    Self->SetKeyUp = ScriptActivePlayer_SetKeyUp;
    Self->GetKeyLeft = ScriptActivePlayer_GetKeyLeft;
    Self->SetKeyLeft = ScriptActivePlayer_SetKeyLeft;
    Self->GetKeyRight = ScriptActivePlayer_GetKeyRight;
    Self->SetKeyRight = ScriptActivePlayer_SetKeyRight;
    Self->GetKeyShoot = ScriptActivePlayer_GetKeyShoot;
    Self->SetKeyShoot = ScriptActivePlayer_SetKeyShoot;
    Self->GetKeyJetpack = ScriptActivePlayer_GetKeyJetpack;
    Self->SetKeyJetpack = ScriptActivePlayer_SetKeyJetpack;
    Self->GetKeyGrenade = ScriptActivePlayer_GetKeyGrenade;
    Self->SetKeyGrenade = ScriptActivePlayer_SetKeyGrenade;
    Self->GetKeyChangeWeap = ScriptActivePlayer_GetKeyChangeWeap;
    Self->SetKeyChangeWeap = ScriptActivePlayer_SetKeyChangeWeap;
    Self->GetKeyThrow = ScriptActivePlayer_GetKeyThrow;
    Self->SetKeyThrow = ScriptActivePlayer_SetKeyThrow;
    Self->GetKeyReload = ScriptActivePlayer_GetKeyReload;
    Self->SetKeyReload = ScriptActivePlayer_SetKeyReload;
    Self->GetKeyCrouch = ScriptActivePlayer_GetKeyCrouch;
    Self->SetKeyCrouch = ScriptActivePlayer_SetKeyCrouch;
    Self->GetKeyProne = ScriptActivePlayer_GetKeyProne;
    Self->SetKeyProne = ScriptActivePlayer_SetKeyProne;
    Self->GetKeyFlagThrow = ScriptActivePlayer_GetKeyFlagThrow;
    Self->SetKeyFlagThrow = ScriptActivePlayer_SetKeyFlagThrow;
    Self->SetWeaponActive = ScriptActivePlayer_SetWeaponActive;

    Self->Ban = ScriptActivePlayer_Ban;
    Self->Say = ScriptActivePlayer_Say;
    Self->Damage = ScriptActivePlayer_Damage;
    Self->Kill = ScriptActivePlayer_Kill;
    Self->BigText = ScriptActivePlayer_BigText;
    Self->WorldText = ScriptActivePlayer_WorldText;
    Self->ForceWeapon = ScriptActivePlayer_ForceWeapon;
    Self->ForwardTo = ScriptActivePlayer_ForwardTo;
    Self->GiveBonus = ScriptActivePlayer_GiveBonus;
    Self->Kick = ScriptActivePlayer_Kick;
    Self->PlaySound = ScriptActivePlayer_PlaySound;
    Self->Move = ScriptActivePlayer_Move;
    Self->SetVelocity = ScriptActivePlayer_SetVelocity;
    Self->Tell = ScriptActivePlayer_Tell;
    Self->WriteConsole = ScriptActivePlayer_WriteConsole;
    Self->ChangeTeam = ScriptActivePlayer_ChangeTeam;

    Self->Destroy = (void (*)(TScriptActivePlayer*))ScriptActivePlayer_Destroy;
    return Self;
}
static void ScriptActivePlayer_Destroy(TScriptActivePlayer *Self) {
    if (Self->base.FPrimary) free(Self->base.FPrimary);
    if (Self->base.FSecondary) free(Self->base.FSecondary);
    free(Self);
}

/* ---------- TScriptPlayerAPI registration stub ---------- */
static void ScriptPlayerAPI_CompilerRegister(void *Compiler) {
    (void)Compiler;
}
static void ScriptPlayerAPI_RuntimeRegisterApi(void *Exec) {
    (void)Exec;
}
TScriptPlayerAPI ScriptPlayerAPI = {
    .CompilerRegister = ScriptPlayerAPI_CompilerRegister,
    .RuntimeRegisterApi = ScriptPlayerAPI_RuntimeRegisterApi
};