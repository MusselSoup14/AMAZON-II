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
extern "C" {
#endif
#include "egl_typedef.h"
#include "egl_base.h"
#include "font/egl_font.h"
#include "font/textout.h"
#include "egl_window.h"
#include "egl_theme.h"
#include "surface.h"
#include "egl_port.h"

#include "font/bitfont.h"
#include "font/bitmapfont.h"

#include "image/loadbmp.h"
#include "image/loadjpg.h"
#include "image/loadpng.h"
#include "image/loadtga.h"
#include "image/savebmp.h"
#include "image/savejpg.h"
#include "image/loadsurf.h"
#include "image/movieplay.h"

#include "primitives/draw_arc.h"
#include "primitives/draw_rect.h"
#include "primitives/draw_line.h"
#include "primitives/draw_thickline.h"
#include "primitives/draw_poly.h"
#include "primitives/draw_roundrect.h"
#include "primitives/draw_legacy.h"

#include "object/egl_animation.h"
#include "object/egl_label.h"
#include "object/egl_button.h"
#include "object/egl_image_button.h"
#include "object/egl_circle_gauge.h"
#include "object/egl_bar_gauge.h"
#include "object/egl_custom_object.h"
#include "object/egl_messagebox.h"
#include "object/egl_progressbar.h"
#include "object/egl_scrollbar.h"
#include "object/egl_listbox.h"
#include "object/egl_slider.h"
#include "object/egl_checkbutton.h"
#include "object/egl_picture.h"
#include "object/egl_toggle_image.h"

#include <stdio.h>
#include <stdlib.h>

#define EGL_VERSION 0x01001


#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
}
#endif
