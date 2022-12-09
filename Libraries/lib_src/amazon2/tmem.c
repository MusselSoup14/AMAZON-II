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
#define BEST_ALLOC // 적합한 메모리중 가장 작은 단위를 선택 한다.

static TMemManager root_tman = {2048 * 128, 0, 0, 0}; //2048*64 is for command queue
extern U32 g_TMEM_Size ;

bool tmemalign( TMemManager* tm, int align, int size )
{
    int invmask;
    int mask;
    int mindiff;
    int curdiff;
    int nextoffset;
    int curalignaddr;
#ifdef BEST_ALLOC
    int bestalignaddr=0;
#endif
    TMemManager* cur;
    TMemManager* best;
    TMemManager* next;

    invmask = align - 1;
    mask = 0x7fffffff - invmask;
    mindiff = g_TMEM_Size + 1;//max
    cur = &root_tman;
    best = 0;
    while( cur != NULL )
    {
        next = cur->next;
        if( next == NULL )
            nextoffset = g_TMEM_Size;
        else
            nextoffset = next->offset;
        curalignaddr = ( cur->offset + cur->size + invmask ) & mask;
        if( nextoffset >= ( curalignaddr + size ) )
        {
#ifdef BEST_ALLOC
            curdiff = nextoffset - ( cur->offset + cur->size + size );
            if( mindiff > curdiff )
            {
                best = cur;
                mindiff = curdiff;
                bestalignaddr = curalignaddr;
            }
#else
            best = cur;
            bestalignaddr = curalignaddr;
            break;
#endif
        }
        cur = next;
    }
    if( best == 0 )
        return false;
    tm->next = best->next;
    tm->prev = best;
    best->next = tm;
    if( tm->next )
        tm->next->prev = tm;

    tm->offset = bestalignaddr;
    tm->size = size;
    return true;
}

void tmemfree( TMemManager* tm )
{
    TMemManager* prev;
    TMemManager* next;
    prev = tm->prev;
    next = tm->next;
    if( prev )
    {
        if( next )
        {
            prev->next = next;
            next->prev = prev;
        }
        else
            prev->next = NULL;
    }
}

U32 tmem_getfree()
{
	int aloc_size = 0;
	TMemManager* cur = &root_tman;
	while (cur != NULL)
	{
		aloc_size += cur->size;
		cur = cur->next;
	}
	return (g_TMEM_Size - aloc_size - root_tman.size);
}
