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
#include "mad/mad.h"

typedef struct 
{
	struct mad_stream Stream;
	struct mad_frame Frame;
	struct mad_synth Synth;
	mad_timer_t Timer;
	unsigned char* inputbuffer;
	FIL* fp;
	U8* mp3databuf;//if not use file-system, it is used.
	U32 mp3databuf_readindex;
	U32 mp3databufsize;
	int framecount;
}MAD_STRUCT;

#define INPUT_BUFFER_SIZE	(5*8192)


typedef unsigned char id3_byte_t;
typedef unsigned long id3_length_t;


/* tag flags */

enum {
  ID3_TAG_FLAG_UNSYNCHRONISATION     = 0x80,
  ID3_TAG_FLAG_EXTENDEDHEADER        = 0x40,
  ID3_TAG_FLAG_EXPERIMENTALINDICATOR = 0x20,
  ID3_TAG_FLAG_FOOTERPRESENT         = 0x10,

  ID3_TAG_FLAG_KNOWNFLAGS            = 0xf0
};
enum tagtype {
  TAGTYPE_NONE = 0,
  TAGTYPE_ID3V1,
  TAGTYPE_ID3V2,
  TAGTYPE_ID3V2_FOOTER
};

static
enum tagtype tagtype(id3_byte_t const *data, id3_length_t length)
{
  if (length >= 3 &&
      data[0] == 'T' && data[1] == 'A' && data[2] == 'G')
    return TAGTYPE_ID3V1;

  if (length >= 10 &&
      ((data[0] == 'I' && data[1] == 'D' && data[2] == '3') ||
       (data[0] == '3' && data[1] == 'D' && data[2] == 'I')) &&
      data[3] < 0xff && data[4] < 0xff &&
      data[6] < 0x80 && data[7] < 0x80 && data[8] < 0x80 && data[9] < 0x80)
    return data[0] == 'I' ? TAGTYPE_ID3V2 : TAGTYPE_ID3V2_FOOTER;

  return TAGTYPE_NONE;
}


unsigned long id3_parse_uint(id3_byte_t const **ptr, unsigned int bytes)
{
  unsigned long value = 0;


  switch (bytes) {
  case 4: value = (value << 8) | *(*ptr)++;
  case 3: value = (value << 8) | *(*ptr)++;
  case 2: value = (value << 8) | *(*ptr)++;
  case 1: value = (value << 8) | *(*ptr)++;
  }

  return value;
}

unsigned long id3_parse_syncsafe(id3_byte_t const **ptr, unsigned int bytes)
{
  unsigned long value = 0;



  switch (bytes) {
  case 5: value = (value << 4) | (*(*ptr)++ & 0x0f);
  case 4: value = (value << 7) | (*(*ptr)++ & 0x7f);
          value = (value << 7) | (*(*ptr)++ & 0x7f);
	  value = (value << 7) | (*(*ptr)++ & 0x7f);
	  value = (value << 7) | (*(*ptr)++ & 0x7f);
  }

  return value;
}

static
void parse_header(id3_byte_t const **ptr,
		  unsigned int *version, int *flags, id3_length_t *size)
{
  *ptr += 3;

  *version = id3_parse_uint(ptr, 2);
  *flags   = id3_parse_uint(ptr, 1);
  *size    = id3_parse_syncsafe(ptr, 4);
}

/*
 * NAME:	tag->query()
 * DESCRIPTION:	if a tag begins at the given location, return its size
 */
signed long id3_tag_query(id3_byte_t const *data, id3_length_t length)
{
  unsigned int version;
  int flags;
  id3_length_t size;
  switch (tagtype(data, length)) {
  case TAGTYPE_ID3V1:
    return 128;

  case TAGTYPE_ID3V2:
    parse_header(&data, &version, &flags, &size);

    if (flags & ID3_TAG_FLAG_FOOTERPRESENT)
      size += 10;

    return 10 + size;

  case TAGTYPE_ID3V2_FOOTER:
    parse_header(&data, &version, &flags, &size);
    return size - 10;

  case TAGTYPE_NONE:
    break;
  }

  return 0;
}


int static mp3idtagsize=0;
/*
 * NAME:	decode->error()
 * DESCRIPTION:	handle a decoding error
 */
static
enum mad_flow decode_error(struct mad_stream *stream,
			   struct mad_frame *frame)
{
signed long tagsize;
  switch (stream->error) {
  case MAD_ERROR_BADDATAPTR:
    return MAD_FLOW_CONTINUE;

  case MAD_ERROR_LOSTSYNC:
    tagsize = id3_tag_query(stream->this_frame,
			    stream->bufend - stream->this_frame);
	mp3idtagsize=tagsize;
    if (tagsize > 0) {      
		
		mad_stream_skip(stream, tagsize);
      return MAD_FLOW_CONTINUE;
    }

    /* fall through */

  default:
    return MAD_FLOW_IGNORE;
  }

  return MAD_FLOW_CONTINUE;
}

			   
/****************************************************************************
* Return an error string associated with a mad error code.					*
****************************************************************************/
/* Mad version 0.14.2b introduced the mad_stream_errorstr() function.
* For previous library versions a replacement is provided below.
*/
#if (MAD_VERSION_MAJOR>=1) || \
	((MAD_VERSION_MAJOR==0) && \
	(((MAD_VERSION_MINOR==14) && \
	(MAD_VERSION_PATCH>=2)) || \
	(MAD_VERSION_MINOR>14)))
#define MadErrorString(x) mad_stream_errorstr(x)
#else
static const char *MadErrorString(const struct mad_stream *Stream)
{
	switch(Stream->error)
	{
		/* Generic unrecoverable errors. */
	case MAD_ERROR_BUFLEN:
		return("input buffer too small (or EOF)");
	case MAD_ERROR_BUFPTR:
		return("invalid (null) buffer pointer");
	case MAD_ERROR_NOMEM:
		return("not enough memory");

		/* Frame header related unrecoverable errors. */
	case MAD_ERROR_LOSTSYNC:
		return("lost synchronization");
	case MAD_ERROR_BADLAYER:
		return("reserved header layer value");
	case MAD_ERROR_BADBITRATE:
		return("forbidden bitrate value");
	case MAD_ERROR_BADSAMPLERATE:
		return("reserved sample frequency value");
	case MAD_ERROR_BADEMPHASIS:
		return("reserved emphasis value");

		/* Recoverable errors */
	case MAD_ERROR_BADCRC:
		return("CRC check failed");
	case MAD_ERROR_BADBITALLOC:
		return("forbidden bit allocation value");
	case MAD_ERROR_BADSCALEFACTOR:
		return("bad scalefactor index");
	case MAD_ERROR_BADFRAMELEN:
		return("bad frame length");
	case MAD_ERROR_BADBIGVALUES:
		return("bad big_values count");
	case MAD_ERROR_BADBLOCKTYPE:
		return("reserved block_type");
	case MAD_ERROR_BADSCFSI:
		return("bad scalefactor selection info");
	case MAD_ERROR_BADDATAPTR:
		return("bad main_data_begin pointer");
	case MAD_ERROR_BADPART3LEN:
		return("bad audio data length");
	case MAD_ERROR_BADHUFFTABLE:
		return("bad Huffman table select");
	case MAD_ERROR_BADHUFFDATA:
		return("Huffman data overrun");
	case MAD_ERROR_BADSTEREO:
		return("incompatible block_type for JS");

		/* Unknown error. This switch may be out of sync with libmad's
		* defined error codes.
		*/
	default:
		return("Unknown error code");
	}
}
#endif


/****************************************************************************
* Converts a sample from libmad's fixed point number format to a signed	*
* short (16 bits).															*
****************************************************************************/
#ifdef SHRT_MIN
#undef SHRT_MIN
#endif
	#define SHRT_MIN    (-32768)        /* minimum (signed) short value */
#ifdef SHRT_MAX
#undef SHRT_MAX
#endif
	#define SHRT_MAX      32767         /* maximum (signed) short value */
#ifdef USHRT_MAX
#undef USHRT_MAX
#endif
#define USHRT_MAX     0xffff        /* maximum unsigned short value */

#define MadFixedToSshort(Fixed,output)\
	do {\
	if(Fixed>=MAD_F_ONE)\
	output = SHRT_MAX;\
	else if(Fixed<=-MAD_F_ONE)\
	output = (-SHRT_MAX);\
	else\
	output=Fixed>>(MAD_F_FRACBITS-15);\
	}while(0)
#if 0
inline static signed short MadFixedToSshort(mad_fixed_t Fixed)
{
	/* A fixed point number is formed of the following bit pattern:
	*
	* SWWWFFFFFFFFFFFFFFFFFFFFFFFFFFFF
	* MSB                          LSB
	* S ==> Sign (0 is positive, 1 is negative)
	* W ==> Whole part bits
	* F ==> Fractional part bits
	*
	* This pattern contains MAD_F_FRACBITS fractional bits, one
	* should alway use this macro when working on the bits of a fixed
	* point number. It is not guaranteed to be constant over the
	* different platforms supported by libmad.
	*
	* The signed short value is formed, after clipping, by the least
	* significant whole part bit, followed by the 15 most significant
	* fractional part bits. Warning: this is a quick and dirty way to
	* compute the 16-bit number, madplay includes much better
	* algorithms.
	*/

	/* Clipping */
	if(Fixed>=MAD_F_ONE)
		return(SHRT_MAX);
	if(Fixed<=-MAD_F_ONE)
		return(-SHRT_MAX);

	/* Conversion. */
	Fixed=Fixed>>(MAD_F_FRACBITS-15);

	return((signed short)Fixed);
}
#endif
/****************************************************************************
* Print human readable informations about an audio MPEG frame.				*
****************************************************************************/
static int PrintFrameInfo( struct mad_header *Header)
{
	const char	*Layer,
		*Mode,
		*Emphasis;

	/* Convert the layer number to it's printed representation. */
	switch(Header->layer)
	{
	case MAD_LAYER_I:
		Layer="I";
		break;
	case MAD_LAYER_II:
		Layer="II";
		break;
	case MAD_LAYER_III:
		Layer="III";
		break;
	default:
		Layer="(unexpected layer value)";
		break;
	}

	/* Convert the audio mode to it's printed representation. */
	switch(Header->mode)
	{
	case MAD_MODE_SINGLE_CHANNEL:
		Mode="single channel";
		break;
	case MAD_MODE_DUAL_CHANNEL:
		Mode="dual channel";
		break;
	case MAD_MODE_JOINT_STEREO:
		Mode="joint (MS/intensity) stereo";
		break;
	case MAD_MODE_STEREO:
		Mode="normal LR stereo";
		break;
	default:
		Mode="(unexpected mode value)";
		break;
	}

	/* Convert the emphasis to it's printed representation. Note that
	* the MAD_EMPHASIS_RESERVED enumeration value appeared in libmad
	* version 0.15.0b.
	*/
	switch(Header->emphasis)
	{
	case MAD_EMPHASIS_NONE:
		Emphasis="no";
		break;
	case MAD_EMPHASIS_50_15_US:
		Emphasis="50/15 us";
		break;
	case MAD_EMPHASIS_CCITT_J_17:
		Emphasis="CCITT J.17";
		break;
#if (MAD_VERSION_MAJOR>=1) || \
	((MAD_VERSION_MAJOR==0) && (MAD_VERSION_MINOR>=15))
	case MAD_EMPHASIS_RESERVED:
		Emphasis="reserved(!)";
		break;
#endif
	default:
		Emphasis="(unexpected emphasis value)";
		break;
	}

	DEBUGPRINTF("%d kb/s audio MPEG layer %s stream %s CRC, "
		"%s with %s emphasis at %d Hz sample rate\r\n",
		Header->bitrate/1000,Layer,
		Header->flags&MAD_FLAG_PROTECTION?"with":"without",
		Mode,Emphasis,Header->samplerate);
	return 0;
}


int refilldata(unsigned char* InputBuffer,MAD_STRUCT* mad,struct mad_stream* pStream)
{
	UINT nRead=0;
	unsigned char* GuardPtr;
	FIL* infp = mad->fp;

	if(pStream->buffer==NULL || pStream->error==MAD_ERROR_BUFLEN)
	{
		size_t			ReadSize,Remaining;
		unsigned char	*ReadStart;

		if(pStream->next_frame!=NULL)
		{
			Remaining=pStream->bufend-pStream->next_frame;
			memmove(InputBuffer,pStream->next_frame,Remaining);
			ReadStart=InputBuffer+Remaining;
			ReadSize=INPUT_BUFFER_SIZE-Remaining;
		}
		else
		{
			ReadSize=INPUT_BUFFER_SIZE;
			ReadStart=InputBuffer;
			Remaining=0;
		}
		if(mad->mp3databuf)
		{
			if((mad->mp3databufsize - mad->mp3databuf_readindex)==0)
				return 1;//end
			if(ReadSize> (mad->mp3databufsize - mad->mp3databuf_readindex))
			{
				ReadSize = (mad->mp3databufsize - mad->mp3databuf_readindex);
			}
			memcpy(ReadStart,mad->mp3databuf+mad->mp3databuf_readindex,ReadSize);
			mad->mp3databuf_readindex+=ReadSize;
		}
		else
		{
			if(f_read(infp,ReadStart,ReadSize,&nRead)!=FR_OK)
				return 1;
			if(nRead<1)
			{
				return 1;//end of file
			}
			ReadSize = nRead;
		}
		GuardPtr=ReadStart+ReadSize;
		if(GuardPtr < (InputBuffer+ INPUT_BUFFER_SIZE))
		{
			memset(GuardPtr,0,MAD_BUFFER_GUARD);
			ReadSize+=MAD_BUFFER_GUARD;
		}

		/* Pipe the new buffer content to libmad's stream decoder
		* facility.
		*/
		mad_stream_buffer(pStream,InputBuffer,ReadSize+Remaining);
		pStream->error = MAD_ERROR_NONE;
	}
	return 0;
}
/*
retrun : PCM data length
*/
int ISPM decode_one_frame(MAD_STRUCT* mad,unsigned char* OutputPtr,int maxframe)
{
	int i;
	int	Status=0;
	struct mad_stream* pStream = &mad->Stream;
	struct mad_frame* pFrame = &mad->Frame;
	struct mad_synth* pSynth = &mad->Synth;
	mad_timer_t* pTimer = &mad->Timer;
	int curframe=0;
	int pcmlen=0;
	do 
	{
		Status = refilldata(mad->inputbuffer,mad,&mad->Stream);
		if(Status != 0)
		{
			return 0;//end of data
		}
		while(1)
		{
			if(mad_header_decode(&pFrame->header,pStream)==-1)
			{
				if (!MAD_RECOVERABLE(pStream->error))
				{
					break;
				}
				switch (decode_error(pStream, pFrame)) 
				{
				case MAD_FLOW_IGNORE:
				case MAD_FLOW_CONTINUE:
				default:
					continue;
				}
			}
			/* Decode the next MPEG frame. The streams is read from the
			* buffer, its constituents are break down and stored the the
			* Frame structure, ready for examination/alteration or PCM
			* synthesis. Decoding options are carried in the Frame
			* structure from the Stream structure.
			*
			* Error handling: mad_frame_decode() returns a non zero value
			* when an error occurs. The error condition can be checked in
			* the error member of the Stream structure. A mad error is
			* recoverable or fatal, the error status is checked with the
			* MAD_RECOVERABLE macro.
			*
			* {4} When a fatal error is encountered all decoding
			* activities shall be stopped, except when a MAD_ERROR_BUFLEN
			* is signaled. This condition means that the
			* mad_frame_decode() function needs more input to complete
			* its work. One shall refill the buffer and repeat the
			* mad_frame_decode() call. Some bytes may be left unused at
			* the end of the buffer if those bytes forms an incomplete
			* frame. Before refilling, the remaining bytes must be moved
			* to the beginning of the buffer and used for input for the
			* next mad_frame_decode() invocation. (See the comments
			* marked {2} earlier for more details.)
			*
			* Recoverable errors are caused by malformed bit-streams, in
			* this case one can call again mad_frame_decode() in order to
			* skip the faulty part and re-sync to the next frame.
			*/
			if(mad_frame_decode(pFrame,pStream))
			{
				if(MAD_RECOVERABLE(pStream->error))
				{
					/* Do not print a message if the error is a loss of
					* synchronization and this loss is due to the end of
					* stream guard bytes. (See the comments marked {3}
					* supra for more informations about guard bytes.)
					*/
					if(pStream->error!=MAD_ERROR_LOSTSYNC)
					{
						//printf("recoverable frame level error (%s)\n",MadErrorString(pStream));
					}
					continue;
				}
				else
					if(pStream->error==MAD_ERROR_BUFLEN)
						continue;
					else
					{
						printf("unrecoverable frame level error (%s).\n",MadErrorString(pStream));
						Status=1;
						break;
					}
			}

			/* Accounting. The computed frame duration is in the frame
			* header structure. It is expressed as a fixed point number
			* whole data type is mad_timer_t. It is different from the
			* samples fixed point format and unlike it, it can't directly
			* be added or subtracted. The timer module provides several
			* functions to operate on such numbers. Be careful there, as
			* some functions of libmad's timer module receive some of
			* their mad_timer_t arguments by value!
			*/
			mad_timer_add(pTimer,pFrame->header.duration);
			
			/* Once decoded the frame is synthesized to PCM samples. No errors
			* are reported by mad_synth_frame();
			*/
			mad_synth_frame(pSynth,pFrame);

			/* Synthesized samples must be converted from libmad's fixed
			* point number to the consumer format. Here we use unsigned
			* 16 bit big endian integers on two channels. Integer samples
			* are temporarily stored in a buffer that is flushed when
			* full.
			*/
			if(MAD_NCHANNELS(&pFrame->header)==2)
			{
				S16 *outBuf = (S16*)OutputPtr;
				for(i=0;i<pSynth->pcm.length;i++)
				{
					signed short	Sample;
					/* Left channel */
					//Sample=MadFixedToSshort(pSynth->pcm.samples[0][i]);
					MadFixedToSshort(pSynth->pcm.samples[0][i],Sample);
					*outBuf = Sample;outBuf++;

					/* Right channel. If the decoded stream is monophonic then
					* the right output channel is the same as the left one.
					*/
					MadFixedToSshort(pSynth->pcm.samples[1][i],Sample);
					*outBuf = Sample;outBuf++;
				}
			}
			else
			{
				S16 *outBuf = (S16*)OutputPtr;
				for(i=0;i<pSynth->pcm.length;i++)
				{
					signed short	Sample;
					MadFixedToSshort(pSynth->pcm.samples[0][i],Sample);
					*outBuf = Sample;outBuf++;
				}
			}
			
			mad->framecount++;
			if(MAD_NCHANNELS(&pFrame->header)==2)
				pcmlen +=i*4;//byte 
			else
				pcmlen +=i*2;
			curframe++;
			if(curframe==maxframe)
				return pcmlen;
		}
		
	} while (1);
	return 0;
}

#define MAX_FRAME_CNT 1
#if MAX_FRAME_CNT*1152*2*2 > WAVE_RELOAD_BUF_MAX
#error "WAVE_RELOAD_BUF_MAX is too small to MP3 Play "
#endif
static void mp3txisr(int dmach, WAVE* pWave)
{
	static const INTERRUPT_TYPE dma_intnum[]={INTNUM_DMA0,INTNUM_DMA1,INTNUM_DMA2,INTNUM_DMA3};
	MAD_STRUCT* mad =(MAD_STRUCT*)pWave->user;
	if(pWave->bNeedReload)
	{
		if( (pWave->bufsize>0) &&(pWave->bufsize2>0))
		{
			if(pWave->cursamplesnum) //samples2 was played
			{
				pWave->transfered_len += pWave->bufsize2;
				snd_dmatransfer(dmach,pWave->channelNumber,pWave->samples,pWave->bufsize);
				pWave->validsamples=false;
				pWave->cursamplesnum=0;
				pWave->validsamples2=false;
				pWave->bufsize2 = decode_one_frame(mad,pWave->samples2,MAX_FRAME_CNT);
				if(pWave->bufsize2 >0)
				{
					if(pWave->bufsize2<WAVE_RELOAD_BUF_MAX)//zero padding for avoid noise
						memset(pWave->samples2+pWave->bufsize2,0,WAVE_RELOAD_BUF_MAX-pWave->bufsize2> 0x20 ? 0x20:WAVE_RELOAD_BUF_MAX-pWave->bufsize2 );
					pWave->validsamples2=true;
				}
				else
				{
					if(f_eof(mad->fp))
					{
						if(pWave->bLoop)
						{
							if(mad->mp3databuf)
								mad->mp3databuf_readindex = pWave->headersize;
							else
								f_lseek(&pWave->file,pWave->headersize);
// 							mad_stream_finish(&mad->Stream);
// 							mad_frame_finish(&mad->Frame);
// 							mad_synth_finish(&mad->Synth);
// 
// 							mad_stream_init(&mad->Stream);
// 							mad_frame_init(&mad->Frame);
// 							mad_synth_init(&mad->Synth);
							
							mad_timer_reset(&mad->Timer);
							mad->framecount = 0; 
							pWave->bufsize2 = decode_one_frame(mad, pWave->samples2, MAX_FRAME_CNT);
							if(pWave->bufsize2 >0)
							{
								if(pWave->bufsize2<WAVE_RELOAD_BUF_MAX)//zero padding for avoid noise
									memset(pWave->samples2+pWave->bufsize2,0,WAVE_RELOAD_BUF_MAX-pWave->bufsize2> 0x20 ? 0x20:WAVE_RELOAD_BUF_MAX-pWave->bufsize2 );
								pWave->validsamples2=true;
							}
						}
					}
				}
			}
			else
			{
				pWave->transfered_len += pWave->bufsize;
				snd_dmatransfer(dmach,pWave->channelNumber,pWave->samples2,pWave->bufsize2);
				pWave->validsamples2=0;
				pWave->cursamplesnum=1;
				pWave->validsamples=false;
				pWave->bufsize = decode_one_frame(mad,pWave->samples,MAX_FRAME_CNT);
				if(pWave->bufsize >0)
				{
					if(pWave->bufsize<WAVE_RELOAD_BUF_MAX)//zero padding for avoid noise
						memset(pWave->samples+pWave->bufsize,0,WAVE_RELOAD_BUF_MAX-pWave->bufsize> 0x20 ? 0x20:WAVE_RELOAD_BUF_MAX-pWave->bufsize );
					pWave->validsamples=true;
				}
				else
				{
					if(f_eof(mad->fp))
					{
						if(pWave->bLoop)
						{
							if(mad->mp3databuf)
								mad->mp3databuf_readindex = pWave->headersize;
							else
								f_lseek(&pWave->file, pWave->headersize);
							mad_timer_reset(&mad->Timer);
							mad->framecount=0;
							pWave->bufsize = decode_one_frame(mad,pWave->samples,MAX_FRAME_CNT);
							if(pWave->bufsize >0)
							{
								if(pWave->bufsize<WAVE_RELOAD_BUF_MAX)//zero padding for avoid noise
									memset(pWave->samples+pWave->bufsize,0,WAVE_RELOAD_BUF_MAX-pWave->bufsize> 0x20 ? 0x20:WAVE_RELOAD_BUF_MAX-pWave->bufsize );
								pWave->validsamples=true;
							}
						}
					}
				}
			}
		}
		else
		{
			if(pWave->validsamples || pWave->validsamples2)
			{
				if(pWave->cursamplesnum) //samples2 was played
				{
					if(pWave->validsamples)
					{
						snd_dmatransfer(dmach,pWave->channelNumber,pWave->samples,pWave->bufsize);
						pWave->validsamples=false;
						pWave->cursamplesnum=0;
					}
				}
				else //samples was played
				{
					if(pWave->validsamples2)
					{
						snd_dmatransfer(dmach,pWave->channelNumber,pWave->samples2,pWave->bufsize2);
						pWave->validsamples2=false;
						pWave->cursamplesnum=1;
					}
				}
			}
			else
			{
				sound_stop(pWave);
				fs_enable_support_interrupt(dma_intnum[dmach],FALSE);
				/*
				if(mad->mp3databuf == 0)
					DEBUGPRINTF("%s was stopped\r\n",pWave->filename);
					*/
			}
		}

	}
	else
	{
		if(pWave->bLoop)
		{
			//debugstring("loop\r\n");
			snd_dmatransfer(dmach,pWave->channelNumber,pWave->samples,pWave->bufsize);
		}
		else
		{
			sound_stop(pWave);
			fs_enable_support_interrupt(dma_intnum[dmach],FALSE);
			/*
			if(mad->mp3databuf == 0)
				DEBUGPRINTF("%s was stopped\r\n",pWave->filename);
			*/
		}
	}	
}
/*
called by ReleaseSound()
*/
static void ReleaseMP3User(void* user)
{
	MAD_STRUCT* mad;
	mad = (MAD_STRUCT* )user;
	if(mad)
	{
		mad_synth_finish(&mad->Synth);
		mad_frame_finish(&mad->Frame);
		mad_stream_finish(&mad->Stream);
		free(mad->inputbuffer);
		free(mad);
	}
}

static bool preparetorplay_mp3(struct _tagWAVE* pWave)
{
	MAD_STRUCT* mad;
	mad = (MAD_STRUCT* )pWave->user;

 	mad_stream_init(&mad->Stream);
 	mad_frame_init(&mad->Frame);
 	mad_synth_init(&mad->Synth);
 	mad_timer_reset(&mad->Timer);

	if(mad->mp3databuf)
		mad->mp3databuf_readindex = pWave->headersize + pWave->transfered_len;
	else
		f_lseek(&pWave->file, pWave->headersize + pWave->transfered_len);
	if (pWave->transfered_len)
	{
		mad_timer_set(&mad->Timer, 0, 1, pWave->byterate);
		mad_timer_multiply(&mad->Timer, pWave->transfered_len);
	}

	pWave->bufsize = decode_one_frame(mad,pWave->samples,1);
	pWave->bufsize2 = decode_one_frame(mad,pWave->samples2,1);

	pWave->cursamplesnum=0;
	pWave->validsamples=1;
	pWave->validsamples2=1;
	return true;
}
static U32 current_time_mp3(WAVE* pWave)
{
	MAD_STRUCT* mad;
	mad = (MAD_STRUCT*)pWave->user;
	if (mad == NULL)
		return 0;
	U32 ms = (U32)mad_timer_count(mad->Timer, MAD_UNITS_MILLISECONDS);
	return ms;
}
static WAVE* sound_loadmp3_r(char* filename,void* mp3databuf,U32 mp3databufsize, bool bfile)
{
	FIL file;
	FRESULT res;
	mp3idtagsize = 0;
	if(bfile)
	{
		res = f_open(&file,filename,FA_OPEN_EXISTING | FA_READ);
		if(res != FR_OK)
		{
			debugerror(SDKERROR_FILEOPEN,filename);
			return 0 ;
		}
	}
	WAVE* pWave = (WAVE*)malloc(sizeof(WAVE));
	memset(pWave,0,sizeof(WAVE));
	pWave->samples = (unsigned char*)malloc(WAVE_RELOAD_BUF_MAX);
	if(pWave->samples==0)
	{
		free(pWave);
		return 0;
	}
	pWave->samples2 = (unsigned char*)malloc(WAVE_RELOAD_BUF_MAX);
	if(pWave->samples2==0)
	{
		free(pWave->samples);
		free(pWave);
		return 0;
	}
	pWave->bMallocedBuf = true;

	MAD_STRUCT* mad;
	mad =(MAD_STRUCT*)malloc(sizeof(MAD_STRUCT));
	mad_stream_init(&mad->Stream);
	mad_frame_init(&mad->Frame);
	mad_synth_init(&mad->Synth);
	mad_timer_reset(&mad->Timer);
	mad->inputbuffer = (unsigned char*)malloc(INPUT_BUFFER_SIZE+MAD_BUFFER_GUARD);
	mad->framecount=0;
	if(bfile)
	{
		pWave->file = file;
		pWave->datasize = f_size(&file);
		mad->mp3databuf = 0;
	}
	else
	{
		pWave->file.fs = 0;
		mad->mp3databuf = mp3databuf;
		mad->mp3databufsize = mp3databufsize;
		mad->mp3databuf_readindex = 0;
		pWave->datasize = mp3databufsize;
	}
	
	mad->fp = &pWave->file;
	pWave->user = (void*)mad;
	pWave->TxIsr = (void (*)(int,WAVE*))mp3txisr;
	pWave->release = ReleaseMP3User;
	strcpy(pWave->filename,filename);

	pWave->bufsize = decode_one_frame(mad,pWave->samples,1);
	if(pWave->bufsize==0)
	{
		free(mad);
		free(pWave->samples);
		free(pWave->samples2);
		free(pWave);
		return 0;
	}
	pWave->byterate = mad->Frame.header.bitrate / 8;
	
	mad_synth_finish(&mad->Synth);
	mad_frame_finish(&mad->Frame);
	mad_stream_finish(&mad->Stream);

	pWave->headersize = mp3idtagsize;
	mad->framecount=0;
	if(bfile)
		f_lseek(&file,pWave->headersize);
	else
		mad->mp3databuf_readindex = pWave->headersize;
	pWave->bNeedReload=true;
	//PrintFrameInfo(&mad->Frame.header);
	pWave->volume = DEFAULT_VOLUME; // maximum 255
	pWave->panpot = 8; 
	pWave->channelNumber = -1;
	pWave->freq =  mad->Frame.header.samplerate;
	pWave->PlaybackPitch = ((pWave->freq*256)/SND_OUTPUT_HZ) -1; 
	if((mad->Frame.header.mode == MAD_MODE_JOINT_STEREO) || (mad->Frame.header.mode == MAD_MODE_STEREO))
		pWave->pcmtype = PCMTYPE_SS16;
	else
		pWave->pcmtype = PCMTYPE_SM16;
	PrintFrameInfo(&mad->Frame.header);
	//DEBUGPRINTF("%s : %dHz, %s\r\n",filename,pWave->freq,pWave->pcmtype == PCMTYPE_SS16 ?"Stereo":"Mono");
	
	mad_timer_reset(&mad->Timer); 
	mad_timer_set(&mad->Timer, 0, 1, pWave->byterate);
	mad_timer_multiply(&mad->Timer, pWave->datasize - mp3idtagsize);
	U32 ms = mad_timer_count(mad->Timer, MAD_UNITS_MILLISECONDS);
	//DEBUGPRINTF("Play Time : %d:%d\r\n",ms/1000,ms%1000); 
	pWave->duration_ms = ms;
	pWave->current_time = current_time_mp3;
	pWave->preparetoplay = preparetorplay_mp3;
	return pWave;
}

WAVE* sound_loadmp3(char* filename)
{
	return sound_loadmp3_r(filename,0,0,1);
}

WAVE* sound_loadmp3p(void* buf,U32 mp3databufsize)
{
	return sound_loadmp3_r(0,buf,mp3databufsize,0);
}

