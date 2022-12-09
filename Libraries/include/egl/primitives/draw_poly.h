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


void r_draw_polyline(EGL_POINT* p, int n,EGL_COLOR c);
void r_draw_polyline_aa(EGL_POINT* p, int n,EGL_COLOR c);
void r_draw_polygon(EGL_POINT* ptable,int cnt,EGL_COLOR c);
void r_draw_polygon_aa(EGL_POINT* ptable,int cnt,EGL_COLOR c);
void r_draw_polygonfill(EGL_POINT* ptable,int cnt,EGL_COLOR c);

void draw_polyline(EGL_POINT* p, int n,EGL_COLOR c);
void draw_polyline_aa(EGL_POINT* p, int n,EGL_COLOR c);
void draw_polygon(EGL_POINT* ptable,int cnt,EGL_COLOR c);
void draw_polygon_aa(EGL_POINT* ptable,int cnt,EGL_COLOR c);
void draw_polygonfill(EGL_POINT* ptable,int cnt,EGL_COLOR c);

void sw_draw_polyline(SURFACE* dst, EGL_POINT* p, int n,EGL_COLOR c);
void sw_draw_polyline_aa(SURFACE* dst,EGL_POINT* p, int n,EGL_COLOR c);
void sw_draw_polygon(SURFACE* dst, EGL_POINT* ptable,int n,EGL_COLOR c);
void sw_draw_polygon_aa(SURFACE* dst, EGL_POINT* ptable,int cnt,EGL_COLOR c);
void sw_draw_polygonfill(SURFACE* dst,EGL_POINT* pointtable,int count,EGL_COLOR c);


#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
}
#endif
