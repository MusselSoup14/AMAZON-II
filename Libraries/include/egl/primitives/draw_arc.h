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

	void r_draw_arc2( int x, int y, int rx, int ry,int ax, int ay, int bx, int by, EGL_COLOR c );
	void r_draw_arc(int x, int y, int rx, int ry,int a1, int a2,EGL_COLOR c);
	void r_draw_pie(int x, int y, int rx, int ry,int a1, int a2,EGL_COLOR c);
	void r_draw_piefill(int x, int y, int rx, int ry,int a1, int a2,EGL_COLOR c);
	void r_draw_ellipse(int x, int y, int rx, int ry,EGL_COLOR c);
	void r_draw_ellipsefill(int x, int y, int rx, int ry,EGL_COLOR c);
	void r_draw_circle(int x, int y, int r,EGL_COLOR c);
	void r_draw_circlefill(int x, int y, int r,EGL_COLOR c);
	void r_draw_ellipse_aa(int x, int y, int rx, int ry, EGL_COLOR color);
	void r_draw_bezier(EGL_POINT* pts,int n, int s,EGL_COLOR c);

	void draw_arc2( int x, int y, int rx, int ry,int ax, int ay, int bx, int by, EGL_COLOR c );
	void draw_arc(int x, int y, int rx, int ry,int a1, int a2,EGL_COLOR c);
	void draw_pie(int x, int y, int rx, int ry,int a1, int a2,EGL_COLOR c);
	void draw_piefill(int x, int y, int rx, int ry,int a1, int a2,EGL_COLOR c);
	void draw_ellipse(int x, int y, int rx, int ry,EGL_COLOR c);
	void draw_ellipsefill(int x, int y, int rx, int ry,EGL_COLOR c);
	void draw_circle(int x, int y, int r,EGL_COLOR c);
	void draw_circlefill(int x, int y, int r,EGL_COLOR c);
	void draw_ellipse_aa(int x, int y, int rx, int ry, EGL_COLOR color);
	void draw_bezier(EGL_POINT* pts,int n, int s,EGL_COLOR c);

#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
}
#endif
