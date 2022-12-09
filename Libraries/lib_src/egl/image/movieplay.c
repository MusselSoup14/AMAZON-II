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

#include "sdk.h"
/**
 * Load movies 
 *
 * \param fname Pointer to a null-terminated string
 * \param bAll			 true to load all images at memory.
 *
 * \return null if it fails, else.
 */
MOVIE* loadmovie( char* fname, BOOL bAll )
{
	U32 nRead;
	FIL fp;
	MOVIE* mov = ( MOVIE* )malloc( sizeof( MOVIE ) );
	if ( mov == NULL )
		return NULL;
	memset( mov, 0, sizeof( MOVIE ) );
	/* load windows BMP file */
	FRESULT res = f_open( &fp, fname, FA_READ | FA_OPEN_EXISTING );
	if ( res != FR_OK )
	{
		DEBUGPRINTF( "Cannot open : %s\r\n", fname );
		return 0;
	}

	memset( mov, 0, sizeof( MOVIE ) );

	f_read( &fp, &mov->header, sizeof( MOVIE_HEADER ), &nRead );

	if ( mov->header.magic != ( 'I' + ( 'M' << 8 ) + ( 'G' << 16 ) + ( 'S' << 24 ) ) )
	{
		free( mov );
		return NULL;
	}
	mov->img_offset = malloc( sizeof( U32 ) * mov->header.cnt );
	if ( mov->img_offset == NULL )
	{
		free( mov );
		return NULL;
	}
	f_lseek( &fp, mov->header.offset_start );
	f_read( &fp, mov->img_offset, sizeof( U32 )*mov->header.cnt, &nRead );
	if ( mov->header.type >= IMGS_MAX )
	{
		debugstring( "Not supported MOVIE format\r\n" );
		free( mov );
		return NULL;
	}
	if ( ( mov->header.au_data_offset != 0 ) && ( mov->header.au_type == 0 ) )
	{
		f_lseek( &fp, mov->header.au_data_offset );
		U32 nSize = mov->header.au_len;
		mov->pWavbuf = malloc( nSize );
		if ( mov->pWavbuf == NULL )
		{
			DEBUGPRINTF( "Not enough memory\r\n" );
			free( mov->img_offset );
			free( mov );
			f_close( &fp );
			return NULL;
		}

		f_read( &fp, mov->pWavbuf, nSize, &nRead );
		mov->pWave = sound_loadwavp( mov->pWavbuf, nRead );
		if ( mov->pWave == NULL )
		{
			free( mov->pWavbuf );
			mov->pWavbuf = NULL;
		}

	}
	if ( bAll )
	{
		U32 nSize;
		if ( mov->header.au_data_offset == 0 )
			nSize = f_size( &fp ) - mov->header.data_offset;
		else
			nSize = mov->header.au_data_offset - mov->header.data_offset;
		f_lseek( &fp, mov->header.data_offset );
		if ( mov->header.type == IMGS_JPEG )
		{
			mov->buf = malloc( nSize );
			if ( mov->buf == NULL )
			{
				DEBUGPRINTF( "Not enough memory\r\n" );
				free( mov->img_offset );
				free( mov );
				f_close( &fp );
				if ( mov->pWave )
					sound_release( mov->pWave );
				if ( mov->pWavbuf )
					free( mov->pWavbuf );
				return NULL;
			}
			f_read( &fp, mov->buf, nSize, &nRead );
			mov->buf_size = nRead;
		}
		else if ( mov->header.type == IMGS_SURF )
		{
			mov->surflist = ( SURFACE** )malloc( sizeof( void* ) * mov->header.cnt);
			if ( mov->surflist == NULL )
			{
				DEBUGPRINTF( "Not enough memory\r\n" );
				free( mov->img_offset );
				free( mov );
				f_close( &fp );
				if ( mov->pWave )
					sound_release( mov->pWave );
				if ( mov->pWavbuf )
					free( mov->pWavbuf );
				return NULL;
			}
			//load all surface image
			int i;
			for ( i = 0; i < mov->header.cnt; i++ )
			{
				U32 offset = mov->header.data_offset + mov->img_offset[i];
				f_lseek(&mov->fp, offset);
				mov->surflist[i] = loadsurf_filepos( &fp );
			}
		}

		f_close( &fp );

	}
	else
	{
		mov->fp = fp;
	}
	mov->image_count = mov->header.cnt;
	mov->bLoop = FALSE;
	mov->cur_num = 0;
	if ( bAll )
		mov->bLoadAll = TRUE;
	else
		mov->bLoadAll = FALSE;

	return mov;
}

/**
 * Releases the movie .
 *
 * \param [in] mov Pointer the movie.
 */
void release_movie( MOVIE* mov )
{
	if ( mov == NULL )
		return;
	if ( mov->buf )
		free( mov->buf );
	if ( mov->img_offset )
		free( mov->img_offset );
	if ( mov->bLoadAll == FALSE )
		f_close( &mov->fp );
	if ( mov->surf )
		release_surface( mov->surf );
	if ( mov->pWave )
		sound_release( mov->pWave );
	if ( mov->pWavbuf )
		free( mov->pWavbuf );
	if ( mov->surflist )
	{
		int i;
		for ( i = 0; i < mov->image_count; i++ )
			release_surface( mov->surflist[i] );
		free( mov->surflist );
	}
	free( mov );
}
/**
 *
 * \param [in] mov Pointer to MOVIE.
 * \param img_num	   The image number.
 * \param x			   The x coordinate.
 * \param y			   The y coordinate.
 * \param w			   The width.
 * \param h			   The height.
 */
static void movie_drawnext_r( MOVIE* mov, int img_num, int x, int y, int w, int h )
{
	U32 fsize;
	U32 nRead;
	SURFACE* surf = NULL;
	if (mov->cur_num == img_num && mov->surf != NULL)
	{
		draw_surface_scalerect(mov->surf, x, y, w, h, 0, 0, mov->surf->w, mov->surf->h);
		return;
	}

	if ( mov->bLoadAll )
	{
		if ( img_num == ( mov->image_count - 1 ) )
		{
			fsize = mov->buf_size - mov->img_offset[img_num];
		}
		else
		{
			fsize = mov->img_offset[img_num + 1] - mov->img_offset[img_num];
		}
		if ( fsize == 0 )
		{
			mov->cur_num = img_num;
			draw_surface_scalerect( mov->surf, x, y, w, h, 0, 0, mov->surf->w, mov->surf->h );
			return;
		}

		U8* imgbuf = mov->buf + mov->img_offset[img_num];
		switch ( mov->header.type )
		{
		case IMGS_SURF:
			surf = mov->surflist[img_num];
			draw_surface_scalerect( surf, x, y, w, h, 0, 0, surf->w, surf->h );
			mov->surf = surf;
			mov->cur_num = img_num;
			return;
		case IMGS_JPEG:
			surf = loadjpgp_hw( ( U32 )imgbuf, fsize );
			break;
		default:
			return;
		}

	}
	else
	{
		if ( img_num == ( mov->image_count - 1 ) )
		{
			if ( mov->header.au_data_offset == 0 )
				fsize = f_size( &mov->fp ) - mov->img_offset[img_num] - mov->header.data_offset;
			else
				fsize = mov->header.au_data_offset - mov->img_offset[img_num];
		}
		else
		{
			fsize = mov->img_offset[img_num + 1] - mov->img_offset[img_num];
		}

		if ( fsize == 0 )
		{
			mov->cur_num = img_num;
			if ( mov->surf != NULL )
				draw_surface_scalerect( mov->surf, x, y, w, h, 0, 0, mov->surf->w, mov->surf->h );
			return;
		}

		U32 offset = mov->header.data_offset + mov->img_offset[img_num];
		f_lseek( &mov->fp, offset );
		switch ( mov->header.type )
		{
		case IMGS_SURF:
			surf = loadsurf_filepos( &mov->fp );
			break;
		case IMGS_JPEG:
		{
			U8* imgbuf = malloc( fsize );
			if ( imgbuf == NULL )
			{
				DEBUGPRINTF( "Not enough memory\r\n" );
				return;
			}
			f_read( &mov->fp, imgbuf, fsize, &nRead );
			surf = loadjpgp_hw( ( U32 )imgbuf, fsize );
			free( imgbuf );
			break;
		}
		default:
			return;
		}
	}
	if ( surf == NULL )
	{
		return;
	}
	draw_surface_scalerect( surf, x, y, w, h, 0, 0, surf->w, surf->h );
	release_surface( mov->surf );
	mov->surf = surf;
	mov->cur_num = img_num;
}
/**
 * Draw the next Video image
 *
 * \param [in,out] mov Pointer the MOVIE
 * \param x			   The x coordinate.
 * \param y			   The y coordinate.
 * \param b_audiosync  true to audio-sync.
 *
 * \return .
 */
MOVIE_DRAW_RESULT movie_drawnext( MOVIE* mov, int x, int y, BOOL b_audiosync )
{
	return movie_drawnextex( mov, x, y, mov->header.w, mov->header.h, b_audiosync );
}
/**
 * Draw the next Video image
 *
 * \param [in,out] mov Pointer the MOVIE
 * \param x			   The x coordinate.
 * \param y			   The y coordinate.
 * \param w			   The width.
 * \param h			   The height.
 * \param b_audiosync  true to audio-sync.
 *
 * \return .
 */
MOVIE_DRAW_RESULT movie_drawnextex( MOVIE* mov, int x, int y, int w, int h, BOOL b_audiosync )
{
	if ( ( mov == NULL ) || ( mov->stat == MOVIE_STAT_STOP ) )
		return MOVIE_DRAW_RESULT_FAIL;
	if ( mov->stat == MOVIE_STAT_PAUSE )
	{
		movie_drawnext_r( mov, mov->cur_num, x, y, w, h );
		if ( mov->cur_num == ( mov->image_count - 1 ) )
			return MOVIE_DRAW_RESULT_EOF;
		return MOVIE_DRAW_RESULT_OK;
	}
	int next_num;
	int re = MOVIE_DRAW_RESULT_OK;
	if ( b_audiosync && mov->bEnableAudio )
	{
		if ( mov->pWave == NULL )
			return -1;
		if ( mov->bLoop == FALSE )
		{
			if ( sound_isplay( mov->pWave ) == FALSE ) //movie end
			{
				re = MOVIE_DRAW_RESULT_EOF;
				next_num = mov->image_count - 1;
			}
		}
		if ( re != MOVIE_DRAW_RESULT_EOF )
		{
			U32 audio_ms = sound_current_time( mov->pWave );
			next_num = mov->header.fps * audio_ms / 1000;
			if ( next_num == mov->cur_num )
			{
				next_num++;
			}
			else if ( next_num < mov->cur_num )
			{
				next_num = mov->cur_num;
			}
			else if ( next_num > ( mov->cur_num + 1 ) )
			{
				next_num = mov->cur_num + 2;
				debugprintf("skip %d\r\n",next_num-mov->cur_num-1);
			}

			if ( next_num >= mov->image_count )
				next_num = mov->image_count - 1;
		}

	}
	else
	{
		next_num = mov->cur_num + 1;
		if ( next_num >= mov->image_count )
		{
			if ( mov->bLoop )
				next_num = 0;//loop;
			else
			{
				next_num = mov->image_count - 1;
				re = MOVIE_DRAW_RESULT_EOF;//ended
			}
		}
	}
	movie_drawnext_r( mov, next_num, x, y, w, h );
	return re;

}
/**
 * seek movie.
 *
 * \param mov Pointer to MOVIE.
 * \param ms The milliseconds.
 *
 * \return Number of video frame.
 */
U32 movie_seek( MOVIE* mov, U32 ms )
{
	int frame = mov->header.fps * ms / 1000;
	if ( frame < ( mov->image_count - 1 ) )
		mov->cur_num = frame;
	else
		frame = mov->image_count - 1;
	if ( mov->bEnableAudio )
	{
		if ( sound_isplay( mov->pWave ) )
		{
			sound_stop( mov->pWave );
			sound_playex( mov->pWave, ms );
		}

	}
	return frame;
}
/**
 * pause movie.
 *
 * \param mov Pointer to MOVIE.
 */
void movie_pause( MOVIE* mov )
{
	if ( mov == NULL )
		return ;

	if ( mov->bEnableAudio )
	{
		sound_pause( mov->pWave );
	}
	mov->stat = MOVIE_STAT_PAUSE;
}
/**
 * resume Movie.
 *
 * \param mov Pointer to MOVIE.
 */
void movie_resume( MOVIE* mov )
{
	if ( mov == NULL )
		return ;
	if ( mov->stat != MOVIE_STAT_PAUSE )
		return ;

	if ( mov->bEnableAudio )
	{
		sound_resume( mov->pWave );
	}
	mov->stat = MOVIE_STAT_RUN;
}

/**
 * stop Movie.
 *
 * \param mov Pointer to MOVIE.
 */
void movie_stop( MOVIE* mov )
{
	if ( mov == NULL )
		return;
	if ( mov->bEnableAudio )
	{
		sound_stop( mov->pWave );
	}
	mov->stat = MOVIE_STAT_STOP;
}
/**
 * Movie play.
 *
 * \param mov Pointer to MOVIE.
 * \param x			   The x coordinate.
 * \param y			   The y coordinate.
 * \param w			   The width.
 * \param h			   The height.
 * \param bAudio	   true to audio.
 *
 * \return true if it succeeds, false if it fails.
 */

BOOL movie_playex(MOVIE* mov, int x, int y, int w, int h, BOOL bAudio)
{
	if (mov == NULL)
	{
		return FALSE;
	}
	movie_seek(mov, 0);
	if (bAudio == TRUE)
	{
		if (mov->pWave == NULL)
			return FALSE;
		sound_stop(mov->pWave);
		if (sound_play(mov->pWave) == FALSE)
			return FALSE;
	}
	mov->bEnableAudio = bAudio;
	mov->stat = MOVIE_STAT_RUN;
	movie_drawnext_r(mov, 0, x, y, w, h);
	return TRUE;
}
BOOL movie_play(MOVIE* mov, int x, int y, BOOL bAudio)
{
	return movie_playex(mov, x, y, mov->header.w, mov->header.h, bAudio);
}
/**
 * Movie current playing time.
 *
 * \param mov Pointer to MOVIE
 *
 * \return milliseconds
 */
U32 movie_current_time( MOVIE* mov )
{
	if (mov == NULL)
		return 0;

	int sec = mov->cur_num / mov->header.fps;
	int ms = 1000 * ( mov->cur_num % mov->header.fps ) / mov->header.fps;
	return ( sec * 1000 ) + ms;
}
