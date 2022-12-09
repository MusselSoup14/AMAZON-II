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

#define R_SFMOD	((volatile unsigned int*)0xF0000000)
#define R_SFBRT	((volatile unsigned int*)0xF0000004)
#define R_SFCSH	((volatile unsigned int*)0xF0000008)
#define R_SFPEM	((volatile unsigned int*)0xF000000c)
#define R_SFCMD	((volatile unsigned char*)0xF0000010)
#define R_SFSTS	((volatile unsigned char*)0xF0000014)
#define R_SFSEA	((volatile unsigned int*)0xF0000018)
#define R_SFBEA	((volatile unsigned int*)0xF000001c)
#define R_SFDAT	((volatile unsigned int*)0xF0000020)
#define R_SFWCP	((volatile unsigned int*)0xF0000024)
#define R_SFCKLDY	((volatile unsigned int*)0xF0000028)
#define R_SFSTS2	((volatile unsigned char*)0xF000002c) //winbond only
#define R_SFSTSW	((volatile unsigned short*)0xF000002c)//winbond only

#define SF_CH_LOW do {*R_SFCMD &= ~(1<<8);} while (0);
#define SF_CH_HIGH do {*R_SFCMD |= (1<<8);} while (0);

void flash_erase_sector(int sector,int len);
void flash_write(U32 addr, BYTE* buf, int len);
void flash_read(U32 addr, BYTE* buf, int len);
int flash_get_sectorsize();
int flash_get_size();
