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
#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
extern "C"{
#endif

void r_draw_hline(int x, int y, int x2,EGL_COLOR c);
void r_draw_vline(int x, int y, int y2,EGL_COLOR c);
void r_draw_line(int x, int y, int x2, int y2,EGL_COLOR c);
void r_draw_line_aa(int x, int y, int x2, int y2,EGL_COLOR c);
void r_sw_draw_line_aa(SURFACE* dst,int x, int y, int x2, int y2,EGL_COLOR c);


//call r_xxx and data cache invalidation
void draw_hline(int x, int y, int x2,EGL_COLOR c);
void draw_vline(int x, int y, int y2,EGL_COLOR c);
void draw_line(int x, int y, int x2, int y2,EGL_COLOR c);
void draw_line_aa(int x, int y, int x2, int y2,EGL_COLOR c);

void sw_draw_hline(SURFACE* dst, int x, int y, int x2,EGL_COLOR c);
void sw_draw_vline(SURFACE* dst, int x, int y, int y2,EGL_COLOR c);
void sw_draw_line(SURFACE* dst,int x1, int y1, int x2, int y2,EGL_COLOR color);
void sw_draw_line_aa(SURFACE* dst,int x, int y, int x2, int y2,EGL_COLOR c);

/*
anti-aliased line
*/


#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
}
#endif
