#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Weapons.h"

void create_weapons(int realistic)
{
	create_weapons_base();
	create_default_weapons(realistic);
}

void create_default_weapons(int realistic)
{
	int weaponindex;
	TGun *gun, *defaultgun;

	if(realistic) create_realistic_weapons();
	else create_normal_weapons();

	for(weaponindex = 0; weaponindex < TOTAL_WEAPONS; weaponindex++)
	{
		gun = &guns[weaponindex];
		defaultgun = &defaultguns[weaponindex];

		defaultgun->hitmultiply = gun->hitmultiply;
		defaultgun->fireinterval = gun->fireinterval;
		defaultgun->ammo = gun->ammo;
		defaultgun->reloadtime = gun->reloadtime;
		defaultgun->speed = gun->speed;
		defaultgun->bulletstyle = gun->bulletstyle;
		defaultgun->startuptime = gun->startuptime;
		defaultgun->bink = gun->bink;
		defaultgun->movementacc = gun->movementacc;
		defaultgun->bulletspread = gun->bulletspread;
		defaultgun->recoil = gun->recoil;
		defaultgun->push = gun->push;
		defaultgun->inheritedvelocity = gun->inheritedvelocity;
		defaultgun->modifierlegs = gun->modifierlegs;
		defaultgun->modifierchest = gun->modifierchest;
		defaultgun->modifierhead = gun->modifierhead;
	}

	build_weapons();
}

void create_weapons_base(void)
{
	TGun *gun;

	gun = &guns[EAGLE];
	gun->ininame = gun->name = (char *) malloc(sizeof "Desert Eagles");
	sprintf(gun->name, "Desert Eagles");
	gun->num = EAGLE_NUM;
	gun->texturenum = GFX_WEAPONS_DEAGLES;
	gun->cliptexturenum = GFX_WEAPONS_DEAGLES_CLIP;
	gun->clipreload = 1;
	gun->bulletimagestyle = GFX_WEAPONS_DEAGLES_BULLET;
	gun->firestyle = GFX_WEAPONS_DEAGLES_FIRE;
	gun->firemode = 2;

	gun = &guns[MP5];
	gun->ininame = gun->name = (char *) malloc(sizeof "HK MP5");
	sprintf(gun->name, "HK MP5");
	gun->num = MP5_NUM;
	gun->texturenum = GFX_WEAPONS_MP5;
	gun->cliptexturenum = GFX_WEAPONS_MP5_CLIP;
	gun->clipreload = 1;
	gun->bulletimagestyle = GFX_WEAPONS_MP5_BULLET;
	gun->firestyle = GFX_WEAPONS_MP5_FIRE;
	gun->firemode = 0;

	gun = &guns[AK74];
	gun->ininame = gun->name = (char *) malloc(sizeof "Ak-74");
	sprintf(gun->name, "Ak-74");
	gun->num = AK74_NUM;
	gun->texturenum = GFX_WEAPONS_AK74;
	gun->cliptexturenum = GFX_WEAPONS_AK74_CLIP;
	gun->clipreload = 1;
	gun->bulletimagestyle = GFX_WEAPONS_AK74_BULLET;
	gun->firestyle = GFX_WEAPONS_AK74_FIRE;
	gun->firemode = 0;

	gun = &guns[STEYRAUG];
	gun->ininame = gun->name = (char *) malloc(sizeof "Steyr AUG");
	sprintf(gun->name, "Steyer AUG");
	gun->num = STEYRAUG_NUM;
	gun->texturenum = GFX_WEAPONS_STEYR;
	gun->cliptexturenum = GFX_WEAPONS_STEYR_CLIP;
	gun->clipreload = 1;
	gun->bulletimagestyle = GFX_WEAPONS_STEYR_BULLET;
	gun->firestyle = GFX_WEAPONS_STEYR_FIRE;
	gun->firemode = 0;

	gun = &guns[SPAS12];
	gun->ininame = gun->name = (char *) malloc(sizeof "Spas-12");
	sprintf(gun->name, "Spas-12");
	gun->num = SPAS12_NUM;
	gun->texturenum = GFX_WEAPONS_SPAS;
	gun->cliptexturenum = 0;
	gun->clipreload = 0;
	gun->bulletimagestyle = 0;
	gun->firestyle = GFX_WEAPONS_SPAS_FIRE;
	gun->firemode = 2;

	gun = &guns[RUGER77];
	gun->ininame = gun->name = (char *) malloc(sizeof "Ruger 77");
	sprintf(gun->name, "Ruger 77");
	gun->num = RUGER77_NUM;
	gun->texturenum = GFX_WEAPONS_RUGER;
	gun->cliptexturenum = 0;
	gun->clipreload = 0;
	gun->bulletimagestyle = GFX_WEAPONS_RUGER_BULLET;
	gun->firestyle = GFX_WEAPONS_RUGER_FIRE;
	gun->firemode = 2;

	gun = &guns[M79];
	gun->ininame = gun->name = (char *) malloc(sizeof "M79");
	sprintf(gun->name, "M79");
	gun->num = M79_NUM;
	gun->texturenum = GFX_WEAPONS_M79;
	gun->cliptexturenum = GFX_WEAPONS_M79_CLIP;
	gun->clipreload = 1;
	gun->bulletimagestyle = 0;
	gun->firestyle = GFX_WEAPONS_M79_FIRE;
	gun->firemode = 0;

	gun = &guns[BARRETT];
	gun->ininame = gun->name = (char *) malloc(sizeof "Barrett M82A1");
	sprintf(gun->name, "Barrett M82A1");
	gun->num = BARRETT_NUM;
	gun->texturenum = GFX_WEAPONS_BARRETT;
	gun->cliptexturenum = GFX_WEAPONS_BARRETT_CLIP;
	gun->clipreload = 1;
	gun->bulletimagestyle = GFX_WEAPONS_BARRETT_BULLET;
	gun->firestyle = GFX_WEAPONS_BARRETT_FIRE;
	gun->firemode = 2;

	gun = &guns[M249];
	gun->ininame = gun->name = (char *) malloc(sizeof "FN Minimi");
	sprintf(gun->name, "FN Minimi");
	gun->num = M249_NUM;
	gun->texturenum = GFX_WEAPONS_MINIMI;
	gun->cliptexturenum = GFX_WEAPONS_MINIMI_CLIP;
	gun->clipreload = 1;
	gun->bulletimagestyle = GFX_WEAPONS_MINIMI_BULLET;
	gun->firestyle = GFX_WEAPONS_MINIMI_FIRE;
	gun->firemode = 0;

	gun = &guns[MINIGUN];
	gun->ininame = gun->name = (char *) malloc(sizeof "XM214 Minigun");
	sprintf(gun->name, "XM214 Minigun");
	gun->num = MINIGUN_NUM;
	gun->texturenum = GFX_WEAPONS_MINIGUN;
	gun->cliptexturenum = GFX_WEAPONS_MINIGUN_CLIP;
	gun->clipreload = 0;
	gun->bulletimagestyle = GFX_WEAPONS_MINIGUN_BULLET;
	gun->firestyle = GFX_WEAPONS_MINIGUN_FIRE;
	gun->firemode = 0;

	gun = &guns[COLT];
	gun->ininame = gun->name = (char *) malloc(sizeof "USSOCOM");
	sprintf(gun->name, "USSOCOM");
	gun->num = COLT_NUM;
	gun->texturenum = GFX_WEAPONS_SOCOM;
	gun->cliptexturenum = GFX_WEAPONS_SOCOM_CLIP;
	gun->clipreload = 1;
	gun->bulletimagestyle = GFX_WEAPONS_SOCOM_BULLET;
	gun->firestyle = GFX_WEAPONS_SOCOM_FIRE;
	gun->firemode = 2;

	gun = &guns[KNIFE];
	gun->ininame = gun->name = (char *) malloc(sizeof "Combat Knife");
	sprintf(gun->name, "Combat Knife");
	gun->num = KNIFE_NUM;
	gun->texturenum = GFX_WEAPONS_KNIFE;
	gun->cliptexturenum = 0;
	gun->clipreload = 0;
	gun->bulletimagestyle = 0;
	gun->firestyle = 0;
	gun->firemode = 0;

	gun = &guns[CHAINSAW];
	gun->ininame = gun->name = (char *) malloc(sizeof "Chainsaw");
	sprintf(gun->name, "Chainsaw");
	gun->num = CHAINSAW_NUM;
	gun->texturenum = GFX_WEAPONS_CHAINSAW;
	gun->cliptexturenum = 0;
	gun->clipreload = 0;
	gun->bulletimagestyle = 0;
	gun->firestyle = GFX_WEAPONS_DEAGLES_FIRE;
	gun->firemode = 0;

	gun = &guns[LAW];
	gun->ininame = gun->name = (char *) malloc(sizeof "M72 LAW");
	sprintf(gun->name, "M72 LAW");
	gun->num = LAW_NUM;
	gun->texturenum = GFX_WEAPONS_LAW;
	gun->cliptexturenum = 0;
	gun->clipreload = 1;
	gun->bulletimagestyle = 0;
	gun->firestyle = GFX_WEAPONS_LAW_FIRE;
	gun->firemode = 0;

	gun = &guns[BOW2];
	gun->ininame = gun->name = (char *) malloc(sizeof "Flame Bow");
	sprintf(gun->name, "Flame Bow");
	gun->num = BOW2_NUM;
	gun->texturenum = GFX_WEAPONS_BOW;
	gun->cliptexturenum = GFX_WEAPONS_BOW_S;
	gun->clipreload = 0;
	gun->bulletimagestyle = 0;
	gun->firestyle = GFX_WEAPONS_BOW_FIRE;
	gun->firemode = 0;

	gun = &guns[BOW];
	gun->ininame = gun->name = (char *) malloc(sizeof "Bow");
	sprintf(gun->name, "Bow");
	gun->num = BOW_NUM;
	gun->texturenum = GFX_WEAPONS_BOW;
	gun->cliptexturenum = GFX_WEAPONS_BOW_S;
	gun->clipreload = 0;
	gun->bulletimagestyle = 0;
	gun->firestyle = GFX_WEAPONS_BOW_FIRE;
	gun->firemode = 0;

	gun = &guns[FLAMER];
	gun->ininame = gun->name = (char *) malloc(sizeof "Flamer");
	sprintf(gun->name, "Flamer");
	gun->num = FLAMER_NUM;
	gun->texturenum = GFX_WEAPONS_FLAMER;
	gun->cliptexturenum = GFX_WEAPONS_FLAMER;
	gun->clipreload = 0;
	gun->bulletimagestyle = 0;
	gun->firestyle = GFX_WEAPONS_FLAMER_FIRE;
	gun->firemode = 0;

	gun = &guns[M2];
	gun->ininame = gun->name = (char *) malloc(sizeof "M2");
	sprintf(gun->name, "M2");
	gun->num = M2_NUM;
	gun->texturenum = GFX_WEAPONS_MINIGUN;
	gun->cliptexturenum = 0;
	gun->clipreload = 0;
	gun->bulletimagestyle = 0;
	gun->firestyle = 0;
	gun->firemode = 0;

	gun = &guns[NOWEAPON];
	gun->ininame = gun->name = (char *) malloc(sizeof "Punch");
	sprintf(gun->name, "Punch");
	gun->num = NOWEAPON_NUM;
	gun->texturenum = 0;
	gun->cliptexturenum = 0;
	gun->clipreload = 0;
	gun->bulletimagestyle = 0;
	gun->firestyle = 0;
	gun->firemode = 0;

	gun = &guns[FRAGGRENADE];
	gun->ininame = gun->name = (char *) malloc(sizeof "Frag Grenade");
	sprintf(gun->name, "Frag Grenade");
	gun->num = FRAGGRENADE_NUM;
	gun->texturenum = GFX_WEAPONS_FRAG_GRENADE;
	gun->cliptexturenum = GFX_WEAPONS_FRAG_GRENADE;
	gun->clipreload = 0;
	gun->bulletimagestyle = 0;
	gun->firestyle = GFX_WEAPONS_AK74_FIRE;
	gun->firemode = 0;

	gun = &guns[CLUSTERGRENADE];
	gun->ininame = gun->name = (char *) malloc(sizeof "Frag Grenade");
	sprintf(gun->name, "Frag Grenade");
	gun->num = CLUSTERGRENADE_NUM;
	gun->texturenum = GFX_WEAPONS_FRAG_GRENADE;
	gun->cliptexturenum = GFX_WEAPONS_FRAG_GRENADE;
	gun->clipreload = 0;
	gun->bulletimagestyle = 0;
	gun->firestyle = GFX_WEAPONS_AK74_FIRE;
	gun->firemode = 0;

	gun = &guns[CLUSTER];
	gun->ininame = gun->name = (char *) malloc(sizeof "Frag Grenade");
	sprintf(gun->name, "Frag Grenade");
	gun->num = CLUSTER_NUM;
	gun->texturenum = GFX_WEAPONS_FRAG_GRENADE;
	gun->cliptexturenum = GFX_WEAPONS_FRAG_GRENADE;
	gun->clipreload = 0;
	gun->bulletimagestyle = 0;
	gun->firestyle = GFX_WEAPONS_AK74_FIRE;
	gun->firemode = 0;

	gun = &guns[THROWNKNIFE];
	gun->ininame = gun->name = (char *) malloc(sizeof "Combat Knife");
	sprintf(gun->name, "Combat Knife");
	gun->num = THROWNKNIFE_NUM;
	gun->texturenum = GFX_WEAPONS_KNIFE;
	gun->cliptexturenum = 0;
	gun->clipreload = 0;
	gun->bulletimagestyle = 0;
	gun->firestyle = 0;
	gun->firemode = 0;
}

void create_normal_weapons(void)
{
	TGun *gun;

	gun = &guns[EAGLE];
	gun->hitmultiply = 1.81;
	gun->fireinterval = 24;
	gun->ammo = 7;
	gun->reloadtime = 87;
	gun->speed = 19;
	gun->bulletstyle = BULLET_STYLE_PLAIN;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0.009;
	gun->bulletspread = 0.15;
	gun->recoil = 0;
	gun->push = 0.0176;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 0.95;
	gun->modifierlegs = 0.85;

	gun = &guns[MP5];
	gun->hitmultiply = 1.01;
	gun->fireinterval = 6;
	gun->ammo = 30;
	gun->reloadtime = 105;
	gun->speed = 18.9;
	gun->bulletstyle = BULLET_STYLE_PLAIN;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0;
	gun->bulletspread = 0.14;
	gun->recoil = 0;
	gun->push = 0.0112;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 0.95;
	gun->modifierlegs = 0.85;

	gun = &guns[AK74];
	gun->hitmultiply = 1.004;
	gun->fireinterval = 10;
	gun->ammo = 35;
	gun->reloadtime = 165;
	gun->speed = 24.6;
	gun->bulletstyle = BULLET_STYLE_PLAIN;
	gun->startuptime = 0;
	gun->bink = -12;
	gun->movementacc = 0.011;
	gun->bulletspread = 0.025;
	gun->recoil = 0;
	gun->push = 0.01376;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 0.95;
	gun->modifierlegs = 0.85;

	gun = &guns[STEYRAUG];
	gun->hitmultiply = 0.71;
	gun->fireinterval = 7;
	gun->ammo = 25;
	gun->reloadtime = 125;
	gun->speed = 26;
	gun->bulletstyle = BULLET_STYLE_PLAIN;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0;
	gun->bulletspread = 0.075;
	gun->recoil = 0;
	gun->push = 0.0084;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 0.95;
	gun->modifierlegs = 0.85;

	gun = &guns[SPAS12];
	gun->hitmultiply = 1.22;
	gun->fireinterval = 32;
	gun->ammo = 7;
	gun->reloadtime = 175;
	gun->speed = 14;
	gun->bulletstyle = BULLET_STYLE_SHOTGUN;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0;
	gun->bulletspread = 0.8;
	gun->recoil = 0;
	gun->push = 0.0188;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 0.95;
	gun->modifierlegs = 0.85;

	gun = &guns[RUGER77];
	gun->hitmultiply = 2.49;
	gun->fireinterval = 45;
	gun->ammo = 4;
	gun->reloadtime = 78;
	gun->speed = 33;
	gun->bulletstyle = BULLET_STYLE_PLAIN;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0.03;
	gun->bulletspread = 0;
	gun->recoil = 0;
	gun->push = 0.012;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.2;
	gun->modifierchest = 1.05;
	gun->modifierlegs = 1;

	gun = &guns[M79];
	gun->hitmultiply = 1550;
	gun->fireinterval = 6;
	gun->ammo = 1;
	gun->reloadtime = 178;
	gun->speed = 10.7;
	gun->bulletstyle = BULLET_STYLE_M79;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0;
	gun->bulletspread = 0;
	gun->recoil = 0;
	gun->push = 0.036;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.15;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.9;

	gun = &guns[BARRETT];
	gun->hitmultiply = 4.45;
	gun->fireinterval = 225;
	gun->ammo = 10;
	gun->reloadtime = 70;
	gun->speed = 55;
	gun->bulletstyle = BULLET_STYLE_PLAIN;
	gun->startuptime = 19;
	gun->bink = 65;
	gun->movementacc = 0.05;
	gun->bulletspread = 0;
	gun->recoil = 0;
	gun->push = 0.018;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1;
	gun->modifierchest = 1;
	gun->modifierlegs = 1;

	gun = &guns[M249];
	gun->hitmultiply = 0.85;
	gun->fireinterval = 9;
	gun->ammo = 50;
	gun->reloadtime = 250;
	gun->speed = 27;
	gun->bulletstyle = BULLET_STYLE_PLAIN;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0.013;
	gun->bulletspread = 0.064;
	gun->recoil = 0;
	gun->push = 0.0128;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 0.95;
	gun->modifierlegs = 0.85;

	gun = &guns[MINIGUN];
	gun->hitmultiply = 0.468;
	gun->fireinterval = 3;
	gun->ammo = 100;
	gun->reloadtime = 480;
	gun->speed = 29;
	gun->bulletstyle = BULLET_STYLE_PLAIN;
	gun->startuptime = 25;
	gun->bink = 0;
	gun->movementacc = 0.0625;
	gun->bulletspread = 0.3;
	gun->recoil = 0;
	gun->push = 0.0104;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 0.95;
	gun->modifierlegs = 0.85;

	gun = &guns[COLT];
	gun->hitmultiply = 1.49;
	gun->fireinterval = 10;
	gun->ammo = 14;
	gun->reloadtime = 60;
	gun->speed = 18;
	gun->bulletstyle = BULLET_STYLE_PLAIN;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0;
	gun->bulletspread = 0;
	gun->recoil = 0;
	gun->push = 0.2;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 0.95;
	gun->modifierlegs = 0.85;

	gun = &guns[KNIFE];
	gun->hitmultiply = 2150;
	gun->fireinterval = 6;
	gun->ammo = 1;
	gun->reloadtime = 3;
	gun->speed = 6;
	gun->bulletstyle = BULLET_STYLE_KNIFE;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0;
	gun->bulletspread = 0;
	gun->recoil = 0;
	gun->push = 0.12;
	gun->inheritedvelocity = 0;
	gun->modifierhead = 1.15;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.9;

	gun = &guns[CHAINSAW];
	gun->hitmultiply = 50;
	gun->fireinterval = 2;
	gun->ammo = 200;
	gun->reloadtime = 110;
	gun->speed = 8;
	gun->bulletstyle = BULLET_STYLE_KNIFE;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0;
	gun->bulletspread = 0;
	gun->recoil = 0;
	gun->push = 0.0028;
	gun->inheritedvelocity = 0;
	gun->modifierhead = 1.15;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.9;

	gun = &guns[LAW];
	gun->hitmultiply = 1550;
	gun->fireinterval = 6;
	gun->ammo = 1;
	gun->reloadtime = 300;
	gun->speed = 23;
	gun->bulletstyle = BULLET_STYLE_LAW;
	gun->startuptime = 13;
	gun->bink = 0;
	gun->movementacc = 0;
	gun->bulletspread = 0;
	gun->recoil = 0;
	gun->push = 0.028;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.15;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.9;

	gun = &guns[BOW2];
	gun->hitmultiply = 8;
	gun->fireinterval = 10;
	gun->ammo = 1;
	gun->reloadtime = 39;
	gun->speed = 18;
	gun->bulletstyle = BULLET_STYLE_FLAMEARROW;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0;
	gun->bulletspread = 0;
	gun->recoil = 0;
	gun->push = 0;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.15;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.9;

	gun = &guns[BOW];
	gun->hitmultiply = 12;
	gun->fireinterval = 10;
	gun->ammo = 1;
	gun->reloadtime = 25;
	gun->speed = 21;
	gun->bulletstyle = BULLET_STYLE_ARROW;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0;
	gun->bulletspread = 0;
	gun->recoil = 0;
	gun->push = 0.0148;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.15;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.9;

	gun = &guns[FLAMER];
	gun->hitmultiply = 19;
	gun->fireinterval = 6;
	gun->ammo = 200;
	gun->reloadtime = 5;
	gun->speed = 10.5;
	gun->bulletstyle = BULLET_STYLE_FLAME;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0;
	gun->bulletspread = 0;
	gun->recoil = 0;
	gun->push = 0.016;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.15;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.9;

	gun = &guns[M2];
	gun->hitmultiply = 1.8;
	gun->fireinterval = 10;
	gun->ammo = 100;
	gun->reloadtime = 366;
	gun->speed = 36;
	gun->bulletstyle = BULLET_STYLE_M2;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0;
	gun->bulletspread = 0;
	gun->recoil = 0;
	gun->push = 0.0088;
	gun->inheritedvelocity = 0;
	gun->modifierhead = 1.1;
	gun->modifierchest = 0.95;
	gun->modifierlegs = 0.85;

	gun = &guns[NOWEAPON];
	gun->hitmultiply = 330;
	gun->fireinterval = 6;
	gun->ammo = 1;
	gun->reloadtime = 3;
	gun->speed = 5;
	gun->bulletstyle = BULLET_STYLE_PUNCH;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0;
	gun->bulletspread = 0;
	gun->recoil = 0;
	gun->push = 0;
	gun->inheritedvelocity = 0;
	gun->modifierhead = 1.15;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.9;

	gun = &guns[FRAGGRENADE];
	gun->hitmultiply = 1500;
	gun->fireinterval = 80;
	gun->ammo = 1;
	gun->reloadtime = 20;
	gun->speed = 5;
	gun->bulletstyle = BULLET_STYLE_FRAGNADE;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0;
	gun->bulletspread = 0;
	gun->recoil = 0;
	gun->push = 0;
	gun->inheritedvelocity = 1;
	gun->modifierhead = 1;
	gun->modifierchest = 1;
	gun->modifierlegs = 1;
}

void create_realistic_weapons(void)
{
	TGun *gun;

	gun = &guns[EAGLE];
	gun->hitmultiply = 1.66;
	gun->fireinterval = 27;
	gun->ammo = 7;
	gun->reloadtime = 106;
	gun->speed = 19;
	gun->bulletstyle = BULLET_STYLE_PLAIN;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0.02;
	gun->bulletspread = 0.1;
	gun->recoil = 55;
	gun->push = 0.0164;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.6;

	gun = &guns[MP5];
	gun->hitmultiply = 0.94;
	gun->fireinterval = 6;
	gun->ammo = 30;
	gun->reloadtime = 110;
	gun->speed = 18.9;
	gun->bulletstyle = BULLET_STYLE_PLAIN;
	gun->startuptime = 0;
	gun->bink = -10;
	gun->movementacc = 0.01;
	gun->bulletspread = 0.03;
	gun->recoil = 9;
	gun->push = 0.0164;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.6;

	gun = &guns[AK74];
	gun->hitmultiply = 1.08;
	gun->fireinterval = 11;
	gun->ammo = 35;
	gun->reloadtime = 158;
	gun->speed = 24;
	gun->bulletstyle = BULLET_STYLE_PLAIN;
	gun->startuptime = 0;
	gun->bink = -10;
	gun->movementacc = 0.02;
	gun->bulletspread = 0;
	gun->recoil = 13;
	gun->push = 0.0132;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.6;

	gun = &guns[STEYRAUG];
	gun->hitmultiply = 0.68;
	gun->fireinterval = 7;
	gun->ammo = 30;
	gun->reloadtime = 126;
	gun->speed = 26;
	gun->bulletstyle = BULLET_STYLE_PLAIN;
	gun->startuptime = 0;
	gun->bink = -9;
	gun->movementacc = 0.01;
	gun->bulletspread = 0;
	gun->recoil = 11;
	gun->push = 0.012;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.6;

	gun = &guns[SPAS12];
	gun->hitmultiply = 1.2;
	gun->fireinterval = 35;
	gun->ammo = 7;
	gun->reloadtime = 175;
	gun->speed = 13.2;
	gun->bulletstyle = BULLET_STYLE_SHOTGUN;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0.01;
	gun->bulletspread = 0.8;
	gun->recoil = 65;
	gun->push = 0.0224;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.6;

	gun = &guns[RUGER77];
	gun->hitmultiply = 2.22;
	gun->fireinterval = 52;
	gun->ammo = 4;
	gun->reloadtime = 104;
	gun->speed = 33;
	gun->bulletstyle = BULLET_STYLE_PLAIN;
	gun->startuptime = 0;
	gun->bink = 14;
	gun->movementacc = 0.03;
	gun->bulletspread = 0;
	gun->recoil = 54;
	gun->push = 0.0096;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.6;

	gun = &guns[M79];
	gun->hitmultiply = 1600;
	gun->fireinterval = 6;
	gun->ammo = 1;
	gun->reloadtime = 173;
	gun->speed = 11.4;
	gun->bulletstyle = BULLET_STYLE_M79;
	gun->startuptime = 0;
	gun->bink = 45;
	gun->movementacc = 0.03;
	gun->bulletspread = 0;
	gun->recoil = 420;
	gun->push = 0.024;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.6;

	gun = &guns[BARRETT];
	gun->hitmultiply = 4.95;
	gun->fireinterval = 200;
	gun->ammo = 10;
	gun->reloadtime = 170;
	gun->speed = 55;
	gun->bulletstyle = BULLET_STYLE_PLAIN;
	gun->startuptime = 16;
	gun->bink = 80;
	gun->movementacc = 0.07;
	gun->bulletspread = 0;
	gun->recoil = 0;
	gun->push = 0.0056;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.6;

	gun = &guns[M249];
	gun->hitmultiply = 0.81;
	gun->fireinterval = 10;
	gun->ammo = 50;
	gun->reloadtime = 261;
	gun->speed = 27;
	gun->bulletstyle = BULLET_STYLE_PLAIN;
	gun->startuptime = 0;
	gun->bink = -8;
	gun->movementacc = 0.02;
	gun->bulletspread = 0;
	gun->recoil = 8;
	gun->push = 0.0116;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.6;

	gun = &guns[MINIGUN];
	gun->hitmultiply = 0.43;
	gun->fireinterval = 4;
	gun->ammo = 100;
	gun->reloadtime = 320;
	gun->speed = 29;
	gun->bulletstyle = BULLET_STYLE_PLAIN;
	gun->startuptime = 33;
	gun->bink = -2;
	gun->movementacc = 0.01;
	gun->bulletspread = 0.1;
	gun->recoil = 4;
	gun->push = 0.0108;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.6;

	gun = &guns[COLT];
	gun->hitmultiply = 1.30;
	gun->fireinterval = 12;
	gun->ammo = 12;
	gun->reloadtime = 72;
	gun->speed = 18;
	gun->bulletstyle = BULLET_STYLE_PLAIN;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0.02;
	gun->bulletspread = 0;
	gun->recoil = 28;
	gun->push = 0.0172;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.6;

	gun = &guns[KNIFE];
	gun->hitmultiply = 2250;
	gun->fireinterval = 6;
	gun->ammo = 1;
	gun->reloadtime = 3;
	gun->speed = 6;
	gun->bulletstyle = BULLET_STYLE_KNIFE;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0.01;
	gun->bulletspread = 0;
	gun->recoil = 10;
	gun->push = 0.028;
	gun->inheritedvelocity = 0;
	gun->modifierhead = 1.1;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.6;

	gun = &guns[CHAINSAW];
	gun->hitmultiply = 21;
	gun->fireinterval = 2;
	gun->ammo = 200;
	gun->reloadtime = 110;
	gun->speed = 7.6;
	gun->bulletstyle = BULLET_STYLE_KNIFE;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0.01;
	gun->bulletspread = 0;
	gun->recoil = 1;
	gun->push = 0.0028;
	gun->inheritedvelocity = 0;
	gun->modifierhead = 1.1;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.6;

	gun = &guns[LAW];
	gun->hitmultiply = 1500;
	gun->fireinterval = 30;
	gun->ammo = 1;
	gun->reloadtime = 495;
	gun->speed = 23;
	gun->bulletstyle = BULLET_STYLE_LAW;
	gun->startuptime = 12;
	gun->bink = 0;
	gun->movementacc = 0.01;
	gun->bulletspread = 0;
	gun->recoil = 9;
	gun->push = 0.012;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.6;

	gun = &guns[BOW2];
	gun->hitmultiply = 8;
	gun->fireinterval = 10;
	gun->ammo = 1;
	gun->reloadtime = 39;
	gun->speed = 18;
	gun->bulletstyle = BULLET_STYLE_FLAMEARROW;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0.01;
	gun->bulletspread = 0;
	gun->recoil = 10;
	gun->push = 0;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.6;

	gun = &guns[BOW];
	gun->hitmultiply = 12;
	gun->fireinterval = 10;
	gun->ammo = 1;
	gun->reloadtime = 25;
	gun->speed = 21;
	gun->bulletstyle = BULLET_STYLE_ARROW;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0.01;
	gun->bulletspread = 0;
	gun->recoil = 10;
	gun->push = 0.0148;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.6;

	gun = &guns[FLAMER];
	gun->hitmultiply = 12;
	gun->fireinterval = 6;
	gun->ammo = 200;
	gun->reloadtime = 5;
	gun->speed = 12.5;
	gun->bulletstyle = BULLET_STYLE_FLAME;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0.01;
	gun->bulletspread = 0;
	gun->recoil = 10;
	gun->push = 0.016;
	gun->inheritedvelocity = 0.5;
	gun->modifierhead = 1.1;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.6;

	gun = &guns[M2];
	gun->hitmultiply = 1.55;
	gun->fireinterval = 14;
	gun->ammo = 100;
	gun->reloadtime = 366;
	gun->speed = 36;
	gun->bulletstyle = BULLET_STYLE_M2;
	gun->startuptime = 21;
	gun->bink = 0;
	gun->movementacc = 0.01;
	gun->bulletspread = 0;
	gun->recoil = 10;
	gun->push = 0.0088;
	gun->inheritedvelocity = 0;
	gun->modifierhead = 1.1;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.6;

	gun = &guns[NOWEAPON];
	gun->hitmultiply = 330;
	gun->fireinterval = 6;
	gun->ammo = 1;
	gun->reloadtime = 3;
	gun->speed = 5;
	gun->bulletstyle = BULLET_STYLE_PUNCH;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0.01;
	gun->bulletspread = 0;
	gun->recoil = 10;
	gun->push = 0;
	gun->inheritedvelocity = 0;
	gun->modifierhead = 1.1;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.6;

	gun = &guns[FRAGGRENADE];
	gun->hitmultiply = 1500;
	gun->fireinterval = 80;
	gun->ammo = 1;
	gun->reloadtime = 20;
	gun->speed = 5;
	gun->bulletstyle = BULLET_STYLE_FRAGNADE;
	gun->startuptime = 0;
	gun->bink = 0;
	gun->movementacc = 0.01;
	gun->bulletspread = 0;
	gun->recoil = 10;
	gun->push = 0;
	gun->inheritedvelocity = 1;
	gun->modifierhead = 1.1;
	gun->modifierchest = 1;
	gun->modifierlegs = 0.6;
}

void build_weapons(void)
{
	int weaponindex;
	TGun *gun;

	gun = &guns[CLUSTERGRENADE];
	gun->hitmultiply = guns[FRAGGRENADE].hitmultiply;
	gun->fireinterval = guns[FRAGGRENADE].fireinterval;
	gun->ammo = guns[FRAGGRENADE].ammo;
	gun->reloadtime = guns[FRAGGRENADE].reloadtime;
	gun->speed = guns[FRAGGRENADE].speed;
	gun->bulletstyle = BULLET_STYLE_CLUSTERNADE;
	gun->startuptime = guns[FRAGGRENADE].startuptime;
	gun->bink = guns[FRAGGRENADE].bink;
	gun->movementacc = guns[FRAGGRENADE].movementacc;
	gun->bulletspread = guns[FRAGGRENADE].bulletspread;
	gun->recoil = guns[FRAGGRENADE].recoil;
	gun->push = guns[FRAGGRENADE].push;
	gun->inheritedvelocity = guns[FRAGGRENADE].inheritedvelocity;

	gun = &guns[CLUSTER];
	gun->hitmultiply = guns[CLUSTERGRENADE].hitmultiply;
	gun->fireinterval = guns[CLUSTERGRENADE].fireinterval;
	gun->ammo = guns[CLUSTERGRENADE].ammo;
	gun->reloadtime = guns[CLUSTERGRENADE].reloadtime;
	gun->speed = guns[CLUSTERGRENADE].speed;
	gun->bulletstyle = BULLET_STYLE_CLUSTER;
	gun->startuptime = guns[CLUSTERGRENADE].startuptime;
	gun->bink = guns[CLUSTERGRENADE].bink;
	gun->movementacc = guns[CLUSTERGRENADE].movementacc;
	gun->bulletspread = guns[CLUSTERGRENADE].bulletspread;
	gun->recoil = guns[CLUSTERGRENADE].recoil;
	gun->push = guns[CLUSTERGRENADE].push;
	gun->inheritedvelocity = guns[CLUSTERGRENADE].inheritedvelocity;

	gun = &guns[THROWNKNIFE];
	gun->hitmultiply = guns[KNIFE].hitmultiply;
	gun->fireinterval = guns[KNIFE].fireinterval;
	gun->ammo = guns[KNIFE].ammo;
	gun->reloadtime = guns[KNIFE].reloadtime;
	gun->speed = guns[KNIFE].speed;
	gun->bulletstyle = BULLET_STYLE_THROWNKNIFE;
	gun->startuptime = guns[KNIFE].startuptime;
	gun->bink = guns[KNIFE].bink;
	gun->movementacc = guns[KNIFE].movementacc;
	gun->bulletspread = guns[KNIFE].bulletspread;
	gun->recoil = guns[KNIFE].recoil;
	gun->push = guns[KNIFE].push;
	gun->inheritedvelocity = guns[KNIFE].inheritedvelocity;

	for(weaponindex = 0; weaponindex < TOTAL_WEAPONS; weaponindex++)
	{
		gun = &guns[weaponindex];

		gun->fireintervalprev = gun->fireinterval;
		gun->fireintervalcount = gun->fireinterval;
		gun->ammocount = gun->ammo;
		gun->reloadtimeprev = gun->reloadtime;
		gun->reloadtimecount = gun->reloadtime;
		gun->startuptimecount = gun->startuptime;

		if(gun->clipreload)
		{
			gun->clipouttime = trunc(gun->reloadtime * 0.8);
			gun->clipintime = trunc(gun->reloadtime * 0.3);
		}
		else
		{
			gun->clipouttime = 0;
			gun->clipintime = 0;
		}

		switch(gun->bulletstyle)
		{
			case BULLET_STYLE_FRAGNADE:
			case BULLET_STYLE_CLUSTERNADE:
				gun->timeout = GRENADE_TIMEOUT;
				break;
			case BULLET_STYLE_FLAME:
				gun->timeout = FLAME_TIMEOUT;
				break;
			case BULLET_STYLE_PUNCH:
			case BULLET_STYLE_KNIFE:
				gun->timeout = MELEE_TIMEOUT;
				break;
			case BULLET_STYLE_M2:
				gun->timeout = M2BULLET_TIMEOUT;
				break;
			default: gun->timeout = BULLET_TIMEOUT;
		}
	}

	guns[M79].ammocount = 0;
}

unsigned int create_wm_checksum(void)
{
	int weaponindex;
	unsigned int hash = 5381;
	TGun *gun;

	for(weaponindex = 0; weaponindex < ORIGINAL_WEAPONS; weaponindex++)
	{
		gun = &guns[weaponindex];

		hash = (hash << 5) + (unsigned int) round(1000.0 * gun->hitmultiply);
		hash = (hash << 5) + (unsigned int) round(1000.0 * gun->fireinterval);
		hash = (hash << 5) + (unsigned int) round(1000.0 * gun->ammo);
		hash = (hash << 5) + (unsigned int) round(1000.0 * gun->reloadtime);
		hash = (hash << 5) + (unsigned int) round(1000.0 * gun->speed);
		hash = (hash << 5) + (unsigned int) round(1000.0 * gun->bulletstyle);
		hash = (hash << 5) + (unsigned int) round(1000.0 * gun->startuptime);
		hash = (hash << 5) + (unsigned int) round(1000.0 * gun->bink);
		hash = (hash << 5) + (unsigned int) round(1000.0 * gun->movementacc);
		hash = (hash << 5) + (unsigned int) round(1000.0 * gun->bulletspread);
		hash = (hash << 5) + (unsigned int) round(1000.0 * gun->recoil);
		hash = (hash << 5) + (unsigned int) round(1000.0 * gun->push);
		hash = (hash << 5) + (unsigned int) round(1000.0 * gun->inheritedvelocity);
	}

	return hash;
}

short weapon_num_to_index(int num)
{
	int ret = -1, weaponindex;

	for(weaponindex = 0; weaponindex < TOTAL_WEAPONS; weaponindex++)
		if(num == guns[weaponindex].num) return ret = weaponindex;

	return ret;
}

int weapon_name_to_num(char *name)
{
	int i, j, ret = -1, len = 0;

	for(i = 0; i < TOTAL_WEAPONS; i++)
	{
		for(j = 0; name[j] && guns[i].name[j]; j++)
		{
			if(name[j] == guns[i].name[j]) len++;
			else
			{
				len = 0;
				break;
			}
		}

		if(len && strlen(name) == len && strlen(guns[i].name) == len) return guns[i].num;
	}

	return ret;
}

char *weapon_num_to_name(int num)
{
	char *ret = "";

	switch(num)
	{
		case EAGLE_NUM: return guns[EAGLE].name;
		case MP5_NUM: return guns[MP5].name;
		case AK74_NUM: return guns[AK74].name;
		case STEYRAUG_NUM: return guns[STEYRAUG].name;
		case SPAS12_NUM: return guns[SPAS12].name;
		case RUGER77_NUM: return guns[RUGER77].name;
		case M79_NUM: return guns[M79].name;
		case BARRETT_NUM: return guns[BARRETT].name;
		case M249_NUM: return guns[M249].name;
		case MINIGUN_NUM: return guns[MINIGUN].name;
		case COLT_NUM: return guns[COLT].name;
		case KNIFE_NUM: return guns[KNIFE].name;
		case CHAINSAW_NUM: return guns[CHAINSAW].name;
		case LAW_NUM: return guns[LAW].name;
		case BOW2_NUM: return guns[BOW2].name;
		case BOW_NUM: return guns[BOW].name;
		case FLAMER_NUM: return guns[FLAMER].name;
		case M2_NUM: return guns[M2].name;
		case NOWEAPON_NUM: return guns[NOWEAPON].name;
		case FRAGGRENADE_NUM: return guns[FRAGGRENADE].name;
		case CLUSTERGRENADE_NUM: return guns[CLUSTERGRENADE].name;
		case CLUSTER_NUM: return guns[CLUSTER].name;
		case THROWNKNIFE_NUM: return guns[THROWNKNIFE].name;
		default: return ret;
	}

	return ret;
}

int weapon_num_internal_to_external(int num)
{
	switch(num)
	{
		case KNIFE_NUM: return 14;
		case CHAINSAW_NUM: return 15;
		case LAW_NUM: return 16;
		case FLAMER_NUM: return 11;
		case BOW_NUM: return 12;
		case BOW2_NUM: return 13;
		default: return num;
	}

	return num;
}

int weapon_num_external_to_internal(int num)
{
	switch(num)
	{
		case 14: return KNIFE_NUM;
		case 15: return CHAINSAW_NUM;
		case 16: return LAW_NUM;
		case 11: return FLAMER_NUM;
		case 12: return BOW_NUM;
		case 13: return BOW2_NUM;
		default: return num;
	}

	return num;
}

char *weapon_name_by_num(int num)
{
	int weaponindex;
	char *ret = "";

	for(weaponindex = 0; weaponindex < TOTAL_WEAPONS; weaponindex++)
		if(num == guns[weaponindex].num) return ret = guns[weaponindex].name;

	return ret;
}

int is_main_weapon_index(short weaponindex)
{
	return (weaponindex >= 1 && weaponindex <= MAIN_WEAPONS);
}

int is_secondary_weapon_index(short weaponindex)
{
	return (weaponindex >= PRIMARY_WEAPONS + 1 && weaponindex <= MAIN_WEAPONS);
}

int is_extended_weapon_index(short weaponindex)
{
	return (weaponindex >= 1 && weaponindex <= EXTENDED_WEAPONS);
}

int calculate_bink(int accumulated, int bink)
{
	return accumulated + bink - round(accumulated * (accumulated / ((10 * bink) + accumulated)));
}
