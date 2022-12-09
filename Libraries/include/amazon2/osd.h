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


#define OSD_4BPP 6
#define OSD_4BPP_A 7
#define OSD_8BPP 8
#define OSD_8BPP_A 9
#define OSD_565 10
#define OSD_A565 11
#define OSD_888 12
#define OSD_A1888 13
#define OSD_A4888 14
#define OSD_A8888 15

#define OSD_MOVE(x,y) do{*R_OSDTARGETX = x;*R_OSDTARGETY = y;}while(0);
#define OSD_DISABLE()	do{*R_OSDCNTL = 0;}while(0);

BOOL osd_set_surface(SURFACE* surf,int x, int y);
BOOL osd_set_surface_with_alpha(SURFACE* surf,int x, int y,U8 alpha);
void osd_set_pal(EGL_COLOR* pal,int num);
void  osd_set_alpha(U8 alpha);
void osd_off();
