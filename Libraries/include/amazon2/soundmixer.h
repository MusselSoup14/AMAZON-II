/*****************************************************************************
*
* Copyright (C) 2014      Advanced Digital Chips, Inc. All Rights Reserved.
*						http://www.adc.co.kr
*
* THIS SOFTWARE IS PROVIDED BY ADCHIPS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
* EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ADCHIPS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE
*
*****************************************************************************/
#pragma once

#define _SND_BASE_ 0xF4005000  

#define R_SNDCON(ch) ((volatile U32*)(_SND_BASE_+ (0x10*(ch))))
#define R_SNDINR	((volatile U32*)(_SND_BASE_+0x70))
#define R_SNDVOL(ch)	((volatile U32*)(_SND_BASE_+0x04+(0x10*(ch))))
#define R_SNDVOLINR	((volatile U32*)(_SND_BASE_+0x74))
#define R_SNDVOLOUT(ch)	((volatile U32*)(_SND_BASE_+0x84 +(0x10*(ch))))
#define R_SNDBST(ch)	((volatile U32*)(_SND_BASE_+0x08+(0x10*(ch))))
#define R_SNDBSTINR	((volatile U32*)(_SND_BASE_+0x78))
#define R_SNDDAT(ch)	((volatile U32*)(_SND_BASE_+0x0c+(0x10*(ch))))
#define R_SNDDATINR	((volatile U32*)(_SND_BASE_+0x7c))
#define R_SNDOUT(ch)	((volatile U32*)(_SND_BASE_+0x8c+(0x10*(ch))))
#define R_SNDIST	((volatile U32*)(_SND_BASE_+0xc0))

typedef enum
{
	PCMTYPE_US8=0,
	PCMTYPE_UM8,
	PCMTYPE_SS8,
	PCMTYPE_SM8,
	PCMTYPE_US16,
	PCMTYPE_UM16,
	PCMTYPE_SS16,
	PCMTYPE_SM16,
}PCMTYPE;
enum 
{
	WAVE_STATE_STOP=0,
	WAVE_STATE_PLAY,
	WAVE_STATE_PAUSE,
};
typedef struct _tagWAVE {
	U32 freq;  ///< Audio frequency in samples per second
	U8* samples; ///< 
	U32 bufsize;///< it is not a length of buf but valid data length in samples, should be 4*8byte align. real buffer size is WAV_BUF_MAX
	U8* samples2; ///< ping-pong buffer
	U32 bufsize2;///< it is not a length of buf but valid data length in samples, should be 4*8byte align. real buffer size is WAV_BUF_MAX
	U32 byterate;///byte per sec
	int headersize;
	volatile int cursamplesnum;
	volatile bool validsamples;//for samples
	volatile bool validsamples2;//for samples2
	bool bNeedReload;
	PCMTYPE pcmtype; ///< PCM8,PCM16 
	S8  channels;///< Number of channels: 1 mono, 2 stereo
	S16 PlaybackPitch;
	U8	bLoop;//0 : one-shot, 1: continuous
	S8  channelNumber;//allocated channel number
	U8  volume;
	U8	panpot;
	char filename[128];
	U32  datasize;// full data size
	volatile int state;//pause, stop play
	FIL file;
	bool bMallocedBuf;///< sound_release will check it.
	bool (*preparetoplay)(struct _tagWAVE* pWave);
	void (*TxIsr)(int dmach, struct _tagWAVE* pWave);//called by DMA-ISR when data-transfer was completed.you can use it for your decoder , see the sma_isr_r() at soundmixer.c
	void* user;//for  mp3 file
	void (*release)(void* user);//for extend, maybe used for free user-memory 

	volatile U32 transfered_len;
	U32(*current_time)(struct _tagWAVE* pWav);//return milliseconds
	U32 duration_ms;//in milliseconds
}WAVE;

BOOL sound_init();
BOOL sound_stop(WAVE* pWave);
BOOL sound_resume(WAVE* pWave);
BOOL sound_pause(WAVE* pWave);
WAVE* sound_loadwav(char* filename);
WAVE* sound_loadwavp(U8* buf,U32 len);
WAVE* sound_loadrawp(void* sampels, int freq, PCMTYPE type, int bytelen);
BOOL sound_play(WAVE* pWave);
BOOL sound_playex(WAVE* pWave, U32 start_ms);
BOOL sound_ispause(WAVE* pWave);
BOOL sound_isplay(WAVE* pWave);
BOOL sound_release(WAVE* pWave);
void sound_vol(U8 vol);
void sound_vol_wav(WAVE* pWav,U8 vol);

WAVE* sound_loadmp3(char* filename); //implemented at mp3play.c 
WAVE* sound_loadmp3p(void* buf,U32 mp3databufsize);

void snd_dmatransfer(int dmach,int sndch,void* src, int len);

U32 sound_current_time(WAVE* pWav);//return current play time (milliseconds)

#define SOUND_MAX_CHANNEL 4

