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
        PICTURE_CLICKED = 0,
        PICTURE_MAX,
    } PICTUREEVENT;

    EGL_HANDLE egl_create_picture( SURFACE* surf, int x, int y, int w, int h );
    BOOL egl_picture_callback( EGL_HANDLE hObj, EVENT_CALLBACK cb );
    SURFACE* egl_picture_set( EGL_HANDLE hObj, SURFACE* surf ); //return old surface
	BOOL egl_release_picture( EGL_HANDLE hObj );

#if	defined(__cplusplus)||defined(__GNUG__)// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
}
#endif
