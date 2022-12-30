#ifndef _DC_AUDIO_
#define _DC_AUDIO_

#include <types.h>
#include <libcd.h>
#include <libspu.h>

typedef struct {
    CdlLOC loc[10];
    int ntoc;
    int currTrackId;
    int frameToUpdate;
    int currSpuChannel;
} SDC_Audio;

typedef struct {
    u_long spu_channel;      // SPU voice to playback to
    u_long spu_address;      // SPU address for memory freeing spu mem
    SpuVoiceAttr voiceAttributes;
} SDC_Sfx;

void dcAudio_Init(SDC_Audio *audio, int nsfxs);
void dcAudio_Update(SDC_Audio *audio);

// Sfx functions ----
void dcAudio_SfxLoad(SDC_Audio *audio, SDC_Sfx* sfx, u_char *data); //data -> Pointer to VAG data address
void dcAudio_SfxPlay(SDC_Sfx* sfx);

// Music functions -----
// the music always is looping
void dcAudio_MusicPlay(SDC_Audio *audio, u_char trackId); // 0...to NTracks (getNTracks)
void dcAudio_MusicStop(SDC_Audio *audio);
int  dcAudio_MusicGetNTracks(SDC_Audio *audio);

#endif