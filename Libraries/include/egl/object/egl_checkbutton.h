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

    typedef enum
    {
        CHECK_CHECKED = 0,
        CHECK_UNCHECKED,
        CHECK_MAX,
    }
    CHECK_EVENT;
    typedef enum
    {
        CHECK_STYLE_CHECKBUTTON = 0,
        CHECK_STYLE_RADIOBUTTON
    } CHECK_STYLE;

    typedef struct _tag_check_data
    {
        EGL_HANDLE label;
        int m_style;
        int m_check_x;
        int m_check_y;
        BOOL m_bChecked;
        SURFACE* surf_check;
    } CHECK_DATA;

    EGL_HANDLE egl_create_checkbutton( int x, int y, int w, int h, const char* text, CHECK_STYLE style );
    BOOL egl_checkbutton_callback( EGL_HANDLE hObj, EVENT_CALLBACK cb );

    void egl_checkbutton_set_check( EGL_HANDLE hObj, BOOL b );
    BOOL egl_checkbutton_get_check( EGL_HANDLE hObj );
    void egl_checkbutton_set_style( EGL_HANDLE hObj, int style );
	BOOL egl_release_checkbutton( EGL_HANDLE hObj );
	
#define CHECK_RADIO_SIZE 32
#pragma once
#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
}
#endif
