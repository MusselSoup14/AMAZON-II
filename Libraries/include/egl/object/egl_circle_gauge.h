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

    typedef struct _tagEGL_CIRCLE_GAUGE
    {
        int needle_x;//center of needle, offset of back-ground image
        int needle_y;//center of needle, offset of back-ground image
        int needle_length;
        int needle_thick;
        int min_value;
        int max_value;
        //clockwise direction
        int min_angle;//0~358
        int max_angle;//1~359
        int cur_value;
        int cur_angle;
		SURFACE* bg;
    } EGL_CIRCLE_GAUGE_DATA;

    typedef struct _tagCIRCLE_GAUGE_INFO
    {
        int needle_x;//center of needle, offset of back-ground image
        int needle_y;//center of needle, offset of back-ground image
        int needle_length;
        int needle_thick;
        int min_value;
        int max_value;
        //clockwise direction
        int min_angle;//0~358
        int max_angle;//1~359
    } EGL_CIRCLE_GAUGE_INFO;

    EGL_HANDLE egl_create_circle_gauge( SURFACE* bg, int x, int y, EGL_CIRCLE_GAUGE_INFO* pInfo );
    BOOL egl_circle_gauge_set_value( EGL_HANDLE h, int value );
    int egl_circle_gauge_get_value( EGL_HANDLE h );
	BOOL egl_release_circle_gauge( EGL_HANDLE hObj );

#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
}
#endif
