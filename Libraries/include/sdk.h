/*****************************************************************************
* 
* Copyright (C) 2011      Advanced Digital Chips, Inc. All Rights Reserved. 
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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>

#define ISPM	__attribute__((section(".ispm"),noinline))
#define DSPM	__attribute__((section(".dspm")))

#define GCC_VERSION (__GNUC__ * 10000 \
                               + __GNUC_MINOR__ * 100 \
                               + __GNUC_PATCHLEVEL__)

#ifdef __GNUG__ // The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
extern "C"{
#endif
#include "lib_config.h"
#include "typedef.h"

#include "fatfs/ff.h"
#include "fatfs/fatutil.h"
#include "egl/egl.h"
#include "amazon2/hw_graphics.h"
#include "egl/image/loadjpg_hw.h"

#include "amazon2/util.h"
#include "amazon2/cacheutil.h"
#include "amazon2/interrupt.h"	
#include "amazon2/pmu.h"	
#include "amazon2/uart.h"	
#include "amazon2/usb.h"	
#include "amazon2/gpio.h"	
#include "amazon2/serialflash.h"
#include "amazon2/usb.h"
#include "amazon2/nandctrl.h"
#include "amazon2/sdcard.h"
#include "amazon2/timer.h"
#include "amazon2/displayctrl.h"
#include "amazon2/dma.h"
#include "amazon2/osd.h"
#include "amazon2/vce.h"
#include "amazon2/error.h"
#include "amazon2/soundmixer.h"
#include "amazon2/twi.h"
#include "amazon2/spi.h"
#include <usb_host/amazon2_usb_host.h>
#include <usb_host/usb_host.h>


	
#ifdef __GNUG__
}
#endif
