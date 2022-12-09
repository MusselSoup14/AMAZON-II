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


static int flash_sector_size = 4 * 1024;
static int flash_size = 512 * 1024;
void flash_erase_sector( int sector, int len )
{
    int i;
    U32 mode = *R_SFMOD;
    *R_SFMOD &= ~( 1 << 7 ); //write enable
    U32 addr = sector * flash_sector_size;
    for( i = 0; i < len; i++ )
    {
        *R_SFSEA = addr;
        addr += flash_sector_size;
    }
    *R_SFMOD = mode;
}

void flash_write( U32 addr, BYTE* buf, int len )
{
    U32 mode = *R_SFMOD;
    *R_SFMOD &= ~( 1 << 7 ); //write enable
    memcpy( ( void* )addr, ( void* )buf, len );
    *R_SFMOD = mode;
}

void flash_read( U32 addr, BYTE* buf, int len )
{
    memcpy( ( void* )buf, ( void* )addr, len );
}

int flash_get_sectorsize()
{
    return flash_sector_size;
}

int flash_get_size()
{
    return flash_size;
}
