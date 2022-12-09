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

#define GPIO_BASE	0xf0001c00

#define R_GPDIR(ch) ((volatile unsigned int*)(GPIO_BASE+(0x40*(ch))))

#define R_GPODIR(ch) ((volatile unsigned int*)(GPIO_BASE+(0x40*(ch))))

// Port Direction Input Mode Setting Registers
#define R_GPIDIR(ch) ((volatile unsigned int*)(GPIO_BASE+4+(0x40*(ch))))

//Port Output Data Level Registers
#define R_GPOLEV(ch) ((volatile unsigned int*)(GPIO_BASE+8+(0x40*(ch))))

// Port Output Data Registers
#define R_GPDOUT(ch) ((volatile unsigned char*)(GPIO_BASE+8+(0x40*(ch))))
 
//Port Output Data High Level Setting Registers
#define R_GPOHIGH(ch) ((volatile unsigned int*)(GPIO_BASE+8+(0x40*(ch))))

//Port Output Data Low Level Setting Registers
#define R_GPOLOW(ch) ((volatile unsigned int*)(GPIO_BASE+0xc+(0x40*(ch))))

//Port Input Data Level Registers
#define R_GPILEV(ch) ((volatile unsigned int*)(GPIO_BASE+0x10+(0x40*(ch))))

//Port Pull-up Status Registers
#define R_GPPUS(ch) ((volatile unsigned int*)(GPIO_BASE+0x18+(0x40*(ch))))

//Port Pull-up Enable Registers
#define R_GPPUEN(ch) ((volatile unsigned int*)(GPIO_BASE+0x18+(0x40*(ch))))

//Port Pull-up Disable Registers
#define R_GPPUDIS(ch) ((volatile unsigned int*)(GPIO_BASE+0x1c+(0x40*(ch))))

//Port Rising-Edge Detect Registers
#define R_GPRED(ch) ((volatile unsigned int*)(GPIO_BASE+0x20+(0x40*(ch))))

//Port Falling-Edge Detect Registers
#define R_GPFED(ch) ((volatile unsigned int*)(GPIO_BASE+0x24+(0x40*(ch))))

//Port Edge Status Registers
#define R_GPEDS(ch) ((volatile unsigned int*)(GPIO_BASE+0x28+(0x40*(ch))))

//======================================================================================================================
// Port Alternate Function Register 0 (PAF0)
#define R_PAF(N) ((volatile unsigned int*)(0xF4000400+(N*4)))

