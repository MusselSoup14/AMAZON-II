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
#include "amazon2/soundmixer.h"

enum
{
    IMGS_SURF = 0,
    IMGS_JPEG,
    IMGS_MAX
};

typedef struct _tagIMGS_HEADER
{
    U32 magic;//imgs
    U32 version;
    U32 type;//JPEG,SURF
    U32 w;
    U32 h;
    U32 fps;
    U32 cnt;//image count
    U32 offset_start;
    U32 data_offset;
    U32 au_type;//0=wav
    U32 au_data_offset;
    U32 au_len;
} MOVIE_HEADER;

typedef enum
{
    MOVIE_STAT_STOP = 0,
    MOVIE_STAT_RUN ,
    MOVIE_STAT_PAUSE ,
} MOVIE_STAT;

typedef struct _tagMOVIE
{
    MOVIE_HEADER header;
    U32 image_count;///< Count of Images
    U32* img_offset;
    U32 cur_num;
    BOOL bLoop;
    BYTE* buf;
    U32 buf_size;
    BOOL bLoadAll;
    FIL fp;
    SURFACE* surf;
    BOOL bEnableAudio;//internal used
    WAVE* pWave;
    U8* pWavbuf;
    MOVIE_STAT stat;
    SURFACE** surflist;//if image is surf, it is used
} MOVIE;
typedef enum
{
    MOVIE_DRAW_RESULT_FAIL = 0,
    MOVIE_DRAW_RESULT_OK,
    MOVIE_DRAW_RESULT_EOF,
} MOVIE_DRAW_RESULT;

MOVIE* loadmovie( char* fname, BOOL bAllLoad );
void release_movie( MOVIE* mov );
BOOL movie_play( MOVIE* mov, int x, int y, BOOL bAudio );
BOOL movie_playex(MOVIE* mov, int x, int y, int w, int h, BOOL bAudio);
MOVIE_DRAW_RESULT movie_drawnext( MOVIE* mov, int x, int y, BOOL b_audiosync );
MOVIE_DRAW_RESULT movie_drawnextex( MOVIE* mov, int x, int y, int w, int h, BOOL b_audiosync );
U32 movie_seek( MOVIE* mov, U32 ms );
U32 movie_current_time( MOVIE* mov );
