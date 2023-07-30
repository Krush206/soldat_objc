#include <AL/al.h>
#include <AL/alc.h>

#define MAX_SOURCES 256
#define RESERVED_SOURCES 128
#define MAX_SAMPLES 163
#define CHANNEL_WEATHER 127

@interface TSoundSample: NSObject
{
        ALuint loaded, buffer;
        int ret;
}

- (void) setLoaded: (ALuint) o;
- (void) setBuffer: (ALuint) o;
- (void) setReturn: (int) o;
- (ALuint) getLoaded;
- (ALuint) getBuffer;
- (int) getReturn;
@end

@interface TScriptSound: NSObject
{
        NSString *name;
        TSoundSample *samp;
}

+ (void) setSampInit: (NSArray *) o;
+ (NSArray *) getSampInit;
- (void) setName: (NSString *) o;
- (void) setSample: (TSoundSample *) o;
- (NSString *) getName;
- (TSoundSample *) getSample;
@end

@interface Sound: NSObject
{
	TSoundSample *samp;
	NSMutableArray *scriptsamp;
	float volumeinternal;
	ALCdevice *aldevice;
	ALCcontext *alcontext;
	ALuint sources[MAX_SOURCES - 1];
	int defaultchannel;
}

- (BOOL) init_sound;
- (int) sound_name_to_id: (NSString *) name;
- (TSoundSample *) load_sample: (NSString *) name sample: (TSoundSample *) sample;
- (float) scale_volume_setting: (int) volumesetting;
- (void) load_sounds: (NSString *) moddir;
- (void) close_sound;
- (void) fplay_sound: (int) samplenum l_x: (float) l_x l_y: (float) l_y e_x: (float) e_x e_y: (float) e_y channel: (int) channel;
- (void) play_sound: (int) sample emitter: (TVector2) emitter channel: (int) channel mode: (int) mode;
- (void) stop_sound: (int) sample;
- (void) set_sound_paused: (int) channel paused: (BOOL) paused;
- (void) set_volume: (int) channel volume: (float) volume;
@end
