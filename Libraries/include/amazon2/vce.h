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
#define ICE_BASE            0xF4002000
#define R_VCE_CTRL		((volatile unsigned int*)(ICE_BASE + 0x00))
#define R_VCE_STATS		((volatile unsigned int*)(ICE_BASE + 0x04))
#define R_VCE_YBUF(N)	((volatile unsigned int*)(ICE_BASE + 0x8 + (N*0xc)))
#define R_VCE_CBBUF(N)	((volatile unsigned int*)(ICE_BASE + 0xc + (N*0xc)))
#define R_VCE_CRBUF(N)	((volatile unsigned int*)(ICE_BASE + 0x10 + (N*0xc)))
#define R_VCE_OFFSET	((volatile unsigned int*)(ICE_BASE + 0x38))
#define R_VCE_NUMLINECUT0	((volatile unsigned int*)(ICE_BASE + 0x3C))
#define R_VCE_NUMLINECUT1 	((volatile unsigned int*)(ICE_BASE + 0x40))
#define R_VCE_NUMPXLCUT0 	((volatile unsigned int*)(ICE_BASE + 0x44))
#define R_VCE_NUMPXLCUT1 	((volatile unsigned int*)(ICE_BASE + 0x48))
#define R_VCE_WIDTH 	((volatile unsigned int*)(ICE_BASE + 0x4C))

BOOL vce_on_surface(SURFACE* dst,int x, int y, bool halfmode);
void vce_off();

