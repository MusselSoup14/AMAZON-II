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

#include "egl_base.h"
#include "egl_typedef.h"

#include "surface.h"
#define DRAW_SURFACE		draw_surface
#define DRAW_SURFACE_RECT	draw_surface_rect
#define DRAW_SURFACE_SCALE_RECT	draw_surface_scalerect
#define DRAW_SURFACE_SCALE	draw_surface_scale

#define EGL_GET_TARGET_SURFACE		get_draw_target
#define EGL_SET_TARGET_SURFACE		set_draw_target
#define CREATE_SURFACE		create_surface
#define RELEASE_SURFACE		release_surface

#include "primitives/draw_rect.h"
#include "primitives/draw_line.h"
#include "primitives/draw_poly.h"
#include "primitives/draw_arc.h"
#define DRAW_RECT			draw_rect
#define DRAW_RECTFILL		draw_rectfill
#define DRAW_RECTFILLGRADIENT	draw_rectfill_gradient
#define DRAW_VLINE			draw_vline
#define DRAW_HLINE			draw_hline
#define DRAW_LINE			draw_line
#define DRAW_THICKLINE		draw_thickline

#define DRAW_CIRCLE			draw_circle
#define DRAW_CIRCLEFILL		draw_circlefill

#define DRAW_POLYGON		draw_polygon
#define DRAW_POLYGONFILL	draw_polygonfill

#define DRAW_ELLIPSE		draw_ellipse
#define DRAW_ELLIPSE_fill	draw_ellipsefill
#define DRAW_ELLIPSE_AA		draw_ellipse_aa

#define EGL_FLIP()			flip()

#include "amazon2/displayctrl.h"
#define EGL_SCREEN_WIDTH	get_screen_width()
#define EGL_SCREEN_HEIGHT	get_screen_height()
#define EGL_SCREEN_BPP		get_screen_bpp()

#include "amazon2/interrupt.h"
#ifndef CRITICAL_ENTER	
#error "Need CRITICAL_ENTER()"
#endif

#ifndef CRITICAL_EXIT
#error "Need CRITICAL_EXIT()"
#endif

#define INLINE inline

#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
}
#endif
