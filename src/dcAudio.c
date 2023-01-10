#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <malloc.h>
#include "dcAudio.h"
#include <libetc.h>

#define MUSIC_FIRST_TRACK_IDX 2

// convert Little endian to Big endian
#define SWAP_ENDIAN32(x) (((x)>>24) | (((x)>>8) & 0xFF00) | (((x)<<8) & 0x00FF0000) | ((x)<<24))
#define GET_SPU_CHANNEL(x) (0x1L<<x)

// VAG header struct (see fileformat47.pdf, p.209)
typedef struct VAGhdr {                // All the values in this header must be big endian
        char id[4];                    // VAGp         4 bytes -> 1 char * 4
        unsigned int version;          // 4 bytes
        unsigned int reserved;         // 4 bytes
        unsigned int dataSize;         // (in bytes) 4 bytes
        unsigned int samplingFrequency;// 4 bytes
        char  reserved2[12];           // 12 bytes -> 1 char * 12
        char  name[16];                // 16 bytes -> 1 char * 16
        // Waveform data after that
} VAGhdr;

void dcAudio_Init(SDC_Audio *audio, int nsfxs) {
    SpuCommonAttr spuSettings;
    // Init extended CD system
    CdInit();
    // Init Spu
    SpuInit();
    // Set master & CD volume to max
    spuSettings.mask = (SPU_COMMON_MVOLL | SPU_COMMON_MVOLR | SPU_COMMON_CDVOLL | SPU_COMMON_CDVOLR | SPU_COMMON_CDMIX);
    // Master volume should be in range 0x0000 - 0x3fff
    spuSettings.mvol.left  = 0x3fff;
    spuSettings.mvol.right = 0x3fff;
    // Cd volume should be in range 0x0000 - 0x7fff
    spuSettings.cd.volume.left = 0x7fff;
    spuSettings.cd.volume.right = 0x7fff;
    // Enable CD input ON
    spuSettings.cd.mix = SPU_ON;
    // Apply settings
    SpuSetCommonAttr(&spuSettings);
    // Set transfer mode 
    SpuSetTransferMode(SPU_TRANSFER_BY_DMA);

    SpuSetIRQ(SPU_OFF);
    SpuSetKey(SpuOff, SPU_ALLCH);

    char *spu_malloc_rec = (char*)malloc3(SPU_MALLOC_RECSIZ * (nsfxs+1));
    SpuInitMalloc(nsfxs, spu_malloc_rec);

    audio->ntoc = CdGetToc(audio->loc);

    // Prevent out of bound pos
    // for (int i = 1; i < ntoc; i++) {
    //     CdIntToPos(CdPosToInt(&loc[i]) - 74, &loc[i]);
    // }

    // Those array will hold the return values of the CD commands
    u_char param[4], result[8];
    // Set CD parameters ; Report Mode ON, CD-DA ON, and Auto Pause. See LibeOver47.pdf, p.180
    param[0] = CdlModeRept|CdlModeDA|CdlModeAP;	
    CdControlB (CdlSetmode, param, 0);	/* set mode */
    VSync (3);				/* wait three vsync times */

    audio->currTrackId = -1;
    audio->frameToUpdate = 0;
    audio->currSpuChannel = -1;
}

void dcAudio_Update(SDC_Audio *audio) {
    // Music Update
    if(audio->currTrackId >= 0){
        u_char result[8];
        audio->frameToUpdate++;
        if (audio->frameToUpdate%50 == 0){
            CdReady(1, &result[0]);
            if(result[0] == 0) 
            {
                CdControlB (CdlPlay, (u_char *)&audio->loc[audio->currTrackId+MUSIC_FIRST_TRACK_IDX], 0);
            }
        }
    }
}

void dcAudio_SfxLoad(SDC_Audio *audio, SDC_Sfx* sfx, u_char *data) {
    audio->currSpuChannel++;
    sfx->spu_channel = GET_SPU_CHANNEL(audio->currSpuChannel);
    u_int pitch;
    const VAGhdr * VAGheader = (VAGhdr *)data;
    pitch = (SWAP_ENDIAN32(VAGheader->samplingFrequency) << 12) / 44100L; 
    unsigned int VAG_data_size = SWAP_ENDIAN32(VAGheader->dataSize);
    sfx->spu_address = SpuMalloc(VAG_data_size);                           // Allocate an area of dataSize bytes in the sound buffer. 
    assert(sfx->spu_address);
    SpuSetTransferStartAddr(sfx->spu_address);                             // Sets a starting address in the sound buffer
    SpuSetTransferMode(SpuTransByDMA);                                     // DMA transfer; can do other processing during transfer
    u_long transferred = SpuWrite (data + sizeof(VAGhdr), VAG_data_size);  // transfer VAG_data_size bytes from VAG_data  address to sound buffer
    SpuIsTransferCompleted (SPU_TRANSFER_WAIT);                            // Checks whether transfer is completed and waits for completion

    sfx->voiceAttributes.mask =                              //~ Attributes (bit string, 1 bit per attribute)
    (
      SPU_VOICE_VOLL |
      SPU_VOICE_VOLR |
      SPU_VOICE_PITCH |
      SPU_VOICE_WDSA |
      SPU_VOICE_ADSR_AMODE |
      SPU_VOICE_ADSR_SMODE |
      SPU_VOICE_ADSR_RMODE |
      SPU_VOICE_ADSR_AR |
      SPU_VOICE_ADSR_DR |
      SPU_VOICE_ADSR_SR |
      SPU_VOICE_ADSR_RR |
      SPU_VOICE_ADSR_SL
    );
    sfx->voiceAttributes.voice        = sfx->spu_channel;        //~ Voice (low 24 bits are a bit string, 1 bit per voice )
    sfx->voiceAttributes.volume.left  = 0x0;                     //~ Volume 
    sfx->voiceAttributes.volume.right = 0x0;                     //~ Volume
    sfx->voiceAttributes.pitch        = pitch;                   //~ Interval (set pitch)
    sfx->voiceAttributes.addr         = sfx->spu_address;        //~ Waveform data start address
    sfx->voiceAttributes.a_mode       = SPU_VOICE_LINEARIncN;    //~ Attack rate mode  = Linear Increase - see libref47.pdf p.1091
    sfx->voiceAttributes.s_mode       = SPU_VOICE_LINEARIncN;    //~ Sustain rate mode = Linear Increase
    sfx->voiceAttributes.r_mode       = SPU_VOICE_LINEARDecN;    //~ Release rate mode = Linear Decrease
    sfx->voiceAttributes.ar           = 0x0;                     //~ Attack rate
    sfx->voiceAttributes.dr           = 0x0;                     //~ Decay rate
    sfx->voiceAttributes.rr           = 0x0;                     //~ Release rate
    sfx->voiceAttributes.sr           = 0x0;                     //~ Sustain rate
    sfx->voiceAttributes.sl           = 0xf;                     //~ Sustain level
    SpuSetVoiceAttr(&sfx->voiceAttributes);                      // set attributes
}

void dcAudio_SfxPlay(SDC_Sfx* sfx) {
    // Set voice volume to max
    sfx->voiceAttributes.mask= ( SPU_VOICE_VOLL | SPU_VOICE_VOLR );
    sfx->voiceAttributes.voice        = sfx->spu_channel;
    sfx->voiceAttributes.volume.left  = 0x1000;
    sfx->voiceAttributes.volume.right = 0x1000;
    SpuSetVoiceAttr(&sfx->voiceAttributes);
    // Play voice
    SpuSetKey(SpuOn, sfx->spu_channel);
}

void dcAudio_MusicPlay(SDC_Audio *audio, u_char trackId) {
    if(trackId > audio->ntoc-MUSIC_FIRST_TRACK_IDX)
        return;

    // Play second track in toc array
    CdControlB (CdlPlay, (u_char *)&audio->loc[trackId+MUSIC_FIRST_TRACK_IDX], 0);
    audio->currTrackId = trackId;
}

void dcAudio_MusicStop(SDC_Audio *audio) {
    CdControlB (CdlStop, 0, 0);
    audio->currTrackId = -1;
}

int  dcAudio_MusicGetNTracks(SDC_Audio *audio) {
    return audio->ntoc - 1;
}


