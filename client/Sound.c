#include <SDL2/SDL.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <physfs.h>
#include "Sound.h"

#define MAX_SOURCES 256
#define RESERVED_SOURCES 128
#define MAX_SAMPLES 163
#define CHANNEL_WEATHER 127

TSoundSample samp[MAX_SAMPLES];
TScriptSound *scriptsamp;
float volumeinternal;
ALCdevice *aldevice;
ALCcontext *alcontext;
ALuint sources[MAX_SOURCES - 1];
long defaultchannel = -1;

int init_sound(void)
{
	int i;

	if((aldevice = alcOpenDevice(NULL)) == NULL) return 0;
	else if((alcontext = alcCreateContext(aldevice, NULL))) return 0;
	else if(!alcMakeContextCurrent(alcontext)) return 0;

	alDistanceModel(AL_NONE);
	alGenSources(MAX_SOURCES, sources);

	if(alGetError() != AL_NO_ERROR) return 0;

	for(i = 0; i < MAX_SAMPLES; i++)
		samp[i].loaded = 0;

	return 1;
}

TSoundSample load_sample(char *name, TSoundSample sampload)
{
	SDL_AudioSpec *spec;
	Uint8 *databuffer;
	Uint32 datalength;
	ALenum format = AL_FORMAT_MONO8;
	int badformat = 0;
	TSoundSample ret;
	SDL_RWops *wavfile;
	PHYSFS_File *filebuffer;

	if(!sampload.loaded) ret.loaded = 0;
	else ret = sampload;

	if((filebuffer = PHYSFS_openRead(name)) == NULL) return ret;

	wavfile = SDL_RWFromMem(filebuffer, strlen(name));

	if(SDL_LoadWAV_RW(wavfile, 1, spec, &databuffer, &datalength) == NULL) return ret;

	switch(spec->channels)
	{
		case 1:
			switch(spec->format)
			{
				case AUDIO_U8:
					format = AL_FORMAT_MONO8;
				case AUDIO_S16SYS:
					format = AL_FORMAT_MONO16;
					break;
				case AUDIO_F32SYS:
					format = AL_FORMAT_MONO_FLOAT32;
					break;
				default: badformat = 1;
			}
			break;
		case 2:
			switch(spec->format)
			{
				case AUDIO_U8:
					format = AL_FORMAT_STEREO8;
				case AUDIO_S16SYS:
					format = AL_FORMAT_STEREO16;
					break;
				case AUDIO_F32SYS:
					format = AL_FORMAT_STEREO_FLOAT32;
					break;
				default: badformat = 1;
			}
			break;
		default: badformat = 1;
	}

	if(!badformat)
	{
		if(sampload.loaded)
		{
			alDeleteBuffers(1, &sampload.buffer);
			alGenBuffers(1, &ret.buffer);
			alBufferData(ret.buffer, format, &databuffer, datalength, spec->freq);
			ret.loaded = 1;
		}

		SDL_FreeWAV(databuffer);
	}

	return ret;
}

short sound_name_to_id(char *name, int size)
{
	int i, j, ret = -1, len = ret;

	if(!size) return ret;

	for(i = 0; i < size; i++)
	{
		for(j = 0; scriptsamp->name[j] && name[j]; j++)
			if(toupper(scriptsamp->name[j]) == toupper(name[j])) len++;

		if(strlen(scriptsamp->name) == len && strlen(name) == len) return ret = i;
		else len = ret;
	}

	return ret;
}

float scale_volume_setting(int volumesetting)
{
	return (pow(1.0404, volumesetting) - 1) / (1.0404 - 1) / 1275;
}

void load_sounds(char *moddir)
{
	int i;
	char *sfxpath, *sfxfile;
	const char *sample_files[] = {
		"ak74-fire.wav",
		"rocketz.wav",
		"ak74-reload.wav",
		"", // empty.wav - no longer used
		"m249-fire.wav",
		"ruger77-fire.wav",
		"ruger77-reload.wav",
		"m249-reload.wav",
		"mp5-fire.wav",
		"mp5-reload.wav",
		"spas12-fire.wav",
		"spas12-reload.wav",
		"standup.wav",
		"fall.wav",
		"spawn.wav",
		"m79-fire.wav",
		"m79-explosion.wav",
		"m79-reload.wav",
		"grenade-throw.wav",
		"grenade-explosion.wav",
		"grenade-bounce.wav",
		"bryzg.wav",
		"infiltmus.wav",
		"headchop.wav",
		"explosion-erg.wav",
		"water-step.wav",
		"bulletby.wav",
		"bodyfall.wav",
		"deserteagle-fire.wav",
		"deserteagle-reload.wav",
		"steyraug-fire.wav",
		"steyraug-reload.wav",
		"barretm82-fire.wav",
		"barretm82-reload.wav",
		"minigun-fire.wav",
		"minigun-reload.wav",
		"minigun-start.wav",
		"minigun-end.wav",
		"pickupgun.wav",
		"capture.wav",
		"colt1911-fire.wav",
		"colt1911-reload.wav",
		"changeweapon.wav",
		"shell.wav",
		"shell2.wav",
		"dead-hit.wav",
		"throwgun.wav",
		"bow-fire.wav",
		"takebow.wav",
		"takemedikit.wav",
		"wermusic.wav",
		"ts.wav",
		"ctf.wav",
		"berserker.wav",
		"godflame.wav",
		"flamer.wav",
		"predator.wav",
		"killberserk.wav",
		"vesthit.wav",
		"burn.wav",
		"vesttake.wav",
		"clustergrenade.wav",
		"cluster-explosion.wav",
		"grenade-pullout.wav",
		"spit.wav",
		"stuff.wav",
		"smoke.wav",
		"match.wav",
		"roar.wav",
		"step.wav",
		"step2.wav",
		"step3.wav",
		"step4.wav",
		"hum.wav",
		"ric.wav",
		"ric2.wav",
		"ric3.wav",
		"ric4.wav",
		"dist-m79.wav",
		"dist-grenade.wav",
		"dist-gun1.wav",
		"dist-gun2.wav",
		"dist-gun3.wav",
		"dist-gun4.wav",
		"death.wav",
		"death2.wav",
		"death3.wav",
		"crouch-move.wav",
		"hit-arg.wav",
		"hit-arg2.wav",
		"hit-arg3.wav",
		"goprone.wav",
		"roll.wav",
		"fall-hard.wav",
		"onfire.wav",
		"firecrack.wav",
		"scope.wav",
		"scopeback.wav",
		"playerdeath.wav",
		"changespin.wav",
		"arg.wav",
		"lava.wav",
		"regenerate.wav",
		"prone-move.wav",
		"jump.wav",
		"crouch.wav",
		"crouch-movel.wav",
		"step5.wav",
		"step6.wav",
		"step7.wav",
		"step8.wav",
		"stop.wav",
		"bulletby2.wav",
		"bulletby3.wav",
		"bulletby4.wav",
		"bulletby5.wav",
		"weaponhit.wav",
		"clipfall.wav",
		"bonecrack.wav",
		"gaugeshell.wav",
		"colliderhit.wav",
		"kit-fall.wav",
		"kit-fall2.wav",
		"flag.wav",
		"flag2.wav",
		"takegun.wav",
		"infilt-point.wav",
		"menuclick.wav",
		"knife.wav",
		"slash.wav",
		"chainsaw-d.wav",
		"chainsaw-m.wav",
		"chainsaw-r.wav",
		"piss.wav",
		"law.wav",
		"chainsaw-o.wav",
		"m2fire.wav",
		"m2explode.wav",
		"m2overheat.wav",
		"signal.wav",
		"m2use.wav",
		"scoperun.wav",
		"mercy.wav",
		"ric5.wav",
		"ric6.wav",
		"ric7.wav",
		"law-start.wav",
		"law-end.wav",
		"boomheadshot.wav",
		"snapshot.wav",
		"radio/efcup.wav",
		"radio/efcmid.wav",
		"radio/efcdown.wav",
		"radio/ffcup.wav",
		"radio/ffcmid.wav",
		"radio/ffcdown.wav",
		"radio/esup.wav",
		"radio/esmid.wav",
		"radio/esdown.wav",
		"bounce.wav",
		"sfx_rain.wav",
		"sfx_snow.wav",
		"sfx_wind.wav"
	};

	if(*moddir)
	{
		sfxpath = (char *) malloc(strlen(moddir) + sizeof "sfx/");
		sprintf(sfxpath, "%s%s", moddir, "sfx/");
	}
	else
	{
		sfxpath = (char *) malloc(sizeof "sfx/");
		sprintf(sfxpath, "sfx/");
	}
	printf("Loading sound effects.\n");

	for(i = 0; i < sizeof sample_files / sizeof(char *); i++)
		if(*sample_files[i])
		{
			sfxfile = (char *) malloc(strlen(sfxpath) + strlen(sample_files[i]) + 1);
			sprintf(sfxfile, "%s%s", sfxpath, sample_files[i]);
			samp[i] = load_sample(sfxfile, samp[i]);

			if(!samp[i].loaded) fprintf(stderr, "Unable to load file %s.\n", sfxfile);
			free(sfxfile);
		}

	free(sfxpath);
}

void close_sound(void)
{
	int i;

	for(i = 0; i < MAX_SOURCES - 1; i++)
		if(samp[i].loaded)
		{
			alDeleteBuffers(1, &samp[i].buffer);
			samp[i].loaded = 0;
		}

	alDeleteSources(MAX_SOURCES - 1, sources);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(alcontext);
	alcCloseDevice(aldevice);
}

void fplay_sound(int samplenum, float l_x, float l_y, float e_x, float e_y, int chan)
{
	int i, random_samp, calc[2], playmode;
	ALint state = 0;
	float dist, volume;

	if(!samp[samplenum].loaded) return;

	if(camerafollowsprite > 0)
	{
		l_x = spriteparts.pos[camerafollowsprite].x;
		l_y = spriteparts.pos[camerafollowsprite].y;
	}

	dist = sqrt((calc[0] = e_x - l_x, calc[0] *= calc[0]) + (calc[1] = e_y - l_y, calc[1] *= calc[1]));

	if(dist > 0.5 && snd_effects_battle.value)
	{
		switch(samplenum)
		{
			case SFX_M79_EXPLOSION:
				fplay_sound(SFX_DIST_M79, l_x, l_y, e_x, e_y, chan);
				break;
			case SFX_GRENADE_EXPLOSION:
			case SFX_CLUSTERGRENADE:
			case SFX_CLUSTER_EXPLOSION:
				fplay_sound(SFX_DIST_GRENADE, l_x, l_y, e_x, e_y, chan);
				break;
			case SFX_AK74_FIRE:
			case SFX_M249_FIRE:
			case SFX_RUGER77_FIRE:
			case SFX_SPAS12_FIRE:
			case SFX_DESERTEAGLE_FIRE:
			case SFX_STEYRAUG_FIRE:
			case SFX_BARRETM82_FIRE:
			case SFX_MINIGUN_FIRE:
			case SFX_COLT1911_FIRE:
				while((random_samp = random()) < 0 || random_samp > 3)
					;

				fplay_sound(81 + random_samp, l_x, l_y, e_x, e_y, chan);
				break;
			case SFX_DIST_M79:
			case SFX_DIST_GRENADE:
			case SFX_DIST_GUN1:
			case SFX_DIST_GUN2:
			case SFX_DIST_GUN3:
			case SFX_DIST_GUN4:
				if(dist > 1) dist--;
				else dist = -dist;
		}
	}

	if(grenadeeffecttimer > 0 && samplenum != SFX_HUM) dist += 10 * (grenadeeffecttimer / 7);

	if(dist > 1) return;

	if(samplenum == SFX_ROCKETZ || samplenum == SFX_CHAINSAW_R || samplenum == SFX_FLAMER) playmode = AL_TRUE;
	else playmode = AL_FALSE;

	if(chan >= RESERVED_SOURCES) return;

	if(chan < 0)
		for(i = RESERVED_SOURCES; i < MAX_SOURCES - 1; i++)
		{
			alGetSourcei(sources[i], AL_SOURCE_STATE, &state);
			if(state != AL_PLAYING)
			{
				chan = i;
				break;
			}
		}

	if(chan > 0)
	{
		alSourcei(sources[chan], AL_LOOPING, playmode);
		volume = volumeinternal * (1 - dist);
		alSourcef(sources[chan], AL_GAIN, volume);
		alSource3f(sources[chan], AL_POSITION, (e_x - l_x) / SOUND_METERLENGTH, (e_y - l_y) / SOUND_METERLENGTH, -SOUND_PANWIDTH / SOUND_METERLENGTH);
		alGetSourcei(sources[chan], AL_SOURCE_STATE, &state);

		if(state == AL_PLAYING) return;
		else if(state == AL_PAUSED) alSourceStop(sources[chan]);

		alSourcei(sources[chan], AL_BUFFER, samp[samplenum].buffer);
		alSourcePlay(sources[chan]);
	}
}

void play_sound(int sample, TVector2 emitter, int channel, int mode)
{
	if(mode == 1) fplay_sound(sample, cam_x, cam_y, cam_x, cam_y, defaultchannel);
	else if(mode == 2) fplay_sound(sample, cam_x, cam_y, cam_x, cam_y, channel);
	else if(mode == 3) fplay_sound(sample, cam_x, cam_y, emitter.x, emitter.y, defaultchannel);
	else fplay_sound(sample, cam_x, cam_y, emitter.x, emitter.y, channel);
}

int stop_sound(int channel)
{
	alSourceStop(sources[channel]);
	return 0;
}

int set_sound_paused(int channel, int paused)
{
	ALint state = 0;

	alGetSourcei(sources[channel], AL_SOURCE_STATE, &state);

	if(state == AL_PLAYING && paused) alSourcePause(sources[channel]);
	else if(state == AL_PAUSED && !paused) alSourcePlay(sources[channel]);

	return 0;
}

int set_volume(int channel, float volume)
{
	int i;

	if(channel < 0)
		for(i = 0; i < MAX_SOURCES - 1; i++)
			alSourcef(sources[i], AL_GAIN, volume);
	else alSourcef(sources[channel], AL_GAIN, volume);

	return 1;
}
