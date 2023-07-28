#define EAGLE 0
#define MP5 1
#define AK74 2
#define STEYRAUG 3
#define SPAS12 4
#define RUGER77 5
#define M79 6
#define BARRETT 7
#define M249 8
#define MINIGUN 9
#define COLT 10
#define KNIFE 11
#define CHAINSAW 12
#define LAW 13
#define BOW2 14
#define BOW 15
#define FLAMER 16
#define M2 17
#define NOWEAPON 18
#define FRAGGRENADE 19
#define CLUSTERGRENADE 20
#define CLUSTER 21
#define THROWNKNIFE 22

#define PRIMARY_WEAPONS 10
#define SECONDARY_WEAPONS 4
#define BONUS_WEAPONS 3
#define MAIN_WEAPONS PRIMARY_WEAPONS + SECONDARY_WEAPONS
#define EXTENDED_WEAPONS MAIN_WEAPONS + BONUS_WEAPONS

#define ORIGINAL_WEAPONS 20
#define TOTAL_WEAPONS 23

#define EAGLE_NUM 1
#define MP5_NUM 2
#define AK74_NUM 3
#define STEYRAUG_NUM 4
#define SPAS12_NUM 5
#define RUGER77_NUM 6
#define M79_NUM 7
#define BARRETT_NUM 8
#define M249_NUM 9
#define MINIGUN_NUM 10
#define COLT_NUM 0
#define KNIFE_NUM 11
#define CHAINSAW_NUM 12
#define LAW_NUM 13
#define BOW2_NUM 16
#define BOW_NUM 15
#define FLAMER_NUM 14
#define M2_NUM 30
#define NOWEAPON_NUM 255
#define FRAGGRENADE_NUM 50
#define CLUSTERGRENADE_NUM 51
#define CLUSTER_NUM 52
#define THROWNKNIFE_NUM 53

#define BULLET_STYLE_PLAIN 1
#define BULLET_STYLE_FRAGNADE 2
#define BULLET_STYLE_SHOTGUN 3
#define BULLET_STYLE_M79 4
#define BULLET_STYLE_FLAME 5
#define BULLET_STYLE_PUNCH 6
#define BULLET_STYLE_ARROW 7
#define BULLET_STYLE_FLAMEARROW 8
#define BULLET_STYLE_CLUSTERNADE 9
#define BULLET_STYLE_CLUSTER 10
#define BULLET_STYLE_KNIFE 11
#define BULLET_STYLE_LAW 12
#define BULLET_STYLE_THROWKNIFE 13
#define BULLET_STYLE_M2 14

#define WEAPON_NOCOLLISION_ENEMY 1
#define WEAPON_NOCOLLISION_TEAM 1 << 1
#define WEAPON_NOCOLLISION_SELF 1 << 2
#define WEAPON_NOCOLLISION_EXP_ENEMY 1 << 3
#define WEAPON_NOCOLLISION_EXP_TEAM 1 << 4
#define WEAPON_NOCOLLISION_EXP_SELF 1 << 5

@interface TGun: NSObject
{
	int ammo, ammocount, num, recoil,
	    fireinterval, fireintervalprev, fireintervalcount,
	    startuptime, startuptimecount, reloadtime,
	    reloadtimeprev, reloadtimecount, texturenum,
	    cliptexturenum, clipreload, clipintime,
	    clipouttime, nocollision, firemode, timeout,
	    bulletstyle, firestyle, bulletimagestyle;
	float speed, movementacc, fireintervalreal, reloadtimereal,
	      hitmultiply, bulletspread, push, inheritedvelocity,
	      modifierlegs, modifierchest, modifierhead;
	short bink;
	NSString *name, *ininame;
}

- (void) setAmmo: (int) o;
- (void) setAmmoCount: (int) o;
- (void) setNum: (int) o;
- (void) setRecoil: (int) o;
- (void) setFireInterval: (int) o;
- (void) setFireIntervalPrev: (int) o;
- (void) setFireIntevalCount: (int) o;
- (void) setStartupTime: (int) o;
- (void) setStartupTimeCount: (int) o;
- (void) setReloadTime: (int) o;
- (void) setReloadTimePrev: (int) o;
- (void) setReloadTimeCount: (int) o;
- (void) setTextureNum: (int) o;
- (void) setClipTextureNum: (int) o;
- (void) setClipReload: (int) o;
- (void) setClipInTime: (int) o;
- (void) setClipOutTime: (int) o;
- (void) setNoCollision: (int) o;
- (void) setFireMode: (int) o;
- (void) setTimeout: (int) o;
- (void) setBulletStyle: (int) o;
- (void) setFireStyle: (int) o;
- (void) setBulletImageStyle: (int) o;
- (void) setSpeed: (float) o;
- (void) setMovementAcc: (float) o;
- (void) setFireIntervalReal: (float) o;
- (void) setReloadTimeReal: (float) o;
- (void) setHitMultiply: (float) o;
- (void) setBulletSpread: (float) o;
- (void) setPush: (float) o;
- (void) setInheritedVelocity: (float) o;
- (void) setModifierLegs: (float) o;
- (void) setModifierChest: (float) o;
- (void) setModifierHead: (float) o;
- (void) setBink: (short) o;
- (void) setName: (NSString *) o;
- (void) setININame: (NSString *) o;
- (int) getAmmo;
- (int) getAmmoCount;
- (int) getNum;
- (int) getRecoil;
- (int) getFireInterval;
- (int) getFireIntervalPrev;
- (int) getFireIntevalCount;
- (int) getStartupTime;
- (int) getStartupTimeCount;
- (int) getReloadTime;
- (int) getReloadTimePrev;
- (int) getReloadTimeCount;
- (int) getTextureNum;
- (int) getClipTextureNum;
- (int) getClipReload;
- (int) getClipInTime;
- (int) getClipOutTime;
- (int) getNoCollision;
- (int) getFireMode;
- (int) getTimeout;
- (int) getBulletStyle;
- (int) getFireStyle;
- (int) getBulletImageStyle;
- (float) getSpeed;
- (float) getMovementAcc;
- (float) getFireIntervalReal;
- (float) getReloadTimeReal;
- (float) getHitMultiply;
- (float) getBulletSpread;
- (float) getPush;
- (float) getInheritedVelocity;
- (float) getModifierLegs;
- (float) getModifierChest;
- (float) getModifierHead;
- (short) getBink;
- (NSString *) getName;
- (NSString *) getININame;
@end

@interface Weapons: NSObject
{
	unsigned int defaultwmchecksum, loadedwmchecksum;
	NSMutableArray *guns, *defaultguns;
}

- (void) create_weapons: (int) realistic;
- (void) create_default_weapons: (int) realistic;
- (void) create_weapons_base;
- (void) create_normal_weapons;
- (void) create_realistic_weapons;
- (void) build_weapons;
- (unsigned int) create_wm_checksum;
- (int) weapon_num_to_index: (int) num;
- (int) weapon_name_to_num: (NSString *) name;
- (NSString *) weapon_num_to_name: (int) num;
- (NSString *) weapon_name_by_num: (int) num;
- (int) is_main_weapon_index: (short) index;
- (int) is_secondary_weapon_index: (short) index;
- (int) is_extended_weapon_index: (short) index;
- (int) calculate_bink: (int) accumulated bink: (short) bink;
- (int) weapon_num_internal_to_external: (int) num;
- (int) weapon_num_external_to_internal: (int) num;
@end
