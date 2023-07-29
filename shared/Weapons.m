#import <Foundation/Foundation.h>
#import "Weapons.h"

@implementation TGun
static NSArray *guns, *defaultguns;

+ (void) setDefaultGuns: (NSArray *) o
{
	defaultguns = o;
}

+ (NSArray *) getDefaultGuns
{
	return defaultguns;
}

+ (void) setGuns: (NSArray *) o
{
	guns = o;
}

+ (NSArray *) getGuns
{
	return guns;
}

- (void) setAmmo: (int) o
{
	ammo = o;
}

- (void) setAmmoCount: (int) o
{
	ammocount = o;
}

- (void) setNum: (int) o
{
	num = o;
}

- (void) setRecoil: (int) o
{
	recoil = o;
}

- (void) setFireInterval: (int) o
{
	fireinterval = o;
}

- (void) setFireIntervalPrev: (int) o
{
	fireintervalprev = o;
}

- (void) setFireIntervalCount: (int) o
{
	fireintervalcount = o;
}

- (void) setStartupTime: (int) o
{
	startuptime = o;
}

- (void) setStartupTimeCount: (int) o
{
	startuptimecount = o;
}

- (void) setReloadTime: (int) o
{
	reloadtime = o;
}

- (void) setReloadTimePrev: (int) o
{
	reloadtimeprev = o;
}

- (void) setReloadTimeCount: (int) o
{
	reloadtimecount = o;
}

- (void) setTextureNum: (int) o
{
	texturenum = o;
}

- (void) setClipTextureNum: (int) o
{
	cliptexturenum = o;
}

- (void) setClipReload: (int) o
{
	clipreload = o;
}

- (void) setClipInTime: (int) o
{
	clipintime = o;
}

- (void) setClipOutTime: (int) o
{
	clipouttime = o;
}

- (void) setNoCollision: (int) o
{
	nocollision = o;
}

- (void) setFireMode: (int) o
{
	firemode = o;
}

- (void) setTimeout: (int) o
{
	timeout = o;
}

- (void) setBulletStyle: (int) o
{
	bulletstyle = o;
}

- (void) setFireStyle: (int) o
{
	firestyle = o;
}

- (void) setBulletImageStyle: (int) o
{
	bulletimagestyle = o;
}

- (void) setSpeed: (float) o
{
	speed = o;
}

- (void) setMovementAcc: (float) o
{
	movementacc = o;
}

- (void) setFireIntervalReal: (float) o
{
	fireintervalreal = o;
}

- (void) setReloadTimeReal: (float) o
{
	reloadtimereal = o;
}

- (void) setHitMultiply: (float) o
{
	hitmultiply = o;
}

- (void) setBulletSpread: (float) o
{
	bulletspread = o;
}

- (void) setPush: (float) o
{
	push = o;
}

- (void) setInheritedVelocity: (float) o
{
	inheritedvelocity = o;
}

- (void) setModifierLegs: (float) o
{
	modifierlegs = o;
}

- (void) setModifierChest: (float) o
{
	modifierchest = o;
}

- (void) setModifierHead: (float) o
{
	modifierhead = o;
}

- (void) setBink: (short) o
{
	bink = o;
}

- (void) setName: (NSString *) o
{
	name = o;
}

- (void) setININame: (NSString *) o
{
	ininame = o;
}

- (int) getAmmo
{
	return ammo;
}

- (int) getAmmoCount
{
	return ammocount;
}

- (int) getNum
{
	return num;
}

- (int) getRecoil
{
	return recoil;
}

- (int) getFireInterval
{
	return fireinterval;
}

- (int) getFireIntervalPrev
{
	return fireintervalprev;
}

- (int) getFireIntervalCount
{
	return fireintervalcount;
}

- (int) getStartupTime
{
	return startuptime;
}

- (int) getStartupTimeCount
{
	return startuptimecount;
}

- (int) getReloadTime
{
	return reloadtime;
}

- (int) getReloadTimePrev
{
	return reloadtimeprev;
}

- (int) getReloadTimeCount
{
	return reloadtimecount;
}

- (int) getTextureNum
{
	return texturenum;
}

- (int) getClipTextureNum
{
	return cliptexturenum;
}

- (int) getClipReload
{
	return clipreload;
}

- (int) getClipInTime
{
	return clipintime;
}

- (int) getClipOutTime
{
	return clipouttime;
}

- (int) getNoCollision
{
	return nocollision;
}

- (int) getFireMode
{
	return firemode;
}

- (int) getTimeout
{
	return timeout;
}

- (int) getBulletStyle
{
	return bulletstyle;
}

- (int) getFireStyle
{
	return firestyle;
}

- (int) getBulletImageStyle
{
	return bulletimagestyle;
}

- (float) getSpeed
{
	return speed;
}

- (float) getMovementAcc
{
	return movementacc;
}

- (float) getFireIntervalReal
{
	return fireintervalreal;
}

- (float) getReloadTimeReal
{
	return reloadtimereal;
}

- (float) getHitMultiply
{
	return hitmultiply;
}

- (float) getBulletSpread
{
	return bulletspread;
}

- (float) getPush
{
	return push;
}

- (float) getInheritedVelocity
{
	return inheritedvelocity;
}

- (float) getModifierLegs
{
	return modifierlegs;
}

- (float) getModifierChest
{
	return modifierchest;
}

- (float) getModifierHead
{
	return modifierhead;
}

- (short) getBink
{
	return bink;
}

- (NSString *) getName
{
	return name;
}

- (NSString *) getININame
{
	return ininame;
}
@end

@implementation Weapons
- (void) create_weapons: (BOOL) realistic
{
	[self create_weapons_base];
	[self create_default_weapons: realistic];
}

- (void) create_default_weapons: (BOOL) realistic
{
	int weaponindex;
	TGun *gun, *defaultgun;

	if(realistic) [self create_realistic_weapons];
	else [self create_normal_weapons];

	for(weaponindex = 0; weaponindex < TOTAL_WEAPONS; weaponindex++)
	{
		gun = [guns objectAtIndex: weaponindex];
		defaultgun = [TGun new];

		[defaultgun setHitMultiply: [gun getHitMultiply]];
		[defaultgun setFireInterval: [gun getFireInterval]];
		[defaultgun setAmmo: [gun getAmmo]];
		[defaultgun setReloadTime: [gun getReloadTime]];
		[defaultgun setSpeed: [gun getSpeed]];
		[defaultgun setBulletStyle: [gun getBulletStyle]];
		[defaultgun setStartupTime: [gun getStartupTime]];
		[defaultgun setBink: [gun getBink]];
		[defaultgun setMovementAcc: [gun getMovementAcc]];
		[defaultgun setBulletSpread: [gun getBulletSpread]];
		[defaultgun setRecoil: [gun getRecoil]];
		[defaultgun setPush: [gun getPush]];
		[defaultgun setInheritedVelocity: [gun getInheritedVelocity]];
		[defaultgun setModifierLegs: [gun getModifierLegs]];
		[defaultgun setModifierChest: [gun getModifierChest]];
		[defaultgun setModifierHead: [gun getModifierHead]];

		[defaultguns addObject: defaultgun];
	}

	[TGun setDefaultGuns: defaultguns];
	[self build_weapons];
}

- (void) create_weapons_base
{
	TGun *gun = [TGun new];

	guns = [NSMutableArray new];
	[gun setName: @"Desert Eagles"];
	[gun setININame: @"Desert Eagles"];
	[gun setNum: EAGLE_NUM];
	[gun setTextureNum: GFX_WEAPONS_DEAGLES];
	[gun setClipTextureNum: GFX_WEAPONS_DEAGLES_CLIP];
	[gun setClipReload: YES];
	[gun setBulletImageStyle: GFX_WEAPONS_DEAGLES_BULLET];
	[gun setFireStyle: GFX_WEAPONS_DEAGLES_FIRE];
	[gun setFireMode: 2];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"HK MP5"];
	[gun setININame: @"HK MP5"];
	[gun setNum: MP5_NUM];
	[gun setTextureNum: GFX_WEAPONS_MP5];
	[gun setClipTextureNum: GFX_WEAPONS_MP5_CLIP];
	[gun setClipReload: YES];
	[gun setBulletImageStyle: GFX_WEAPONS_MP5_BULLET];
	[gun setFireStyle: GFX_WEAPONS_MP5_FIRE];
	[gun setFireMode: 0];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"Ak-74"];
	[gun setININame: @"Ak-74"];
	[gun setNum: AK74_NUM];
	[gun setTextureNum: GFX_WEAPONS_AK74];
	[gun setClipTextureNum: GFX_WEAPONS_AK74_CLIP];
	[gun setClipReload: YES];
	[gun setBulletImageStyle: GFX_WEAPONS_AK74_BULLET];
	[gun setFireStyle: GFX_WEAPONS_AK74_FIRE];
	[gun setFireMode: 0];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"Steyr AUG"];
	[gun setININame: @"Steyer AUG"];
	[gun setNum: STEYRAUG_NUM];
	[gun setTextureNum: GFX_WEAPONS_STEYR];
	[gun setClipTextureNum: GFX_WEAPONS_STEYR_CLIP];
	[gun setClipReload: YES];
	[gun setBulletImageStyle: GFX_WEAPONS_STEYR_BULLET];
	[gun setFireStyle: GFX_WEAPONS_STEYR_FIRE];
	[gun setFireMode: 0];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"Spas-12"];
	[gun setININame: @"Spas-12"];
	[gun setNum: SPAS12_NUM];
	[gun setTextureNum: GFX_WEAPONS_SPAS];
	[gun setClipTextureNum: 0];
	[gun setClipReload: NO];
	[gun setBulletImageStyle: 0];
	[gun setFireStyle: GFX_WEAPONS_SPAS_FIRE];
	[gun setFireMode: 2];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"Ruger 77"];
	[gun setININame: @"Ruger 77"];
	[gun setNum: RUGER77_NUM];
	[gun setTextureNum: GFX_WEAPONS_RUGER];
	[gun setClipTextureNum: 0];
	[gun setClipReload: NO];
	[gun setBulletImageStyle: GFX_WEAPONS_RUGER_BULLET];
	[gun setFireStyle: GFX_WEAPONS_RUGER_FIRE];
	[gun setFireMode: 2];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"M79"];
	[gun setININame: @"M79"];
	[gun setNum: M79_NUM];
	[gun setTextureNum: GFX_WEAPONS_M79];
	[gun setClipTextureNum: GFX_WEAPONS_M79_CLIP];
	[gun setClipReload: YES];
	[gun setBulletImageStyle: 0];
	[gun setFireStyle: GFX_WEAPONS_M79_FIRE];
	[gun setFireMode: 0];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"Barrett M82A1"];
	[gun setININame: @"Barrett M82A1"];
	[gun setNum: BARRETT_NUM];
	[gun setTextureNum: GFX_WEAPONS_BARRETT];
	[gun setClipTextureNum: GFX_WEAPONS_BARRETT_CLIP];
	[gun setClipReload: YES];
	[gun setBulletImageStyle: GFX_WEAPONS_BARRETT_BULLET];
	[gun setFireStyle: GFX_WEAPONS_BARRETT_FIRE];
	[gun setFireMode: 2];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"FN Minimi"];
	[gun setININame: @"FN Minimi"];
	[gun setNum: M249_NUM];
	[gun setTextureNum: GFX_WEAPONS_MINIMI];
	[gun setClipTextureNum: GFX_WEAPONS_MINIMI_CLIP];
	[gun setClipReload: YES];
	[gun setBulletImageStyle: GFX_WEAPONS_MINIMI_BULLET];
	[gun setFireStyle: GFX_WEAPONS_MINIMI_FIRE];
	[gun setFireMode: 0];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"XM214 Minigun"];
	[gun setININame: @"XM214 Minigun"];
	[gun setNum: MINIGUN_NUM];
	[gun setTextureNum: GFX_WEAPONS_MINIGUN];
	[gun setClipTextureNum: GFX_WEAPONS_MINIGUN_CLIP];
	[gun setClipReload: NO];
	[gun setBulletImageStyle: GFX_WEAPONS_MINIGUN_BULLET];
	[gun setFireStyle: GFX_WEAPONS_MINIGUN_FIRE];
	[gun setFireMode: 0];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"USSOCOM"];
	[gun setININame: @"USSOCOM"];
	[gun setNum: COLT_NUM];
	[gun setTextureNum: GFX_WEAPONS_SOCOM];
	[gun setClipTextureNum: GFX_WEAPONS_SOCOM_CLIP];
	[gun setClipReload: YES];
	[gun setBulletImageStyle: GFX_WEAPONS_SOCOM_BULLET];
	[gun setFireStyle: GFX_WEAPONS_SOCOM_FIRE];
	[gun setFireMode: 2];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"Combat Knife"];
	[gun setININame: @"Combat Knife"];
	[gun setNum: KNIFE_NUM];
	[gun setTextureNum: GFX_WEAPONS_KNIFE];
	[gun setClipTextureNum: 0];
	[gun setClipReload: NO];
	[gun setBulletImageStyle: 0];
	[gun setFireStyle: 0];
	[gun setFireMode: 0];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"Chainsaw"];
	[gun setININame: @"Chainsaw"];
	[gun setNum: CHAINSAW_NUM];
	[gun setTextureNum: GFX_WEAPONS_CHAINSAW];
	[gun setClipTextureNum: 0];
	[gun setClipReload: NO];
	[gun setBulletImageStyle: 0];
	[gun setFireStyle: GFX_WEAPONS_DEAGLES_FIRE];
	[gun setFireMode: 0];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"M72 LAW"];
	[gun setININame: @"M72 LAW"];
	[gun setNum: LAW_NUM];
	[gun setTextureNum: GFX_WEAPONS_LAW];
	[gun setClipTextureNum: 0];
	[gun setClipReload: YES];
	[gun setBulletImageStyle: 0];
	[gun setFireStyle: GFX_WEAPONS_LAW_FIRE];
	[gun setFireMode: 0];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"Flame Bow"];
	[gun setININame: @"Flame Bow"];
	[gun setNum: BOW2_NUM];
	[gun setTextureNum: GFX_WEAPONS_BOW];
	[gun setClipTextureNum: GFX_WEAPONS_BOW_S];
	[gun setClipReload: NO];
	[gun setBulletImageStyle: 0];
	[gun setFireStyle: GFX_WEAPONS_BOW_FIRE];
	[gun setFireMode: 0];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"Bow"];
	[gun setININame: @"Bow"];
	[gun setNum: BOW_NUM];
	[gun setTextureNum: GFX_WEAPONS_BOW];
	[gun setClipTextureNum: GFX_WEAPONS_BOW_S];
	[gun setClipReload: NO];
	[gun setBulletImageStyle: 0];
	[gun setFireStyle: GFX_WEAPONS_BOW_FIRE];
	[gun setFireMode: 0];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"Flamer"];
	[gun setININame: @"Flamer"];
	[gun setNum: FLAMER_NUM];
	[gun setTextureNum: GFX_WEAPONS_FLAMER];
	[gun setClipTextureNum: GFX_WEAPONS_FLAMER];
	[gun setClipReload: NO];
	[gun setBulletImageStyle: 0];
	[gun setFireStyle: GFX_WEAPONS_FLAMER_FIRE];
	[gun setFireMode: 0];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"M2"];
	[gun setININame: @"M2"];
	[gun setNum: M2_NUM];
	[gun setTextureNum: GFX_WEAPONS_MINIGUN];
	[gun setClipTextureNum: 0];
	[gun setClipReload: NO];
	[gun setBulletImageStyle: 0];
	[gun setFireStyle: 0];
	[gun setFireMode: 0];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"Punch"];
	[gun setININame: @"Punch"];
	[gun setNum: NOWEAPON_NUM];
	[gun setTextureNum: 0];
	[gun setClipTextureNum: 0];
	[gun setClipReload: NO];
	[gun setBulletImageStyle: 0];
	[gun setFireStyle: 0];
	[gun setFireMode: 0];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"Frag Grenade"];
	[gun setININame: @"Frag Grenade"];
	[gun setNum: FRAGGRENADE_NUM];
	[gun setTextureNum: GFX_WEAPONS_FRAG_GRENADE];
	[gun setClipTextureNum: GFX_WEAPONS_FRAG_GRENADE];
	[gun setClipReload: NO];
	[gun setBulletImageStyle: 0];
	[gun setFireStyle: GFX_WEAPONS_AK74_FIRE];
	[gun setFireMode: 0];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"Frag Grenade"];
	[gun setININame: @"Frag Grenade"];
	[gun setNum: CLUSTERGRENADE_NUM];
	[gun setTextureNum: GFX_WEAPONS_FRAG_GRENADE];
	[gun setClipTextureNum: GFX_WEAPONS_FRAG_GRENADE];
	[gun setClipReload: NO];
	[gun setBulletImageStyle: 0];
	[gun setFireStyle: GFX_WEAPONS_AK74_FIRE];
	[gun setFireMode: 0];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"Frag Grenade"];
	[gun setININame: @"Frag Grenade"];
	[gun setNum: CLUSTER_NUM];
	[gun setTextureNum: GFX_WEAPONS_FRAG_GRENADE];
	[gun setClipTextureNum: GFX_WEAPONS_FRAG_GRENADE];
	[gun setClipReload: NO];
	[gun setBulletImageStyle: 0];
	[gun setFireStyle: GFX_WEAPONS_AK74_FIRE];
	[gun setFireMode: 0];
	[guns addObject: gun];

	gun = [TGun new];
	[gun setName: @"Combat Knife"];
	[gun setININame: @"Combat Knife"];
	[gun setNum: THROWNKNIFE_NUM];
	[gun setTextureNum: GFX_WEAPONS_KNIFE];
	[gun setClipTextureNum: 0];
	[gun setClipReload: NO];
	[gun setBulletImageStyle: 0];
	[gun setFireStyle: 0];
	[gun setFireMode: 0];
	[guns addObject: gun];

	[TGun setGuns: [guns copy]];
}

- (void) create_normal_weapons
{
	TGun *gun = [guns objectAtIndex: EAGLE];

	[gun setHitMultiply: 1.81];
	[gun setFireInterval: 24];
	[gun setAmmo: 7];
	[gun setReloadTime: 87];
	[gun setSpeed: 19];
	[gun setBulletStyle: BULLET_STYLE_PLAIN];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0.009];
	[gun setBulletSpread: 0.15];
	[gun setRecoil: 0];
	[gun setPush: 0.0176];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 0.95];
	[gun setModifierLegs: 0.85];

	gun = [guns objectAtIndex: MP5];
	[gun setHitMultiply: 1.01];
	[gun setFireInterval: 6];
	[gun setAmmo: 30];
	[gun setReloadTime: 105];
	[gun setSpeed: 18.9];
	[gun setBulletStyle: BULLET_STYLE_PLAIN];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0];
	[gun setBulletSpread: 0.14];
	[gun setRecoil: 0];
	[gun setPush: 0.0112];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 0.95];
	[gun setModifierLegs: 0.85];

	gun = [guns objectAtIndex: AK74];
	[gun setHitMultiply: 1.004];
	[gun setFireInterval: 10];
	[gun setAmmo: 35];
	[gun setReloadTime: 165];
	[gun setSpeed: 24.6];
	[gun setBulletStyle: BULLET_STYLE_PLAIN];
	[gun setStartupTime: 0];
	[gun setBink: -12];
	[gun setMovementAcc: 0.011];
	[gun setBulletSpread: 0.025];
	[gun setRecoil: 0];
	[gun setPush: 0.01376];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 0.95];
	[gun setModifierLegs: 0.85];

	gun = [guns objectAtIndex: STEYRAUG];
	[gun setHitMultiply: 0.71];
	[gun setFireInterval: 7];
	[gun setAmmo: 25];
	[gun setReloadTime: 125];
	[gun setSpeed: 26];
	[gun setBulletStyle: BULLET_STYLE_PLAIN];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0];
	[gun setBulletSpread: 0.075];
	[gun setRecoil: 0];
	[gun setPush: 0.0084];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 0.95];
	[gun setModifierLegs: 0.85];

	gun = [guns objectAtIndex: SPAS12];
	[gun setHitMultiply: 1.22];
	[gun setFireInterval: 32];
	[gun setAmmo: 7];
	[gun setReloadTime: 175];
	[gun setSpeed: 14];
	[gun setBulletStyle: BULLET_STYLE_SHOTGUN];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0];
	[gun setBulletSpread: 0.8];
	[gun setRecoil: 0];
	[gun setPush: 0.0188];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 0.95];
	[gun setModifierLegs: 0.85];

	gun = [guns objectAtIndex: RUGER77];
	[gun setHitMultiply: 2.49];
	[gun setFireInterval: 45];
	[gun setAmmo: 4];
	[gun setReloadTime: 78];
	[gun setSpeed: 33];
	[gun setBulletStyle: BULLET_STYLE_PLAIN];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0.03];
	[gun setBulletSpread: 0];
	[gun setRecoil: 0];
	[gun setPush: 0.012];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.2];
	[gun setModifierChest: 1.05];
	[gun setModifierLegs: 1];

	gun = [guns objectAtIndex: M79];
	[gun setHitMultiply: 1550];
	[gun setFireInterval: 6];
	[gun setAmmo: 1];
	[gun setReloadTime: 178];
	[gun setSpeed: 10.7];
	[gun setBulletStyle: BULLET_STYLE_M79];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0];
	[gun setBulletSpread: 0];
	[gun setRecoil: 0];
	[gun setPush: 0.036];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.15];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.9];

	gun = [guns objectAtIndex: BARRETT];
	[gun setHitMultiply: 4.45];
	[gun setFireInterval: 225];
	[gun setAmmo: 10];
	[gun setReloadTime: 70];
	[gun setSpeed: 55];
	[gun setBulletStyle: BULLET_STYLE_PLAIN];
	[gun setStartupTime: 19];
	[gun setBink: 65];
	[gun setMovementAcc: 0.05];
	[gun setBulletSpread: 0];
	[gun setRecoil: 0];
	[gun setPush: 0.018];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 1];

	gun = [guns objectAtIndex: M249];
	[gun setHitMultiply: 0.85];
	[gun setFireInterval: 9];
	[gun setAmmo: 50];
	[gun setReloadTime: 250];
	[gun setSpeed: 27];
	[gun setBulletStyle: BULLET_STYLE_PLAIN];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0.013];
	[gun setBulletSpread: 0.064];
	[gun setRecoil: 0];
	[gun setPush: 0.0128];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 0.95];
	[gun setModifierLegs: 0.85];

	gun = [guns objectAtIndex: MINIGUN];
	[gun setHitMultiply: 0.468];
	[gun setFireInterval: 3];
	[gun setAmmo: 100];
	[gun setReloadTime: 480];
	[gun setSpeed: 29];
	[gun setBulletStyle: BULLET_STYLE_PLAIN];
	[gun setStartupTime: 25];
	[gun setBink: 0];
	[gun setMovementAcc: 0.0625];
	[gun setBulletSpread: 0.3];
	[gun setRecoil: 0];
	[gun setPush: 0.0104];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 0.95];
	[gun setModifierLegs: 0.85];

	gun = [guns objectAtIndex: COLT];
	[gun setHitMultiply: 1.49];
	[gun setFireInterval: 10];
	[gun setAmmo: 14];
	[gun setReloadTime: 60];
	[gun setSpeed: 18];
	[gun setBulletStyle: BULLET_STYLE_PLAIN];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0];
	[gun setBulletSpread: 0];
	[gun setRecoil: 0];
	[gun setPush: 0.2];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 0.95];
	[gun setModifierLegs: 0.85];

	gun = [guns objectAtIndex: KNIFE];
	[gun setHitMultiply: 2150];
	[gun setFireInterval: 6];
	[gun setAmmo: 1];
	[gun setReloadTime: 3];
	[gun setSpeed: 6];
	[gun setBulletStyle: BULLET_STYLE_KNIFE];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0];
	[gun setBulletSpread: 0];
	[gun setRecoil: 0];
	[gun setPush: 0.12];
	[gun setInheritedVelocity: 0];
	[gun setModifierHead: 1.15];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.9];

	gun = [guns objectAtIndex: CHAINSAW];
	[gun setHitMultiply: 50];
	[gun setFireInterval: 2];
	[gun setAmmo: 200];
	[gun setReloadTime: 110];
	[gun setSpeed: 8];
	[gun setBulletStyle: BULLET_STYLE_KNIFE];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0];
	[gun setBulletSpread: 0];
	[gun setRecoil: 0];
	[gun setPush: 0.0028];
	[gun setInheritedVelocity: 0];
	[gun setModifierHead: 1.15];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.9];

	gun = [guns objectAtIndex: LAW];
	[gun setHitMultiply: 1550];
	[gun setFireInterval: 6];
	[gun setAmmo: 1];
	[gun setReloadTime: 300];
	[gun setSpeed: 23];
	[gun setBulletStyle: BULLET_STYLE_LAW];
	[gun setStartupTime: 13];
	[gun setBink: 0];
	[gun setMovementAcc: 0];
	[gun setBulletSpread: 0];
	[gun setRecoil: 0];
	[gun setPush: 0.028];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.15];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.9];

	gun = [guns objectAtIndex: BOW2];
	[gun setHitMultiply: 8];
	[gun setFireInterval: 10];
	[gun setAmmo: 1];
	[gun setReloadTime: 39];
	[gun setSpeed: 18];
	[gun setBulletStyle: BULLET_STYLE_FLAMEARROW];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0];
	[gun setBulletSpread: 0];
	[gun setRecoil: 0];
	[gun setPush: 0];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.15];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.9];

	gun = [guns objectAtIndex: BOW];
	[gun setHitMultiply: 12];
	[gun setFireInterval: 10];
	[gun setAmmo: 1];
	[gun setReloadTime: 25];
	[gun setSpeed: 21];
	[gun setBulletStyle: BULLET_STYLE_ARROW];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0];
	[gun setBulletSpread: 0];
	[gun setRecoil: 0];
	[gun setPush: 0.0148];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.15];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.9];

	gun = [guns objectAtIndex: FLAMER];
	[gun setHitMultiply: 19];
	[gun setFireInterval: 6];
	[gun setAmmo: 200];
	[gun setReloadTime: 5];
	[gun setSpeed: 10.5];
	[gun setBulletStyle: BULLET_STYLE_FLAME];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0];
	[gun setBulletSpread: 0];
	[gun setRecoil: 0];
	[gun setPush: 0.016];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.15];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.9];

	gun = [guns objectAtIndex: M2];
	[gun setHitMultiply: 1.8];
	[gun setFireInterval: 10];
	[gun setAmmo: 100];
	[gun setReloadTime: 366];
	[gun setSpeed: 36];
	[gun setBulletStyle: BULLET_STYLE_M2];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0];
	[gun setBulletSpread: 0];
	[gun setRecoil: 0];
	[gun setPush: 0.0088];
	[gun setInheritedVelocity: 0];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 0.95];
	[gun setModifierLegs: 0.85];

	gun = [guns objectAtIndex: NOWEAPON];
	[gun setHitMultiply: 330];
	[gun setFireInterval: 6];
	[gun setAmmo: 1];
	[gun setReloadTime: 3];
	[gun setSpeed: 5];
	[gun setBulletStyle: BULLET_STYLE_PUNCH];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0];
	[gun setBulletSpread: 0];
	[gun setRecoil: 0];
	[gun setPush: 0];
	[gun setInheritedVelocity: 0];
	[gun setModifierHead: 1.15];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.9];

	gun = [guns objectAtIndex: FRAGGRENADE];
	[gun setHitMultiply: 1500];
	[gun setFireInterval: 80];
	[gun setAmmo: 1];
	[gun setReloadTime: 20];
	[gun setSpeed: 5];
	[gun setBulletStyle: BULLET_STYLE_FRAGNADE];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0];
	[gun setBulletSpread: 0];
	[gun setRecoil: 0];
	[gun setPush: 0];
	[gun setInheritedVelocity: 1];
	[gun setModifierHead: 1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 1];
}

- (void) create_realistic_weapons
{
	TGun *gun = [guns objectAtIndex: EAGLE];

	[gun setHitMultiply: 1.66];
	[gun setFireInterval: 27];
	[gun setAmmo: 7];
	[gun setReloadTime: 106];
	[gun setSpeed: 19];
	[gun setBulletStyle: BULLET_STYLE_PLAIN];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0.02];
	[gun setBulletSpread: 0.1];
	[gun setRecoil: 55];
	[gun setPush: 0.0164];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.6];

	gun = [guns objectAtIndex: MP5];
	[gun setHitMultiply: 0.94];
	[gun setFireInterval: 6];
	[gun setAmmo: 30];
	[gun setReloadTime: 110];
	[gun setSpeed: 18.9];
	[gun setBulletStyle: BULLET_STYLE_PLAIN];
	[gun setStartupTime: 0];
	[gun setBink: -10];
	[gun setMovementAcc: 0.01];
	[gun setBulletSpread: 0.03];
	[gun setRecoil: 9];
	[gun setPush: 0.0164];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.6];

	gun = [guns objectAtIndex: AK74];
	[gun setHitMultiply: 1.08];
	[gun setFireInterval: 11];
	[gun setAmmo: 35];
	[gun setReloadTime: 158];
	[gun setSpeed: 24];
	[gun setBulletStyle: BULLET_STYLE_PLAIN];
	[gun setStartupTime: 0];
	[gun setBink: -10];
	[gun setMovementAcc: 0.02];
	[gun setBulletSpread: 0];
	[gun setRecoil: 13];
	[gun setPush: 0.0132];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.6];

	gun = [guns objectAtIndex: STEYRAUG];
	[gun setHitMultiply: 0.68];
	[gun setFireInterval: 7];
	[gun setAmmo: 30];
	[gun setReloadTime: 126];
	[gun setSpeed: 26];
	[gun setBulletStyle: BULLET_STYLE_PLAIN];
	[gun setStartupTime: 0];
	[gun setBink: -9];
	[gun setMovementAcc: 0.01];
	[gun setBulletSpread: 0];
	[gun setRecoil: 11];
	[gun setPush: 0.012];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.6];

	gun = [guns objectAtIndex: SPAS12];
	[gun setHitMultiply: 1.2];
	[gun setFireInterval: 35];
	[gun setAmmo: 7];
	[gun setReloadTime: 175];
	[gun setSpeed: 13.2];
	[gun setBulletStyle: BULLET_STYLE_SHOTGUN];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0.01];
	[gun setBulletSpread: 0.8];
	[gun setRecoil: 65];
	[gun setPush: 0.0224];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.6];

	gun = [guns objectAtIndex: RUGER77];
	[gun setHitMultiply: 2.22];
	[gun setFireInterval: 52];
	[gun setAmmo: 4];
	[gun setReloadTime: 104];
	[gun setSpeed: 33];
	[gun setBulletStyle: BULLET_STYLE_PLAIN];
	[gun setStartupTime: 0];
	[gun setBink: 14];
	[gun setMovementAcc: 0.03];
	[gun setBulletSpread: 0];
	[gun setRecoil: 54];
	[gun setPush: 0.0096];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.6];

	gun = [guns objectAtIndex: M79];
	[gun setHitMultiply: 1600];
	[gun setFireInterval: 6];
	[gun setAmmo: 1];
	[gun setReloadTime: 173];
	[gun setSpeed: 11.4];
	[gun setBulletStyle: BULLET_STYLE_M79];
	[gun setStartupTime: 0];
	[gun setBink: 45];
	[gun setMovementAcc: 0.03];
	[gun setBulletSpread: 0];
	[gun setRecoil: 420];
	[gun setPush: 0.024];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.6];

	gun = [guns objectAtIndex: BARRETT];
	[gun setHitMultiply: 4.95];
	[gun setFireInterval: 200];
	[gun setAmmo: 10];
	[gun setReloadTime: 170];
	[gun setSpeed: 55];
	[gun setBulletStyle: BULLET_STYLE_PLAIN];
	[gun setStartupTime: 16];
	[gun setBink: 80];
	[gun setMovementAcc: 0.07];
	[gun setBulletSpread: 0];
	[gun setRecoil: 0];
	[gun setPush: 0.0056];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.6];

	gun = [guns objectAtIndex: M249];
	[gun setHitMultiply: 0.81];
	[gun setFireInterval: 10];
	[gun setAmmo: 50];
	[gun setReloadTime: 261];
	[gun setSpeed: 27];
	[gun setBulletStyle: BULLET_STYLE_PLAIN];
	[gun setStartupTime: 0];
	[gun setBink: -8];
	[gun setMovementAcc: 0.02];
	[gun setBulletSpread: 0];
	[gun setRecoil: 8];
	[gun setPush: 0.0116];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.6];

	gun = [guns objectAtIndex: MINIGUN];
	[gun setHitMultiply: 0.43];
	[gun setFireInterval: 4];
	[gun setAmmo: 100];
	[gun setReloadTime: 320];
	[gun setSpeed: 29];
	[gun setBulletStyle: BULLET_STYLE_PLAIN];
	[gun setStartupTime: 33];
	[gun setBink: -2];
	[gun setMovementAcc: 0.01];
	[gun setBulletSpread: 0.1];
	[gun setRecoil: 4];
	[gun setPush: 0.0108];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.6];

	gun = [guns objectAtIndex: COLT];
	[gun setHitMultiply: 1.30];
	[gun setFireInterval: 12];
	[gun setAmmo: 12];
	[gun setReloadTime: 72];
	[gun setSpeed: 18];
	[gun setBulletStyle: BULLET_STYLE_PLAIN];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0.02];
	[gun setBulletSpread: 0];
	[gun setRecoil: 28];
	[gun setPush: 0.0172];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.6];

	gun = [guns objectAtIndex: KNIFE];
	[gun setHitMultiply: 2250];
	[gun setFireInterval: 6];
	[gun setAmmo: 1];
	[gun setReloadTime: 3];
	[gun setSpeed: 6];
	[gun setBulletStyle: BULLET_STYLE_KNIFE];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0.01];
	[gun setBulletSpread: 0];
	[gun setRecoil: 10];
	[gun setPush: 0.028];
	[gun setInheritedVelocity: 0];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.6];

	gun = [guns objectAtIndex: CHAINSAW];
	[gun setHitMultiply: 21];
	[gun setFireInterval: 2];
	[gun setAmmo: 200];
	[gun setReloadTime: 110];
	[gun setSpeed: 7.6];
	[gun setBulletStyle: BULLET_STYLE_KNIFE];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0.01];
	[gun setBulletSpread: 0];
	[gun setRecoil: 1];
	[gun setPush: 0.0028];
	[gun setInheritedVelocity: 0];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.6];

	gun = [guns objectAtIndex: LAW];
	[gun setHitMultiply: 1500];
	[gun setFireInterval: 30];
	[gun setAmmo: 1];
	[gun setReloadTime: 495];
	[gun setSpeed: 23];
	[gun setBulletStyle: BULLET_STYLE_LAW];
	[gun setStartupTime: 12];
	[gun setBink: 0];
	[gun setMovementAcc: 0.01];
	[gun setBulletSpread: 0];
	[gun setRecoil: 9];
	[gun setPush: 0.012];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.6];

	gun = [guns objectAtIndex: BOW2];
	[gun setHitMultiply: 8];
	[gun setFireInterval: 10];
	[gun setAmmo: 1];
	[gun setReloadTime: 39];
	[gun setSpeed: 18];
	[gun setBulletStyle: BULLET_STYLE_FLAMEARROW];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0.01];
	[gun setBulletSpread: 0];
	[gun setRecoil: 10];
	[gun setPush: 0];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.6];

	gun = [guns objectAtIndex: BOW];
	[gun setHitMultiply: 12];
	[gun setFireInterval: 10];
	[gun setAmmo: 1];
	[gun setReloadTime: 25];
	[gun setSpeed: 21];
	[gun setBulletStyle: BULLET_STYLE_ARROW];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0.01];
	[gun setBulletSpread: 0];
	[gun setRecoil: 10];
	[gun setPush: 0.0148];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.6];

	gun = [guns objectAtIndex: FLAMER];
	[gun setHitMultiply: 12];
	[gun setFireInterval: 6];
	[gun setAmmo: 200];
	[gun setReloadTime: 5];
	[gun setSpeed: 12.5];
	[gun setBulletStyle: BULLET_STYLE_FLAME];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0.01];
	[gun setBulletSpread: 0];
	[gun setRecoil: 10];
	[gun setPush: 0.016];
	[gun setInheritedVelocity: 0.5];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.6];

	gun = [guns objectAtIndex: M2];
	[gun setHitMultiply: 1.55];
	[gun setFireInterval: 14];
	[gun setAmmo: 100];
	[gun setReloadTime: 366];
	[gun setSpeed: 36];
	[gun setBulletStyle: BULLET_STYLE_M2];
	[gun setStartupTime: 21];
	[gun setBink: 0];
	[gun setMovementAcc: 0.01];
	[gun setBulletSpread: 0];
	[gun setRecoil: 10];
	[gun setPush: 0.0088];
	[gun setInheritedVelocity: 0];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.6];

	gun = [guns objectAtIndex: NOWEAPON];
	[gun setHitMultiply: 330];
	[gun setFireInterval: 6];
	[gun setAmmo: 1];
	[gun setReloadTime: 3];
	[gun setSpeed: 5];
	[gun setBulletStyle: BULLET_STYLE_PUNCH];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0.01];
	[gun setBulletSpread: 0];
	[gun setRecoil: 10];
	[gun setPush: 0];
	[gun setInheritedVelocity: 0];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.6];

	gun = [guns objectAtIndex: FRAGGRENADE];
	[gun setHitMultiply: 1500];
	[gun setFireInterval: 80];
	[gun setAmmo: 1];
	[gun setReloadTime: 20];
	[gun setSpeed: 5];
	[gun setBulletStyle: BULLET_STYLE_FRAGNADE];
	[gun setStartupTime: 0];
	[gun setBink: 0];
	[gun setMovementAcc: 0.01];
	[gun setBulletSpread: 0];
	[gun setRecoil: 10];
	[gun setPush: 0];
	[gun setInheritedVelocity: 1];
	[gun setModifierHead: 1.1];
	[gun setModifierChest: 1];
	[gun setModifierLegs: 0.6];
}

- (void) build_weapons
{
	int weaponindex;
	TGun *gun = [guns objectAtIndex: CLUSTERGRENADE];

	[gun setHitMultiply: [[guns objectAtIndex: FRAGGRENADE] getHitMultiply]];
	[gun setFireInterval: [[guns objectAtIndex: FRAGGRENADE] getFireInterval]];
	[gun setAmmo: [[guns objectAtIndex: FRAGGRENADE] getAmmo]];
	[gun setReloadTime: [[guns objectAtIndex: FRAGGRENADE] getReloadTime]];
	[gun setSpeed: [[guns objectAtIndex: FRAGGRENADE] getSpeed]];
	[gun setBulletStyle: BULLET_STYLE_CLUSTERNADE];
	[gun setStartupTime: [[guns objectAtIndex: FRAGGRENADE] getStartupTime]];
	[gun setBink: [[guns objectAtIndex: FRAGGRENADE] getBink]];
	[gun setMovementAcc: [[guns objectAtIndex: FRAGGRENADE] getMovementAcc]];
	[gun setBulletSpread: [[guns objectAtIndex: FRAGGRENADE] getBulletSpread]];
	[gun setRecoil: [[guns objectAtIndex: FRAGGRENADE] getRecoil]];
	[gun setPush: [[guns objectAtIndex: FRAGGRENADE] getPush]];
	[gun setInheritedVelocity: [[guns objectAtIndex: FRAGGRENADE] getInheritedVelocity]];

	gun = [guns objectAtIndex: CLUSTER];
	[gun setHitMultiply: [[guns objectAtIndex: CLUSTERGRENADE] getHitMultiply]];
	[gun setFireInterval: [[guns objectAtIndex: CLUSTERGRENADE] getFireInterval]];
	[gun setAmmo: [[guns objectAtIndex: CLUSTERGRENADE] getAmmo]];
	[gun setReloadTime: [[guns objectAtIndex: CLUSTERGRENADE] getReloadTime]];
	[gun setSpeed: [[guns objectAtIndex: CLUSTERGRENADE] getSpeed]];
	[gun setBulletStyle: BULLET_STYLE_CLUSTER];
	[gun setStartupTime: [[guns objectAtIndex: CLUSTERGRENADE] getStartupTime]];
	[gun setBink: [[guns objectAtIndex: CLUSTERGRENADE] getBink]];
	[gun setMovementAcc: [[guns objectAtIndex: CLUSTERGRENADE] getMovementAcc]];
	[gun setBulletSpread: [[guns objectAtIndex: CLUSTERGRENADE] getBulletSpread]];
	[gun setRecoil: [[guns objectAtIndex: CLUSTERGRENADE] getRecoil]];
	[gun setPush: [[guns objectAtIndex: CLUSTERGRENADE] getPush]];
	[gun setInheritedVelocity: [[guns objectAtIndex: CLUSTERGRENADE] getInheritedVelocity]];

	gun = [guns objectAtIndex: THROWNKNIFE];
	[gun setHitMultiply: [[guns objectAtIndex: KNIFE] getHitMultiply]];
	[gun setFireInterval: [[guns objectAtIndex: KNIFE] getFireInterval]];
	[gun setAmmo: [[guns objectAtIndex: KNIFE] getAmmo]];
	[gun setReloadTime: [[guns objectAtIndex: KNIFE] getReloadTime]];
	[gun setSpeed: [[guns objectAtIndex: KNIFE] getSpeed]];
	[gun setBulletStyle: BULLET_STYLE_THROWNKNIFE];
	[gun setStartupTime: [[guns objectAtIndex: KNIFE] getStartupTime]];
	[gun setBink: [[guns objectAtIndex: KNIFE] getBink]];
	[gun setMovementAcc: [[guns objectAtIndex: KNIFE] getMovementAcc]];
	[gun setBulletSpread: [[guns objectAtIndex: KNIFE] getBulletSpread]];
	[gun setRecoil: [[guns objectAtIndex: KNIFE] getRecoil]];
	[gun setPush: [[guns objectAtIndex: KNIFE] getPush]];
	[gun setInheritedVelocity: [[guns objectAtIndex: KNIFE] getInheritedVelocity]];

	for(weaponindex = 0; weaponindex < TOTAL_WEAPONS; weaponindex++)
	{
		gun = [guns objectAtIndex: weaponindex];

		[gun setFireIntervalPrev: [gun getFireInterval]];
		[gun setFireIntervalCount: [gun getFireInterval]];
		[gun setAmmoCount: [gun getAmmo]];
		[gun setReloadTimePrev: [gun getReloadTime]];
		[gun setReloadTimeCount: [gun getReloadTime]];
		[gun setStartupTimeCount: [gun getStartupTime]];

		if([gun getClipReload])
		{
			[gun setClipOutTime: trunc([gun getReloadTime] * 0.8)];
			[gun setClipInTime: trunc([gun getReloadTime] * 0.3)];
		}
		else
		{
			[gun setClipOutTime: 0];
			[gun setClipInTime: 0];
		}

		switch([gun getBulletStyle])
		{
			case BULLET_STYLE_FRAGNADE:
			case BULLET_STYLE_CLUSTERNADE:
				[gun setTimeout: GRENADE_TIMEOUT];
				break;
			case BULLET_STYLE_FLAME:
				[gun setTimeout: FLAME_TIMEOUT];
				break;
			case BULLET_STYLE_PUNCH:
			case BULLET_STYLE_KNIFE:
				[gun setTimeout: MELEE_TIMEOUT];
				break;
			case BULLET_STYLE_M2:
				[gun setTimeout: M2BULLET_TIMEOUT];
				break;
			default: [gun setTimeout: BULLET_TIMEOUT];
		}
	}

	[[guns objectAtIndex: M79] setAmmoCount: 0];
}

- (unsigned int) create_wm_checksum
{
	int weaponindex;
	unsigned int hash = 5381;
	TGun *gun;

	for(weaponindex = 0; weaponindex < ORIGINAL_WEAPONS; weaponindex++)
	{
		gun = [guns objectAtIndex: weaponindex];

		hash <<= 5 + (unsigned int) round(1000.0 * [gun getHitMultiply]);
		hash <<= 5 + (unsigned int) round(1000.0 * [gun getFireInterval]);
		hash <<= 5 + (unsigned int) round(1000.0 * [gun getAmmo]);
		hash <<= 5 + (unsigned int) round(1000.0 * [gun getReloadTime]);
		hash <<= 5 + (unsigned int) round(1000.0 * [gun getSpeed]);
		hash <<= 5 + (unsigned int) round(1000.0 * [gun getBulletStyle]);
		hash <<= 5 + (unsigned int) round(1000.0 * [gun getStartupTime]);
		hash <<= 5 + (unsigned int) round(1000.0 * [gun getBink]);
		hash <<= 5 + (unsigned int) round(1000.0 * [gun getMovementAcc]);
		hash <<= 5 + (unsigned int) round(1000.0 * [gun getBulletSpread]);
		hash <<= 5 + (unsigned int) round(1000.0 * [gun getRecoil]);
		hash <<= 5 + (unsigned int) round(1000.0 * [gun getPush]);
		hash <<= 5 + (unsigned int) round(1000.0 * [gun getInheritedVelocity]);
	}

	return hash;
}

- (int) weapon_num_to_index: (int) num
{
	int ret = -1, weaponindex;

	for(weaponindex = 0; weaponindex < TOTAL_WEAPONS; weaponindex++)
		if(num == [[guns objectAtIndex: weaponindex] getNum]) return ret = weaponindex;

	return ret;
}

- (int) weapon_name_to_num: (NSString *) name
{
	int i, ret = -1;

	for(i = 0; i < TOTAL_WEAPONS; i++)
		if([[[guns objectAtIndex: i] getName] isEqualToString: name]) return [[guns objectAtIndex: i] getNum];

	return ret;
}

- (NSString *) weapon_num_to_name: (int) num
{
	NSString *ret = [NSString new];

	switch(num)
	{
		case EAGLE_NUM: return ret = [[guns objectAtIndex: EAGLE] getName];
		case MP5_NUM: return ret = [[guns objectAtIndex: MP5] getName];
		case AK74_NUM: return ret = [[guns objectAtIndex: AK74] getName];
		case STEYRAUG_NUM: return ret = [[guns objectAtIndex: STEYRAUG] getName];
		case SPAS12_NUM: return ret = [[guns objectAtIndex: SPAS12] getName];
		case RUGER77_NUM: return ret = [[guns objectAtIndex: RUGER77] getName];
		case M79_NUM: return ret = [[guns objectAtIndex: M79] getName];
		case BARRETT_NUM: return ret = [[guns objectAtIndex: BARRETT] getName];
		case M249_NUM: return ret = [[guns objectAtIndex: M249] getName];
		case MINIGUN_NUM: return ret = [[guns objectAtIndex: MINIGUN] getName];
		case COLT_NUM: return ret = [[guns objectAtIndex: COLT] getName];
		case KNIFE_NUM: return ret = [[guns objectAtIndex: KNIFE] getName];
		case CHAINSAW_NUM: return ret = [[guns objectAtIndex: CHAINSAW] getName];
		case LAW_NUM: return ret = [[guns objectAtIndex: LAW] getName];
		case BOW2_NUM: return ret = [[guns objectAtIndex: BOW2] getName];
		case BOW_NUM: return ret = [[guns objectAtIndex: BOW] getName];
		case FLAMER_NUM: return ret = [[guns objectAtIndex: FLAMER] getName];
		case M2_NUM: return ret = [[guns objectAtIndex: M2] getName];
		case NOWEAPON_NUM: return ret = [[guns objectAtIndex: NOWEAPON] getName];
		case FRAGGRENADE_NUM: return ret = [[guns objectAtIndex: FRAGGRENADE] getName];
		case CLUSTERGRENADE_NUM: return ret = [[guns objectAtIndex: CLUSTERGRENADE] getName];
		case CLUSTER_NUM: return ret = [[guns objectAtIndex: CLUSTER] getName];
		case THROWNKNIFE_NUM: return ret = [[guns objectAtIndex: THROWNKNIFE] getName];
	}

	return ret;
}

- (int) weapon_num_internal_to_external: (int) num
{
	switch(num)
	{
		case KNIFE_NUM: return 14;
		case CHAINSAW_NUM: return 15;
		case LAW_NUM: return 16;
		case FLAMER_NUM: return 11;
		case BOW_NUM: return 12;
		case BOW2_NUM: return 13;
	}

	return num;
}

- (int) weapon_num_external_to_internal: (int) num
{
	switch(num)
	{
		case 14: return KNIFE_NUM;
		case 15: return CHAINSAW_NUM;
		case 16: return LAW_NUM;
		case 11: return FLAMER_NUM;
		case 12: return BOW_NUM;
		case 13: return BOW2_NUM;
	}

	return num;
}

- (NSString *) weapon_name_by_num: (int) num
{
	int weaponindex;
	NSString *ret = [NSString new];

	for(weaponindex = 0; weaponindex < TOTAL_WEAPONS; weaponindex++)
		if(num == [[guns objectAtIndex: weaponindex] getNum]) return ret = [[guns objectAtIndex: weaponindex] getName];

	return ret;
}

- (int) is_main_weapon_index: (int) weaponindex
{
	return (weaponindex >= 1 && weaponindex <= MAIN_WEAPONS);
}

- (int) is_secondary_weapon_index: (int) weaponindex
{
	return (weaponindex >= PRIMARY_WEAPONS + 1 && weaponindex <= MAIN_WEAPONS);
}

- (int) is_extended_weapon_index: (int) weaponindex
{
	return (weaponindex >= 1 && weaponindex <= EXTENDED_WEAPONS);
}

- (int) calculate_bink: (int) accumulated bink: (short) bink
{
	return accumulated + bink - round(accumulated * (accumulated / ((10 * bink) + accumulated)));
}
@end
