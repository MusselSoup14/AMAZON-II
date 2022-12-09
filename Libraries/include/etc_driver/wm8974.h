/******************************************************************************
	Copyright (C) 2014	Advanced Digital Chips Inc. 
						http://www.adc.co.kr
						
	Author : Software Team.
******************************************************************************/

#pragma once

//==============================================================================
// Software Reset

#define R_WM8974_RESET					0x0

#define F_WM8974_RESET					( (x&0x1FF)<< 0)

//==============================================================================
// Power manage't 1

#define R_WM8974_PM1					0x1

// Dedicated buffer for DC level shifting output stages when in 1.5x gain boost configuration
// 0 = Buffer disabled
// 1 = Buffer enabled (required for 1.5x gain boost)
#define F_WM8974_PM1_BUFDCOPEN(x)		( (x&0x1)<< 8)

// Auxilliary input buffer enable
// 0 = OFF
// 1 = ON
#define F_WM8974_PM1_AUXEN(x)			( (x&0x1)<< 6)

// PLL enable
// 0 = PLL Off
// 1 = PLL On
#define F_WM8974_PM1_PLLEN(x)			( (x&0x1)<< 5)

// Microphone Bias Enable
// 0 = OFF (high impedance output)
// 1 = ON
#define F_WM8974_PM1_MICBEN(x)			( (x&0x1)<< 4)

// Analogue amplifier bias control
// 0 = Disabled
// 1 = Enabled
#define F_WM8974_PM1_BIASEN(x)			( (x&0x1)<< 3)

// Unused input/output tie off buffer enable
// 0 = Disabled
// 1 = Enabled
#define F_WM8974_PM1_BUFIOEN(x)			( (x&0x1)<< 2)

// Reference string impedance to VMID pin
// 00 = off (open circuit)
// 01 = 50kohm
// 10 = 500kohm
// 11 = 5kohm
#define F_WM8974_PM1_VMIDSEL(x)			( (x&0x3)<< 0)

//==============================================================================
// Power manage't 2

#define R_WM8974_PM2					0x2

// Input BOOST enable
// 0 = Boost stage OFF
// 1 = Boost stage ON
#define F_WM8974_PM2_BOOSTEN(x)			( (x&0x1)<< 4)

// Input microphone PGA enable
// 0 = Disabled
// 1 = Enabled
#define F_WM8974_PM2_INPPGAEN(x)		( (x&0x1)<< 2)

// ADC Enable Control
// 0 = ADC Disabled
// 1 = ADC Enabled
#define F_WM8974_PM2_ADCEN(x)			( (x&0x1)<< 0)

//==============================================================================
// Power manage't 3

#define R_WM8974_PM3					0x3

// MONOOUT Enable
// 0 = Disabled
// 1 = Enabled
#define F_WM8974_PM3_MONOEN(x)			( (x&0x1)<< 7)

// SPKOUTN Enable
#define F_WM8974_PM3_SPKNEN(x)			( (x&0x1)<< 6)

// SPKOUTP Enable
#define F_WM8974_PM3_SPKPEN(x)			( (x&0x1)<< 5)

// Mono Mixer Enable
#define F_WM8974_PM3_MONOMIXEN(x)		( (x&0x1)<< 3)

// Speaker Mixer Enable
#define F_WM8974_PM3_SPKMIXEN(x)		( (x&0x1)<< 2)

// DAC Enable
#define F_WM8974_PM3_DACEN(x)			( (x&0x1)<< 0)

//==============================================================================
// Audio Interface

#define R_WM8974_AI						0x4

// BCLK polarity
// 0 = normal
// 1 = inverted
#define F_WM8974_AI_BCP(x)				( (x&0x1)<< 8)

// Frame clock polarity
// 0 = normal
// 1 = inverted
// DSP Mode control
// 1 = Reserved
// 0 = Configures the interface so that MSB is available o'n 2nd BCLK rising edge after FRAME rising edge
#define F_WM8974_AI_FRAMEP(x)			( (x&0x1)<< 7)

// Word length
// 00 = 16 bits
// 01 = 20 bits
// 10 = 24 bits
// 11 = 32 bits
#define F_WM8974_AI_WL(x)				( (x&0x3)<< 5)

// Audio interface Data Format Select:
// 00 = Right Justified
// 01 = Left Justified
// 10 = I2S format
// 11 = DSP/PCM mode
#define F_WM8974_AI_FMT(x)				( (x&0x3)<< 3)

// Controls whether DAC data appears in 'right' or 'left' phases of FRAME clock:
// 0 = DAC data appear in 'left' phase of FRAME
// 1 = DAC data appear in 'right' phase of FRAME
#define F_WM8974_AI_DACLRSWAP(x)		( (x&0x1)<< 2)

// Controls whether ADC data appears in 'right' or 'left' phases of FRAME clock:
// 0 = ADC data appear in 'left' phase of FRAME
// 1 = ADC data appear in 'right' phase of FRAME
#define F_WM8974_AI_ADCLRSWAP(x)		( (x&0x1)<< 1)

//==============================================================================
// Companding ctrl

#define R_WM8974_CPC					0x5

// DAC companding
// 00 = off
// 01 = reserved
// 10 = u-law
// 11 = A-law
#define F_WM8974_CPC_DAC_COMP(x)		( (x&0x3)<< 3)

// ADC companding
// 00 = off
// 01 = reserved
// 10 = u-law
// 11 = A-law
#define F_WM8974_CPC_ADC_COMP(x)		( (x&0x3)<< 1)

// Digital loopback function
// 0 = No loopback
// 1 = Loopback enabled, ADC data output is fed directly into DAC data input
#define F_WM8974_CPC_LOOPBACK(x)		( (x&0x1)<< 0)

//==============================================================================
// Clock Gen ctrl

#define R_WM8974_CGC					0x6

// Controls the source of the clock for all internal operation:
// 0 = MCLK
// 1 = PLL output
#define F_WM8974_CGC_CLKSEL(x)			( (x&0x1)<< 8)

// Sets the scaling for either the MCLK or PLL clock output(under control of CLKSEL)
// 000 = divide by 1
// 001 = divide by 1.5
// 010 = divide by 2
// 011 = divide by 3
// 100 = divide by 4
// 101 = divide by 6
// 110 = divide by 8
// 111 = divide by 12
#define F_WM8974_CGC_MCLKDIV(x)			( (x&0x7)<< 5)

// Configures the BCLK and FRAME output frequency, for use when the chip is master over BCLK
// 000 = divide by 1 (BCLK=MCLK)
// 001 = divide by 1 (BCLK=MCLK/2)
// 010 = divide by 4
// 011 = divide by 8
// 100 = divide by 16
// 101 = divide by 32
// 110 = reserved
// 111 = reserved
#define F_WM8974_CGC_BCLKDIV(x)			( (x&0x7)<< 2)

// Sets the chips to be master over FRAME and BCLK
// 0 = BCLK and FRAME clock are inputs
// 1 = BCLK and FRAME clock are outputs generated by the WM8974 (MASTER)
#define F_WM8974_CGC_MS(x)				( (x&0x1)<< 0)

//==============================================================================
// Additinoal ctrl

#define R_WM8974_ADD					0x7

// Approximate sample rate (configures the coefficients for the internal digital filters):
// 000 = 48KHz
// 001 = 32KHz
// 010 = 24KHz
// 011 = 16KHz
// 100 = 12KHz
// 101 = 8KHz
// 110 = reserved
// 111 = reserved
#define F_WM8974_ADD_SR_48K				( 0<< 1)
#define F_WM8974_ADD_SR_32K				( 1<< 1)
#define F_WM8974_ADD_SR_24K				( 2<< 1)
#define F_WM8974_ADD_SR_16K				( 3<< 1)
#define F_WM8974_ADD_SR_12K				( 4<< 1)
#define F_WM8974_ADD_SR_8K				( 5<< 1)
#define F_WM8974_ADD_SR(x)				( (x&0x7)<< 1)


#define F_WM8974_ADD_SLOWCLKEN(x)		( (x&0x1)<< 0)

//==============================================================================
// GPIO

#define R_WM8974_GPIO					0x8

// PLL Output clock division ratio
// 00 = divide by 1
// 01 = divide by 2
// 10 = divide by 3
// 11 = divide by 4
#define F_WM8974_GPIO_OPCLKDIV(x)		( (x&0x3)<< 4)

// GPIO Polarity invert
// 0 = Non inverted
// 1 = Inverted
#define F_WM8974_GPIO_POL(x)			( (x&0x1)<< 3)

// CSB/GPIO pin function select:
// 000 = CSB input
// 001 = Jack insert detect
// 010 = Temp ok
// 011 = Amute active
// 100 = PLL clk o/p
// 101 = PLL lock
// 110 = Reserved
// 111 = Reserved
#define F_WM8974_GPIO_SEL(x)			( (x&0x7)<< 0)

//==============================================================================
// DAC Control

#define R_WM8974_DACC					0xA

// DAC soft mute enable
#define F_WM8974_DACC_DACMU(x)			( (x&0x1)<< 6)

// De-Emphasis Control
// 00 = No de-emphasis
// 01 = 32KHz sample rate
// 10 = 44.1KHz sample rate
// 11 = 48KHz sample rate
#define F_WM8974_DACC_DEEMPH(x)			( (x&0x3)<< 4)

// DAC oversample rate select
// 0 = 64x (lowest power)
// 1 = 128x (best SNR)
#define F_WM8974_DACC_DACOSR128(x)		( (x&0x1)<< 3)

// DAC auto mute enable
// 0 = auto mute disabled
// 1 = auto mute enabled
#define F_WM8974_DACC_AMUTE(x)			( (x&0x1)<< 2)

// DAC Polarity Invert
// 0 = No inversion
// 1 = DAC output inverted
#define F_WM8974_DACC_DACPOL(x)			( (x&0x1)<< 0)

//==============================================================================
// DAC digital Vol

#define R_WM8974_DACVOL					0xB

// DAC Digital Volume Control
// 0000 0000 = Digital Mute
// 0000 0001 = -127dB
// 0000 0010 = -126.5dB
// ... 0.5dB steps up to
// 1111 1111 = 0dB
#define F_WM8974_DACVOL(x)				( (x&0xFF)<< 0)

//==============================================================================
// ADC Control

#define R_WM8974_ADCC					0xE

// High Pass Filter Enable
#define F_WM8974_ADCC_HPFEN(x)			( (x&0x1)<< 8)

// Select audio mode or application mode
// 0 = Audio mode (1st order, fc = ~3.7Hz)
// 1 = Application mode (2nd order, fc = HPFCUT)
#define F_WM8974_ADCC_HPFAPP(x)			( (x&0x1)<< 7)

// Application mode cut-off frequency
#define F_WM8974_ADCC_HPFCUT(x)			( (x&0x7)<< 4)

// ADC oversample rate select
// 0 = 64x (lowest power)
// 1 = 128x (best SNR)
#define F_WM8974_ADCC_ADCOSR128(x)		( (x&0x1)<< 3)

// ADC Polarity
// 0 = normal
// 1 = invernted
#define F_WM8974_ADCC_ADCPOL(x)			( (x&0x1)<< 0)

//==============================================================================
// ADC Digital Vol

#define R_WM8974_ADCVOL					0xF

// ADC Digital Volume Control
// 0000 0000 = Digital Mute
// 0000 0001 = -127dB
// 0000 0010 = -126.5dB
// ... 0.5dB steps up to
// 1111 1111 = 0dB
#define F_WM8974_ADCVOL(x)				( (x&0xFF)<< 0)

//==============================================================================
// EQ1 _ low shelf

#define R_WM8974_EQ1					0x12

// 0 = Equaliser applied to ADC path
// 1 = Equaliser applied to DAC path
#define F_WM8974_EQ1_MODE(x)			( (x&0x1)<< 8)

// EQ Band 1 Cut-off Frequency:
// 00 = 80Hz
// 01 = 105Hz
// 10 = 135Hz
// 11 = 175Hz
#define F_WM8974_EQ1_C(x)				( (x&0x3)<< 5)

// EQ Band 1 Gain Control
#define F_WM8974_EQ1_G(x)				( (x&0x1F)<< 0)

//==============================================================================
// EQ2 _ peak 1

#define R_WM8974_EQ2					0x13

// Band 2 Bandwidth Control
// 0 = narrow bandwidth
// 1 = wide bandwidth
#define F_WM8974_EQ2_BW(x)				( (x&0x1)<< 8)

// Band 2 Centre Frequency:
// 00 = 230Hz
// 01 = 300Hz
// 10 = 385Hz
// 11 = 500Hz
#define F_WM8974_EQ2_C(x)				( (x&0x3)<< 5)

// Band 2 Gain Control
#define F_WM8974_EQ2_G(x)				( (x&0x1F)<< 0)

//==============================================================================
// EQ3 _ peak 2

#define R_WM8974_EQ3					0x14

// Band 3 Bandwidth Control
// 0 = narrow bandwidth
// 1 = wide bandwidth
#define F_WM8974_EQ3_BW(x)				( (x&0x1)<< 8)

// Band 3 Centre Frequency:
// 00 = 650Hz
// 01 = 850Hz
// 10 = 1.1kHz
// 11 = 1.4kHz
#define F_WM8974_EQ3_C(x)				( (x&0x3)<< 5)

// Band 3 Gain Control
#define F_WM8974_EQ3_G(x)				( (x&0x1F)<< 0)

//==============================================================================
// EQ4 _ peak 3

#define R_WM8974_EQ4					0x15

// Band 4 BandWidth Control
// 0 = narrow bandwidth
// 1 = wide bandwidth
#define F_WM8974_EQ4_BW(x)				( (x&0x1)<< 8)

// Band 4 Centre Frequency:
// 00 = 1.8kHz
// 01 = 2.4kHz
// 10 = 3.2kHz
// 11 = 4.1kHz
#define F_WM8974_EQ4_C(x)				( (x&0x3)<< 5)

// Band 4 Gain Control
#define F_WM8974_EQ4_G(x)				( (x&0x1F)<< 0)
//==============================================================================
// EQ5 _ high shelf

#define R_WM8974_EQ5					0x16

// Band 5 Cut-off Frequency:
// 00 = 5.3kHz
// 01 = 6.9kHz
// 10 = 9kHz
// 11 = 11.7kHz
#define F_WM8974_EQ5_C(x)				( (x&0x3)<< 5)

// Band 5 Gain Control
#define F_WM8974_EQ5_G(x)				( (x&0x1F)<< 0)

//==============================================================================
// DAC Limiter 1

#define R_WM8974_DACLM1					0x18

// Enable the DAC digital limiter:
// 0 = Disabled
// 1 = Enabled
#define F_WM8974_DACLM1_EN(x)			( (x&0x1)<< 8)

// DAC Limiter Decay time (per 6dB gain change) for 44.1kHz sampling.
// 0000 = 750us
// 0001 = 1.5ms
// 0010 = 3ms
// 0011 = 6ms
// 0100 = 12ms
// 0101 = 24ms
// 0110 = 48ms
// 0111 = 96ms
// 1000 = 192ms
// 1001 = 384ms
// 1010 = 768ms
// 1011 to 1111 = 1.536s
#define F_WM8974_DACLM1_DCY(x)			( (x&0xF)<< 4)

// DAC Limiter Attack time (per 6dB gain change) for 44.1kHz sampling
// 0000 = 94us
// 0001 = 188s
// 0010 = 375us
// 0011 = 750us
// 0100 = 1.5ms
// 0101 = 3ms
// 0110 = 6ms
// 0111 = 12ms
// 1000 = 24ms
// 1001 = 48ms
// 1010 = 96ms
// 1011 to 1111 = 192ms
#define F_WM8974_DACLM1_ATK(x)			( (x&0xF)<< 0)

//==============================================================================
// DAC Limiter 2

#define R_WM8974_DACLM2					0x19

// DAC Limiter Programmable signal threshold level (determines level at which the limiter starts to operate)
// 000 = -1dB
// 001 = -2dB
// 010 = -3dB
// 011 = -4dB
// 100 = -5dB
// 101 to 111 = -6dB
#define F_WM8974_DACLM2_LVL(x)			( (x&0x7)<< 4)

// DAC Limiter volume boost (can be used as a stand alone volume boost when LIMEN=0):
// 0000 = 0dB
// 0001 = +1dB
// 0010 = +2dB
// ... (1dB steps)
// 1011 = +11dB
// 1100 = +12dB
// 1101 to 1111 = reserved
#define F_WM8974_DACLM2_BOOST(x)		( (x&0xF)<< 0)

//==============================================================================
// Notch Filter 1

#define R_WM8974_NF1					0x1B

// Notch filter update. The notch fileter values used internally only update when once of the NFU bits is set high
#define F_WM8974_NF1_NFU(x)				( (x&0x1)<< 8)

// Notch filter enable:
// 0 = Disabled
// 1 = Enabled
#define F_WM8974_NF1_NFEN(x)			( (x&0x1)<< 7)

// Notch Filter a0 coefficient, bits [13: 7]
#define F_WM8974_NF1_NFA0(x)			( (x&0x7F)<< 0)

//==============================================================================
// Notch Filter 2

#define R_WM8974_NF2					0x1C

// Notch filter update. The notch fileter values used internally only update when once of the NFU bits is set high
#define F_WM8974_NF2_NFU(x)				( (x&0x1)<< 8)

// Notch Filter a0 coefficient, bits [ 6: 0]
#define F_WM8974_NF2_NFA0(x)			( (x&0x7F)<< 0)

//==============================================================================
// Notch Filter 3

#define R_WM8974_NF3					0x1D

// Notch filter update. The notch fileter values used internally only update when once of the NFU bits is set high
#define F_WM8974_NF3_NFU(x)				( (x&0x1)<< 8)

// Notch Filter a1 coefficient, bits [13: 7]
#define F_WM8974_NF3_NFA1(x)			( (x&0x7F)<< 0)

//==============================================================================
// Notch Filter 4

#define R_WM8974_NF4					0x1E

// Notch filter update. The notch fileter values used internally only update when once of the NFU bits is set high
#define F_WM8974_NF4_NFU(x)				( (x&0x1)<< 8)

// Notch Filter a1 coefficient, bits [ 6: 0]
#define F_WM8974_NF4_NFA1(x)			( (x&0x7F)<< 0)

//==============================================================================
// ALC control 1

#define R_WM8974_ALC1					0x20

// ALC function select :
// 0 = ALC off (PGA gain set by INPPGAVOL register bits)
// 1 = ALC on (ALC controls PGA gain)
#define F_WM8974_ALC1_SEL(x)			( (x&0x1)<< 8)

// Set Maximum Gain of PGA when using ALC:
// 111 = +35.25dB
// 110 = +29.25dB
// 101 = +23.25dB
// 100 = +17.25dB
// 011 = +11.25dB
// 010 = +5.25dB
// 001 = -0.75dB
// 000 = -6.75dB
#define F_WM8974_ALC1_MAX(x)			( (x&0x7)<< 3)

// Set minimum gain of PGA when using ALC:
// 000 = -12dB
// 001 = -6dB
// 010 = 0dB
// 011 = +6dB
// 100 = +12dB
// 101 = +18dB
// 110 = +24dB
// 111 = +30dB
#define F_WM8974_ALC1_MIN(x)			( (x&0x7)<< 0)

//==============================================================================
// ALC control 2

#define R_WM8974_ALC2					0x21

// ALC zero cross detection
#define F_WM8974_ALC2_ZC(x)				( (x&0x1)<< 8)

// ALC hold time before gain is increased
// 0000 = 0ms
// 0001 = 2.67ms
// 0010 = 5.33ms
// ... (time doubles with every step)
// 1111 = 43.691s
#define F_WM8974_ALC2_HLD(x)			( (x&0xF)<< 4)

// ALC target - sets signal level at ADC input
// 0000 = -28.5dB FS
// 0001 = -27.0dB FS
// ... (1.5dB steps)
// 1110 = -7.5dB FS
// 1111 = -6dB FS
#define F_WM8974_ALC2_LVL(x)			( (x&0xF)<< 0)

//==============================================================================
// ALC control 3

#define R_WM8974_ALC3					0x22

// Determines the ALC mode of operation:
// 0 = ALC mode
// 1 = Limiter mode
#define F_WM8974_ALC3_MODE(x)			( (x&0x1)<< 8)

// Decay (gain ramp-up) time (ALCMODE = 0)
// 			Per step	|	Per 6dB	|	90% of range
// 0000	|	410us		|	3.3ms	|	24ms
// 0001	|	820us		|	6.6ms	|	48ms
// 0010	|	1.64ms		|	13.1ms	|	192ms
// ... (time doubles with every step)
// 1010	|	420ms		|	3.36s	|	24.576s
// or higher

// Decay (gain ramp-up) time (ALCMODE = 1)
// 			Per step	|	Per 6dB	|	90% of range
// 0000	|	90.8us		|	726.4us	|	5.26ms
// 0001	|	181.6us		|	1.453ms	|	10.53ms
// 0010	|	363.2us		|	2.905ms	|	21.06ms
// ... (time doubles with every step)
// 1010	|	93ms		|	744ms	|	5.39s
#define F_WM8974_ALC3_DCY(x)			( (x&0xF)<< 4)

// ALC attack (gain ramp-down) time (ALCMODE = 0)
// 			Per step	|	Per 6dB	|	90% of range
// 0000	|	104us		|	832us	|	6ms
// 0001	|	208us		|	1.664ms	|	12ms
// 0010	|	416us		|	3.328ms	|	24.1ms
// ... (time doubles with every step)
// 1010	|	106ms		|	852s	|	6.18s
// or higher

// ALC attack (gain ramp-down) time (ALCMODE = 1)
// 			Per step	|	Per 6dB	|	90% of range
// 0000	|	22.7us		|	182.4us	|	1.31ms
// 0001	|	45.4us		|	363.2us	|	2.62ms
// 0010	|	90.8us		|	726.4us	|	5.26ms
// ... (time doubles with every step)
// 1010	|	23.2ms		|	186ms	|	1.348s
#define F_WM8974_ALC3_ATK(x)			( (x&0xF)<< 0)

//==============================================================================
// Noise Gate

#define R_WM8974_NGATE					0x23

// ALC Noise gate function enable
#define F_WM8974_NGATE_NGEN(x)			( (x&0x1)<< 3)

// ALC Noise gate threshold:
// 000 = -39dB
// 001 = -45dB
// 010 = -51dB
// ... (6dB steps)
// 111 = -81dB
#define F_WM8974_NGATE_NGTH(x)			( (x&0x3)<< 0)

//==============================================================================
// PLL N

#define R_WM8974_PLLN					0x24

// 0 = MCLK input not divided (default)
// 1 = Divide MCLK by 2 before input PLL
#define F_WM8974_PLLN_PRESCALE(x)		( (x&0x1)<< 4)

// Integer (N) part of PLL input/output frequency ratio. Use values greater than 5 and less than 13
#define F_WM8974_PLLN(x)				( (x&0xF)<< 0)

//==============================================================================
// PLL K 1

#define R_WM8974_PLLK1					0x25

// Fractional (K) part of PLL1 input/output frequency radio (treat as one 24-digit binary number)
#define F_WM8974_PLLK1(x)				( (x&0x3F)<< 0)

//==============================================================================
// PLL K 2

#define R_WM8974_PLLK2					0x26

// Fractional (K) part of PLL1 input/output frequency radio (treat as one 24-digit binary number)
#define F_WM8974_PLLK2(x)				( (x&0x1FF)<< 0)

//==============================================================================
// PLL K 3

#define R_WM8974_PLLK3					0x27

// Fractional (K) part of PLL1 input/output frequency radio (treat as one 24-digit binary number)
#define F_WM8974_PLLK3(x)				( (x&0x1FF)<< 0)

//==============================================================================
// Attenuation ctrl

#define R_WM8974_ATTEN					0x28

// Attenuation control for bypass path (output of input boost stage) to mono mixer input
// 0 = 0dB
// 1 = -10dB
#define F_WM8974_ATTEN_MONO(x)			( (x&0x1)<< 2)

// Attenuation control for bypass path (output of input boost stage) to speaker mixer input
// 0 = 0dB
// 1 = -10dB
#define F_WM8974_ATTEN_SPK(x)			( (x&0x1)<< 1)

//==============================================================================
// Input ctrl

#define R_WM8974_IP						0x2C

// Microphone Bias Voltage Control
// 0 = 0.9 * AVDD
// 1 = 0.65 * AVDD
#define F_WM8974_IP_MBVSEL(x)			( (x&0x1)<< 8)

// Auxiliary Input Mode
// 0 = inverting buffer
// 1 = mixer (on-chip input resistor bypassed)
#define F_WM8974_IP_AUXMODE(x)			( (x&0x1)<< 3)

// Select AUX amplifier output as input PGA signal source.
// 0 = AUX not connected to input PGA
// 1 = AUX connected to input PGA amplifier negative terminal.
#define F_WM8974_IP_AUX2(x)				( (x&0x1)<< 2)

// Connect MICN to input PGA negative terminal.
// 0 = MICN not connected to input PGA
// 1 = MICN connected to input PGA amplifier negative terminal
#define F_WM8974_IP_MICN2(x)			( (x&0x1)<< 1)

// Connect input PGA amplifier positive terminal to MICP or VMID
// 0 = input PGA amplifier positive terminal connected to VMID
// 1 = input PGA amplifier positive terminal connected to MICP through variable resister string
#define F_WM8974_IP_MICP2(x)			( (x&0x1)<< 0)

//==============================================================================
// INP PGA gain ctrl

#define R_WM8974_INPPGA					0x2D

// Input PGA zero cross enable:
// 0 = Update gain when gain register changes
// 1 = Update gain on 1st zero cross after gain register write
#define F_WM8974_INPPGA_AZC(x)			( (x&0x1)<< 7)

// Mute control for input PGA:
// 0 = Input PGA not muted, normal operation
// 1 = Input PGA muted (and disconnected from the following input BOOST stage)
#define F_WM8974_INPPGA_MUTE(x)			( (x&0x1)<< 6)

// Input PGA volume
// 000000 = -12dB
// 000001 = -11.25dB
// .
// 010000 = 0dB
// .
// 111111 = 35.25dB
#define F_WM8974_INPPGA_VOL(x)			( (x&0x3F)<< 0)

//==============================================================================
// ADC Boost ctrl

#define R_WM8974_ADCBST					0x2F

// Input Boost
// 0 = PGA output has +0dB gain throught input BOOST stage
// 1 = PGA output gas +20dB gain through input BOOST state
#define F_WM8974_ADCBST_PGA(x)			( (x&0x1)<< 8)

// Controls the MICP pin to the input boost stage (NB, when using this path set MICP2INPPGA=0):
// 000 = Path disabled (disconnected)
// 001 = -12dB gain through boost stage
// 010 = -9dB gain through boost stage
// ...
// 111 = +6dB gain through boost stage
#define F_WM8974_ADCBST_MICP2(x)		( (x&0x7)<< 4)

// Controls auxiliary amplifier to the input boost stage:
// 000 = Path disabled (disconnected)
// 001 = -12dB gain through boost stage
// 010 = -9dB gain through boost stage
// ...
// 111 = +6dB gain through boost stage
#define F_WM8974_ADCBST_AUX2(x)			( (x&0x7)<< 0)

//==============================================================================
// Output ctrl

#define R_WM8974_OP						0x31

// Mono output boost stage control
// 0 = No boost (output is inverting buffer)
// 1 = 1.5x gain boost
#define F_WM8974_OP_MONOBST(x)			( (x&0x1)<< 3)

// Speaker output boost stage control
// 0 = No boost (outputs are inverting buffers)
// 1 = 1.5x gain boost
#define F_WM8974_OP_SPKBST(x)			( (x&0x1)<< 2)

// Thermal Shutdown Enable
// 0 : thermal shutdown disabled
// 1 : thermal shutdown enabled
#define F_WM8974_OP_TSDEN(x)			( (x&0x1)<< 1)

// VREF (AVDD/2 or 1.5x AVDD/2) to analogue output resistance
// 0 : approx 1kohm
// 1 : approx 30kohm
#define F_WM8974_OP_VROI(x)				( (x&0x1)<< 0)

//==============================================================================
// SPK mixer ctrl

#define R_WM8974_SPKMIX					0x32

// Output of auxiliary amplifier to speaker mixer input
// 0 : not selected
// 1 : selected
#define F_WM8974_SPKMIX_AUX2(x)			( (x&0x1)<< 5)

// Bypass path (output of input boost stage) to speaker mixer input
// 0 = not selected
// 1 = selected
#define F_WM8974_SPKMIX_BYP2(x)			( (x&0x1)<< 1)

// Output of DAC to speaker mixer input
// 0 = not selected
// 1 = selected
#define F_WM8974_SPKMIX_DAC2(x)			( (x&0x1)<< 0)
//==============================================================================
// SPK volume ctrl

#define R_WM8974_SPKVOL					0x36

// Speaker Volume control zero cross enable:
// 1 = Change gain on zero cross only
// 0 = Change gain immediately
#define F_WM8974_SPKVOL_ZC(x)			( (x&0x1)<< 7)

// Speaker output mute enable
// 0 = Speaker output enabled
// 1 = Speaker output muted (VMIDOP)
#define F_WM8974_SPKVOL_MUTE(x)			( (x&0x1)<< 6)

// Speaker Volume Adjust
// 111111 = +6dB
// 111110 = +5dB
// ... (1.0 dB steps)
// 111001 = +0dB
// ...
// 000000 = -57dB
#define F_WM8974_SPKVOL_VOL(x)			( (x&0x3F)<< 0)
//==============================================================================
// MONO mixer ctrl

#define R_WM8974_MONOMIX				0x38

// MONOOUT Mute Control
// 0 = No mute
// 1 = Output muted. During mute the mono output will output VMID which can be used as a DC reference for a headphone out
#define F_WM8974_MONOMIX_MUTE(x)		( (x&0x1)<< 6)

// Output of Auxiliary amplifier to mono mixer input:
// 0 = not selected
// 1 = selected
#define F_WM8974_MONOMIX_AUX2(x)		( (x&0x1)<< 2)

// Bypass path (output of input boost stage) to mono mixer input
// 0 = non selected
// 1 = selected
#define F_WM8974_MONOMIX_BYP2(x)		( (x&0x1)<< 1)

// Output of DAC to mono mixer input
// 0 = not selected
// 1 = selected
#define F_WM8974_MONOMIX_DAC2(x)		( (x&0x1)<< 0)

//==============================================================================


void wm8974_init(void);
void WM8974_init_bypass(void);
void wm8974_mic_on(void);
void wm8974_mic_off(void);
void wm8974_speacker_on(void);
void wm8974_speacker_off(void);
void wm8974_mono_on(void);
void wm8974_mono_off(void);
void wm8974_set_spk_vol(U8 vol);
void wm8974_write(U32 reg, U32 data);
