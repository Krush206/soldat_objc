#ifndef SOUND_H
#define SOUND_H

#include <AL/al.h>
#include <AL/alc.h>
#include "../shared/Vector.h"

#define MAX_SOURCES 256
#define RESERVED_SOURCES 128
#define MAX_SAMPLES 163
#define CHANNEL_WEATHER 127

typedef struct {
    ALuint loaded, buffer;
    int ret;
} TSoundSample;

typedef struct {
    char name[26];
    TSoundSample samp;
} TScriptSound;

extern TSoundSample samp[MAX_SAMPLES];
extern TScriptSound *scriptsamp;
extern float volumeinternal;
extern ALCdevice *aldevice;
extern ALCcontext *alcontext;
extern ALuint sources[MAX_SOURCES - 1];
extern int defaultchannel;

int init_sound(void);
short sound_name_to_id(char *);
TSoundSample load_sample(char *, TSoundSample);
float scale_volume_setting(int);
void load_sounds(char *);
void close_sound(void);
void fplay_sound(int, float, float, float, float, int);
void play_sound(int, TVector2, int);
int stop_sound(int);
int set_sound_paused(int, int);
int set_volume(int, float);

#endif /* SOUND_H */