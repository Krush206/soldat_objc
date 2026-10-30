/* Sound.c — C port stub */
#include "Sound.h"
#include "../shared/Vector.h"
#include <stddef.h>

TSoundSample samp[MAX_SAMPLES];
TScriptSound *scriptsamp = NULL;
float volumeinternal = 0.0f;
ALCdevice *aldevice = NULL;
ALCcontext *alcontext = NULL;
ALuint sources[MAX_SOURCES - 1];
int defaultchannel = -1;

int init_sound(void) { return 0; }
short sound_name_to_id(char *s) { return 0; }
TSoundSample load_sample(char *name, TSoundSample sampload) { TSoundSample r = {0}; return r; }
float scale_volume_setting(int v) { return 0.0f; }
void load_sounds(char *s) {}
void close_sound(void) {}
void fplay_sound(int a, float b, float c, float d, float e, int f) {}
void play_sound(int a, TVector2 v, int c) {}
int stop_sound(int a) { return 0; }
int set_sound_paused(int a, int b) { return 0; }
int set_volume(int a, float b) { return 0; }