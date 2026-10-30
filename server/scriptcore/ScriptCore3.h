#ifndef SCRIPTCORE3_H
#define SCRIPTCORE3_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* Forward declarations of supporting types */
typedef struct TStringList TStringList;
typedef struct TMemIniFile TMemIniFile;
typedef struct TPascalExec TPascalExec;
typedef struct TPascalCompiler TPascalCompiler;
typedef struct TScriptCoreAPIAdapter TScriptCoreAPIAdapter;
typedef struct TScriptGameAPI TScriptGameAPI;
typedef struct TScriptPlayersAPI TScriptPlayersAPI;
typedef struct TScriptUnitAPI TScriptUnitAPI;
typedef struct TScriptMapAPI TScriptMapAPI;
typedef struct TScriptSpawnPointAPI TScriptSpawnPointAPI;
typedef struct TScriptFileAPI TScriptFileAPI;
typedef struct TScriptWeaponChange TScriptWeaponChange;
typedef struct TScriptFunction TScriptFunction;
typedef struct TAbstractFunction TAbstractFunction;
typedef struct TExternalFunction TExternalFunction;
typedef struct TPascalDebugger TPascalDebugger;
typedef struct TVector2 TVector2;
typedef struct TList TList;

/* Configuration object */
typedef struct {
    int8_t  FSandboxLevel;
    bool    FAllowDlls;
    bool    FAllowIniEdit;
    TStringList *FSearchPaths;
    TStringList *FDefines;
    char *FMainFileName;
} TScriptCore3Conf;

/* Main script core object */
typedef struct {
    TScriptCore3Conf FConfig;
    TPascalExec *FExec;
    char *FBytecode;
    char *FDebugSymbols;
    TList *FApi;
    char *FMainFileName;
    bool FLegacyMode;
    TScriptCoreAPIAdapter *FAdapter;
    TScriptGameAPI *FGame;
    TScriptPlayersAPI *FPlayers;
    TScriptUnitAPI *FUnit;
    TScriptMapAPI *FMap;
    bool FDebug;
    bool FGameMod;
    TScriptSpawnPointAPI *FSpawnPoint;
    TScriptFileAPI *FFile;
    TScriptWeaponChange *FOnWeaponChangeNewPrimary;
    TScriptWeaponChange *FOnWeaponChangeNewSecondary;
    /* Lock handling omitted – assume external mutex */
    void *Lock;
    char *FDir;
    char *FName;
} TScriptCore3;

/* Public API */
TScriptCore3 *CheckFunction(const char *Dir);

/* TScriptCore3Conf methods */
void TScriptCore3Conf_Create(TScriptCore3Conf *Self);
void TScriptCore3Conf_Destroy(TScriptCore3Conf *Self);
void TScriptCore3Conf_LoadConfig(TScriptCore3Conf *Self,
                                 TMemIniFile *INI,
                                 const char *ConfigSectionName,
                                 const char *SearchPathsSectionName,
                                 const char *DefinesSectionName);

/* TScriptCore3 methods */
TScriptCore3 *TScriptCore3_Create(void);
void TScriptCore3_Destroy(TScriptCore3 *Self);
bool TScriptCore3_Prepare(TScriptCore3 *Self);
void TScriptCore3_Launch(TScriptCore3 *Self);
void *TScriptCore3_CallFunc(TScriptCore3 *Self,
                            void **Params, size_t ParamCount,
                            const char *FuncName,
                            void *DefaultReturn);
void *TScriptCore3_CallEvent(TScriptCore3 *Self,
                             void *Event,
                             void **Params, size_t ParamCount);

/* Event callbacks – all return void except where noted */
void TScriptCore3_OnClockTick(TScriptCore3 *Self);
void TScriptCore3_OnIdle(TScriptCore3 *Self);
int TScriptCore3_OnRequestGame(TScriptCore3 *Self,
                               const char *Ip, const char *Hw,
                               uint16_t Port, uint8_t State,
                               bool Forwarded, const char *Password);
int8_t TScriptCore3_OnBeforeJoinTeam(TScriptCore3 *Self,
                                     uint8_t Id, uint8_t Team, uint8_t OldTeam);
void TScriptCore3_OnJoinTeam(TScriptCore3 *Self,
                             uint8_t Id, uint8_t Team, uint8_t OldTeam,
                             bool JoinGame);
void TScriptCore3_OnLeaveGame(TScriptCore3 *Self,
                              uint8_t Id, bool Kicked);
void TScriptCore3_OnBeforeMapChange(TScriptCore3 *Self,
                                    const char *Map);
void TScriptCore3_OnAfterMapChange(TScriptCore3 *Self,
                                   const char *Map);
void TScriptCore3_OnAdminConnect(TScriptCore3 *Self,
                                 const char *Ip, uint16_t Port);
void TScriptCore3_OnAdminDisconnect(TScriptCore3 *Self,
                                    const char *Ip, uint16_t Port);
void TScriptCore3_OnAdminMessage(TScriptCore3 *Self,
                                 const char *Ip, uint16_t Port,
                                 const char *Message);
void TScriptCore3_OnFlagGrab(TScriptCore3 *Self,
                             uint8_t Id, uint8_t TeamFlag,
                             bool GrabbedInBase);
void TScriptCore3_OnFlagScore(TScriptCore3 *Self,
                              uint8_t Id, uint8_t TeamFlag);
void TScriptCore3_OnFlagReturn(TScriptCore3 *Self,
                               uint8_t Id, uint8_t TeamFlag);
void TScriptCore3_OnFlagDrop(TScriptCore3 *Self,
                             uint8_t Id, uint8_t TeamFlag,
                             bool Thrown);
void TScriptCore3_OnKitPickup(TScriptCore3 *Self,
                              uint8_t Id, uint8_t KitId);
TVector2 TScriptCore3_OnBeforePlayerRespawn(TScriptCore3 *Self,
                                            uint8_t Id);
void TScriptCore3_OnAfterPlayerRespawn(TScriptCore3 *Self,
                                       uint8_t Id);
float TScriptCore3_OnPlayerDamage(TScriptCore3 *Self,
                                  uint8_t Victim, uint8_t Shooter,
                                  float Damage, uint8_t BulletID);
void TScriptCore3_OnPlayerKill(TScriptCore3 *Self,
                               uint8_t Killer, uint8_t Victim,
                               uint8_t BulletID);
void TScriptCore3_OnWeaponChange(TScriptCore3 *Self,
                                 uint8_t Id, uint8_t Primary,
                                 uint8_t Secondary,
                                 uint8_t PrimaryAmmo,
                                 uint8_t SecondaryAmmo);
bool TScriptCore3_OnVoteMapStart(TScriptCore3 *Self,
                                 uint8_t Id, const char *Map);
bool TScriptCore3_OnVoteKickStart(TScriptCore3 *Self,
                                  uint8_t Id, uint8_t Victim,
                                  const char *Reason);
void TScriptCore3_OnVoteMap(TScriptCore3 *Self,
                            uint8_t Id, const char *Map);
void TScriptCore3_OnVoteKick(TScriptCore3 *Self,
                             uint8_t Id, uint8_t Victim);
void TScriptCore3_OnPlayerSpeak(TScriptCore3 *Self,
                                uint8_t Id, const char *Text);
bool TScriptCore3_OnPlayerCommand(TScriptCore3 *Self,
                                  uint8_t Id, const char *Command);
bool TScriptCore3_OnConsoleCommand(TScriptCore3 *Self,
                                   const char *Ip, uint16_t Port,
                                   const char *Command);

/* Property accessors */
int8_t TScriptCore3_GetSandboxLevel(const TScriptCore3 *Self);
bool TScriptCore3_GetAllowIniEdit(const TScriptCore3 *Self);
bool TScriptCore3_GetDebug(const TScriptCore3 *Self);
bool TScriptCore3_GetGameMod(const TScriptCore3 *Self);
TPascalExec *TScriptCore3_GetExec(const TScriptCore3 *Self);
TList *TScriptCore3_GetApi(const TScriptCore3 *Self);

#endif /* SCRIPTCORE3_H */