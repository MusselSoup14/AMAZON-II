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

//======================================================================================================================
// Watchdog Timer 
#define R_WDTCTRL ((volatile unsigned int*)0xf4003400)
#define R_WDTCNT ((volatile unsigned int*)0xf4003404)
//======================================================================================================================

#define MAX_TIMER_CHANNEL 3
#define TIMER_BASE_ADDR	0xF4003800
//======================================================================================================================
// Timer Pre-scale Control Registers (TPxCTRL)

#define F_TPCTRL_CNTCLR			(1 << 1)		// Pre-scale Counter and Timer Counter Reset
#define F_TPCTRL_CLKSEL_CAP0	(0 << 0)		// Pre-scale Clock Selection - CAP0
#define F_TPCTRL_CLKSEL_SCLK	(1 << 0)		// Pre-scale Clock Selection - System Clock
//======================================================================================================================

//======================================================================================================================
// Timer Control Registers (TMxCTRL)

#define F_TMCTRL_TMOD_TIMER		(0 <<14)		// Timer/Counter Mode - TIMER
#define F_TMCTRL_TMOD_PWM		(1 <<14)		// Timer/Counter Mode - PWM
#define F_TMCTRL_TMOD_CAP		(2 <<14)		// Timer/Counter Mode - Capture
#define F_TMCTRL_OVST			(1 <<12)		// Capture Overflow Status bit
#define F_TMCTRL_CAPMOD_LH		(0 << 8)		// Capture Mode Selection - Low/High Pulse
#define F_TMCTRL_CAPMOD_LP		(2 << 8)		// Capture Mode Selection - Low Pulse
#define F_TMCTRL_CAPMOD_HP		(3 << 8)		// Capture Mode Selection - High Pulse
#define F_TMCTRL_CAPMOD_FF		(4 << 8)		// Capture Mode Selection - Falling to Falling Period
#define F_TMCTRL_CAPMOD_RR		(6 << 8)		// Capture Mode Selection - Rising to Rising Period
#define F_TMCTRL_PWMO			(1 << 6)		// PWM Output One Period Generation - Enable
#define F_TMCTRL_PWML_HIGH		(1 << 5)		// PWM Output Start Level - High
#define F_TMCTRL_TMOUT			(1 << 4)		// Timer Wave Output Generation - Enable
#define F_TMCTRL_PFSEL_2		(0 << 1)		// Pre-scale Factor Selection - 1/2
#define F_TMCTRL_PFSEL_8		(1 << 1)		// Pre-scale Factor Selection - 1/8
#define F_TMCTRL_PFSEL_32		(2 << 1)		// Pre-scale Factor Selection - 1/32
#define F_TMCTRL_PFSEL_128		(3 << 1)		// Pre-scale Factor Selection - 1/128
#define F_TMCTRL_PFSEL_512		(4 << 1)		// Pre-scale Factor Selection - 1/512
#define F_TMCTRL_PFSEL_2048		(5 << 1)		// Pre-scale Factor Selection - 1/2048
#define F_TMCTRL_PFSEL_8192		(6 << 1)		// Pre-scale Factor Selection - 1/8192
#define F_TMCTRL_PFSEL_32768	(7 << 1)		// Pre-scale Factor Selection - 1/32768
#define F_TMCTRL_TMEN			(1 << 0)		// Timer/Counter or PWM Enable

#define R_TPCTRL(ch) ((volatile unsigned int*)(TIMER_BASE_ADDR+0x20*ch))
#define R_TMCTRL(ch) ((volatile unsigned int*)(TIMER_BASE_ADDR+4+0x20*ch))
#define R_TMCNT(ch) ((volatile unsigned int*)(TIMER_BASE_ADDR+8+0x20*ch))
#define R_TMDUT(ch) ((volatile unsigned int*)(TIMER_BASE_ADDR+0xc+0x20*ch))
#define R_TMPUL(ch) ((volatile unsigned int*)(TIMER_BASE_ADDR+0x10+0x20*ch))


BOOL set_timer(int nCh,U32 ms);
BOOL stop_timer(int nCh);
BOOL delayms(U32 ms); //return false, if ms is too big, check the timer.c
BOOL delayus(int us);
