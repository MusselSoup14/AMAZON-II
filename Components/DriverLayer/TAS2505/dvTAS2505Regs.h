/**
  ******************************************************************************
  * @file    dvTAS2505Regs.h
  * @author  MadeFactory Chief Engineer Jong-Jun Yim
  * @version V1.0.0
  * @date    09-05-2019
  * @brief   This file contains Register Map for the Audio Amp Driver
  ******************************************************************************
  * @attention
  *
  * MadeFactory owns the sole copyright to this software. Under international
  * copyright laws you (1) may not make a copy of this software except for
  * the purposes of maintaining a single archive copy, (2) may not derive
  * works herefrom, (3) may not distribute this work to others. These rights
  * are provided for information clarification, other restrictions of rights
  * may apply as well.
  *
  * <h2><center>&copy; Copyright 1997-2019 (c) MadeFactory Inc.</center></h2>
  ******************************************************************************
  */  
  
/* Define to prevent recursive inclusion -------------------------------------*/
#if !defined(_INCLUDED_DVTAS2505REGS_H_)
#define _INCLUDED_DVTAS2505REGS_H_

 /* Includes ------------------------------------------------------------------*/
#include "common.h"

#if (USE_LCD_1024x600 ||  USE_LCD_1280x800)
/** @addtogroup CWM-T10
  * @{
  */


/** @defgroup dvTAS2505 
  * @brief TAS2505 Driver
  * @{
  */

/** @defgroup dvTAS2505_Exported_Types
  * @{
  */

//----------------------------------------------------------------------------
// TAS2505 Register to consist of Page 
//----------------------------------------------------------------------------
#define TAS_PAGE_SHIFT      8         // Page offset in (Page | Regster) address
#define TAS_PAGE_MASK       0xFF00
#define TAS_REG_SHIFT       0         // Regster offset in (Page | Regster) address
#define TAS_REG_MASK        0x00FF

#define TAS_PAGE(x)           (((WORD)x << TAS_PAGE_SHIFT) & TAS_PAGE_MASK)

// ---------------------------------------------------------------------------
// PAGE 0 :  Clock Multipliers, Dividers, Serial Interface, Flags
//           , Interrupts and GPIOs
// ---------------------------------------------------------------------------  

//4  Page 0 / Register 0: Page Select Register - 0x00 / 0x00 [b'0]
#define REG_TAS_PAGE        (TAS_PAGE(0) | 0)
typedef union tagTASPAGE
{
  BYTE cData;
  struct
  {
    BYTE PAGE;
    /* 0-255: Selects the Register Page for next read or write command. 
              See the Table "Summary of Memory Map" for details.
    */  
  };
}TAS_PAGE, *PTAS_PAGE;

//4  Page 0 / Register 1: Software Reset Register - 0x00 / 0x01 [b'0]
#define REG_TAS_SWRESET     (TAS_PAGE(0) | 1)
typedef union tagTASRESET
{
  BYTE cData;
  struct
  {
    BYTE RESET:       1;
    /* Self clearing software reset bit
        (*)0: Don't care
        1: Self-clearing software rese */    
    BYTE RSV71:       7; /* Reserved. Write only the default values */  
  };
}TAS_SWRESET, *PTAS_SWRESET;

//4 Page 0 / Register 4: Clock Setting Register 1, Multiplexers - 0x00 / 0x04 [b'0]
#define REG_TAS_CLOCK_MUX1       (TAS_PAGE(1) | 4)
typedef union tagTASCLOCKMUX1
{
  BYTE cData;
  struct
  {
    /*[1:0]*/ BYTE CODEC_CLKIN:       2;
    /* Select CODEC_CLKIN
        (*)00: MCLK pin is CODEC_CLKIN
        01: BCLK pin is CODEC_CLKIN
        10: GPIO pin is CODEC_CLKIN
        11: PLL Clock is CODEC_CLKIN */
    /*[3:2]*/ BYTE PLL_CLKIN:         2;
    /* Select PLL Input Clock
       (*)00: MCLK pin is input to PLL
        01: BCLK pin is input to PLL
        10: GPIO pin is input to PLL
        11: DIN pin is input to PLL */
    /*[5:4]*/ BYTE RSV54:             2; /* Reserved. Write only the default values */  
    /*[6]*/   BYTE PLL_RANGE:         1;
    /* Select PLL Range
        (*)0: Low PLL Clock Range
        1: High PLL Clock Range */
    /*[7]*/   BYTE RSV7:              1; /* Reserved. Write only the default values */  
  };
}TAS_CLOCK_MUX1, *PTAS_CLOCK_MUX1;

//4 Page 0 / Register 5: Clock Setting Register 2, PLL P and R Values - 0x00 / 0x05 [b'0_001_0001] 
//4 Page 0 / Register 6: Clock Setting Register 3, PLL J Values - 0x00 / 0x06 [b'00_00_0100]
//4 Page 0 / Register 7: Clock Setting Register 4, PLL D Values (MSB) - 0x00 / 0x07 [b'0]
//4 Page 0 / Register 8: Clock Setting Register 5, PLL D Values (LSB) - 0x00 / 0x08 [b'0]
#define REG_TAS_CLOCK_PLL         (TAS_PAGE(0) | 5)
#define REG_TAS_CLOCK_PLL_P       (TAS_PAGE(0) | 5)
#define REG_TAS_CLOCK_PLL_J       (TAS_PAGE(0) | 6)
#define REG_TAS_CLOCK_PLL_DH      (TAS_PAGE(0) | 7)
#define REG_TAS_CLOCK_PLL_DL      (TAS_PAGE(0) | 8)

typedef union tagTASCLOCKPLL
{
  DWORD dwData;
  struct
  {
    /* 0 */
    BYTE PLL_R:            4;
    /* PLL Multiplier R Control
      0000: Reserved. Do not use
      (*)0001: R = 1
      0010: R = 2
      0011: R = 3
      0100: R = 4
      0101 ~ 0111: Reserved. Do not use */
    BYTE PLL_P:             3;
    /* PLL Divider P Control
      000: P=8
      (*)001: P=1
      010: P=2
      011: P=3
      100: P=4
      101: P=5
      110: P=6
      111: P=7 */
    BYTE PLL_PWR:           1;
    /* PLL Power Control
      (*)0: PLL Power Down
      1: PLL Power Up */

    /* 1 */
    BYTE PLL_J:             6;
    /* PLL divider J value
       00 0000…00 0011: Do not use
       *00 0100: J = 4
       00 0101: J = 5
       …
       11 1110: J = 62
       11 1111: J = 63 */
    BYTE RSV1_76:           2; /* Reserved. Write only the default values */

    /* 2 */
    BYTE PLL_DH:            6;     
    BYTE RSV2_76:           2; /* Reserved. Write only the default values */  

    /* 3 */
    BYTE PLL_DL;
    /* PLL divider D value(MSB) and PLL divider D value(LSB)
      (*)00 0000 0000 0000: D=0000
      00 0000 0000 0001: D=0001
      …
      10 0111 0000 1110: D=9998
      10 0111 0000 1111: D=9999
      10 0111 0001 0000…11 1111 1111 1111: Do not use
      Note: This register will be updated only when the Page-0, Reg-8 is written immediately after
      Page-0, Reg-7. */    
  };    
}TAS_CLOCK_PLL, *PTAS_CLOCK_PLL;

//4 Page 0 / Register 11: Clock Setting Register 6, NDAC Values - 0x00 / 0x0B [b'0_0000001]
#define REG_TAS_CLOCK_NDAC         (TAS_PAGE(0) | 11)
typedef union tagTASCLOCKNDAC
{
  BYTE cData;
  struct
  {
    BYTE NDAC:              7;
    /* NDAC Value
        000 0000: NDAC=128
        (*)000 0001: NDAC=1
        000 0010: NDAC=2
        …
        111 1110: NDAC=126
        111 1111: NDAC=127
        Note: Please check the clock frequency requirements in the Overview section. */
    BYTE NDAC_DIV_PWR:       1;
    /* NDAC Divider Power Control
        (*)0: NDAC divider powered down
        1: NDAC divider powered up */        
  };
}TAS_CLOCK_NDAC, *PTAS_CLOCK_NDAC;

//4 Page 0 / Register 12: Clock Setting Register 7, MDAC Values - 0x00 / 0x0C [b'0_0000001]
#define REG_TAS_CLOCK_MDAC         (TAS_PAGE(0) | 12)
typedef union tagTASCLOCKMDAC
{
  BYTE cData;
  struct
  {
    BYTE MDAC:              7;
    /* MDAC Value
        000 0000: MDAC=128
        (*)000 0001: MDAC=1
        000 0010: MDAC=2
        …
        111 1110: MDAC=126
        111 1111: MDAC=127
        Note: Please check the clock frequency requirements in the Overview section. */
    BYTE MDAC_DIV_PWR:       1;
    /* MDAC Divider Power Control
        (*)0: MDAC divider powered down
        1: MDAC divider powered up */        
  };
}TAS_CLOCK_MDAC, *PTAS_CLOCK_MDAC;

//4 Page 0 / Register 13: DAC OSR Setting Register 1, MSB Value - 0x00 / 0x0D [b'0000_0000] 
//4 Page 0 / Register 14: DAC OSR Setting Register 2, LSB Value - 0x00 / 0x0E [b'1000_0000]
#define REG_TAS_DAC_OSR           (TAS_PAGE(0) | 13)
#define REG_TAS_DAC_OSRH          (TAS_PAGE(0) | 13)
#define REG_TAS_DAC_OSRL          (TAS_PAGE(0) | 14)
typedef union tagTASDACDOSR
{
  WORD wData;
  struct
  {   
    /* 0 */
    BYTE DOSR_H:            2;
    /* DAC OSR (DOSR) MSB Setting
        00 0000 0000: DOSR=1024
        00 0000 0001: DOSR=1
        00 0000 0010: DOSR=2
        …
        11 1111 1110: DOSR=1022
        11 1111 1111: DOSR=1023
        Note: This register is updated when Page-0, Reg-14 is written to immediately after Page-0, Reg-13. */    
    BYTE RSV1_72:           6; /* Reserved. Write only the default values */ 

    /* 1 */
    BYTE DOSR_L;
    /* DAC OSR (DOSR) LSB Setting
        00 0000 0000: DOSR=1024
        00 0000 0001: DOSR=1
        00 0000 0010: DOSR=2
        …
        (*)00 1000 0000: DOSR=100
        …
        11 1111 1110: DOSR=1022
        11 1111 1111: DOSR=1023
        Note: This register is updated when Page-0, Reg-14 is written to immediately after Page-0, Reg-13. */
  };
}TAS_DAC_OSR, *PTAS_DAC_OSR;

//4 Page 0 / Registers 25: Clock Setting Register 10, Multiplexers - 0x00 / 0x19 [b'0]
#define REG_TAS_CLOCK_MUX2      (TAS_PAGE(0) | 25)
typedef union tagTASCLOCKMUX2
{
  BYTE cData;
  struct
  {
    BYTE CDIV_CLKIN:        3;  
    /* CDIV_CLKIN Clock Selection
        (*)000: CDIV_CLKIN = MCLK
        001: CDIV_CLKIN = BCLK
        010: CDIV_CLKIN = DIN
        011: CDIV_CLKIN = PLL_CLK
        100: CDIV_CLKIN = DAC_CLK
        101: CDIV_CLKIN = DAC_MOD_CLK */
    BYTE RSV73:             5; /* Reserved. Write only the default values */ 
  };
}TAS_CLOCK_MUX2, *PTAS_CLOCK_MUX2;

//4 Page 0 / Registers 26: Clock Setting Register 11, CLKOUT M divider value - 0x00 / 0x1A [b'0_0000001]
#define REG_TAS_CLOCK_CLKOUTM      (TAS_PAGE(0) | 26)
typedef union tagTASCLOCKCLKOUTM
{
  BYTE cData;
  struct
  {
    BYTE DIV:       7;
    /* CLKOUT M divider value
        000 0000: CLKOUT divider M = 128
        (*)000 0001: CLKOUT divider M = 1
        000 0010: CLKOUT divider M = 2
        ...
        111 1110: CLKOUT divider M = 126
        111 1111: CLKOUT divider M = 127
        Note: Check the clock frequency requirements in the application overview section. */    
    BYTE DIV_PWR:   1;    
    /* CLKOUT M divider power control
        (*)0: CLKOUT M divider is powered down.
        1: CLKOUT M divider is powered up. */
  };
}TAS_CLOCK_CLKOUTM, *PTAS_CLOCK_CLKOUTM;

//4 Page 0 / Register 27: Audio Interface Setting Register 1 - 0x00 / 0x1B [b'0] 
#define REG_TAS_AIF1        (TAS_PAGE(0) | 27)
typedef union tagTASAIF1
{
  BYTE cData;
  struct
  {
    BYTE RSV10:         2; /* Reserved. Write only the default values */ 
    BYTE WCLK_DIR:      1;
    /* WCLK Direction Control
        (*)0: WCLK is input to the device
        1: WCLK is output from the device */
    BYTE BLCK_DIR:      1;
    /* BCLK Direction Control
        (*)0: BCLK is input to the device
        1: BCLK is output from the device */
    BYTE WORD_LEN:      2;
    /* Audio Data Word length
        (*)00: Data Word length = 16 bits
        01: Data Word length = 20 bits
        10: Data Word length = 24 bits
        11: Data Word length = 32 bits */
    BYTE FORMAT:        2;    
    /* Audio Interface Selection
        (*)00: Audio Interface = I2S
        01: Audio Interface = DSP
        10: Audio Interface = RJF
        11: Audio Interface = LJF */
  };
}TAS_AIF1, *PTAS_AIF1;

//4 Page 0 / Register 28: Audio Interface Setting Register 2, Data offset setting - 0x00 / 0x1C [b'0] 
#define REG_TAS_AIF2        (TAS_PAGE(0) | 28)
typedef union tagTASAIF2
{
  BYTE cData;
  struct
  {
    BYTE DATA_OFFSET;
    /* Data Offset Value
        (*)0000 0000: Data Offset = 0 BCLK's
        0000 0001: Data Offset = 1 BCLK's
        …
        1111 1110: Data Offset = 254 BCLK's
        1111 1111: Data Offset = 255 BCLK's */  
  };
}TAS_AIF2, *PTAS_AIF2;

//4 Page 0 / Register 29: Audio Interface Setting Register 3 - 0x00 / 0x1D [b'0]
#define REG_TAS_AIF3        (TAS_PAGE(0) | 29)
typedef union tagTASAIF3
{
  BYTE cData;
  struct
  {
    BYTE BDIV_CLKIN_MUX:    2;
    /* BDIV_CLKIN Multiplexer Control
        (*)00: BDIV_CLKIN = DAC_CLK
        01: BDIV_CLKIN = DAC_MOD_CLK
        10: Do not use
        11: Do not use */
    BYTE PBCLK_PWCLK_PWR:   1;
    /* Primary BCLK and Primary WCLK Power control
        (*)0: Primary BCLK and Primary WCLK buffers are powered up when they are used in clock
           generation even when the codec is powered down
        1: Priamry BCLK and Primary WCLK buffers are powered down when the codec is powered down */    
    BYTE BCLK_POL:          1;    
    /* Audio Bit Clock Polarity Control
        (*)0: Default Bit Clock polarity
        1: Bit Clock is inverted w.r.t. default polarity */
    BYTE RSV74:             4; /* Reserved. Write only the default values */ 
  };
}TAS_AIF3, *PTAS_AIF3;

//4 Page 0 / Register 30: Clock Setting Register 12, BCLK N Divider- 0x00 / 0x1E [b'0_0000001]
#define REG_TAS_BCLK_DIVN        (TAS_PAGE(0) | 30)
typedef union tagTASBCLKDIVN
{
  BYTE cData;
  struct
  {
    BYTE DIV:           7; 
    /* BCLK N Divider value
        000 0000: BCLK divider N = 128
        (*)000 0001: BCLK divider N = 1
        ...
        111 1110: BCLK divider N = 126
        111 1111: BCLK divider N = 127 */
    BYTE PWR:           1;
    /* BCLK N Divider Power Control
        (*)0: BCLK N-divider is powered down.
        1: BCLK N-divider is powered up. */
  };
}TAS_BCLK_DIVN, *PTAS_BCLK_DIVN;

//4 Page 0 / Register 31: Audio Interface Setting Register 4, Secondary Audio Interface - 0x00 / 0x1F [b'0] 
#define REG_TAS_AIF4            (TAS_PAGE(0) | 31)
typedef union tagTASAIF4
{
  BYTE cData;
  struct
  {
    BYTE S_DATAIN_MUX:    1;
    /* Secondary Data Input Multiplexer
        (*)0: Secondary Data Input = GPIO
        1: Secondary Data Input = SCLK */
    BYTE RSV21:           2; /* Reserved. Write only the default values */ 
    BYTE S_WCLK_MUX:      2;
    /* Secondary Word Clock Multiplexer
        (*)00: Secondary Word Clock = GPIO
        01: Secondary Word Clock = SCLK
        10: Secondary Word Clock = MISO
        11: Secondary Word Clock = DOUT */
    BYTE S_BCLK_MUX:      2;
    /* Secondary Bit Clock Multiplexer
        (*)00: Secondary Bit Clock = GPIO
        01: Secondary Bit Clock = SCLK
        10: Secondary Bit Clock = MISO
        11: Secondary Bit Clock = DOUT */
    BYTE RSV7:            1; /* Reserved. Write only the default values */ 
  };
}TAS_AIF4, *PTAS_AIF4;

//4 Page 0 / Register 32: Audio Interface Setting Register 5 - 0x00 / 0x20 [b'0]
//4 Page 0 / Register 33: Audio Interface Setting Register 6 - 0x00 / 0x21 [b'0]
#define REG_TAS_AIF56            (TAS_PAGE(0) | 32)
typedef union tagTASAIF56
{
  WORD wData;
  struct
  {
    /* 0 */
    BYTE DATAIN_CTRL:     1;
    /* Audio Data In Control
        (*)0: DIN is used for Audio Data In
        1: Secondary Data In is used for Audio Data In */
    BYTE RSV1:            1; /* Reserved. Write only the default values */ 
    BYTE WCLKIN_CTRL:     1;
    /* Primary / Secondary Word Clock Control
        (*)0: Primary Word Clock(WCLK) is used for Audio Interface
        1: Secondary Word Clock is used for Audio Interface */
    BYTE BCLKIN_CTRL:     1;  
    /* Primary / Secondary Bit Clock Control
        (*)0: Primary Bit Clock(BCLK) is used for Audio Interface and Clocking
        1: Secondary Bit Clock is used for Audio Interface and Clocking */
    BYTE RSV74:           4; /* Reserved. Write only the default values */ 

    /* 1 */
    BYTE S_DATAOUT_CTRL:  1;
    /* Secondary Data Out output control
        (*)0: Secondary Data Output = DIN input (Loopback)
        1: Reserved. Do not use. */
    BYTE P_DATAOUT_CTRL:  1;
    /* Primary Data Out output control
        (*)0: Reserved. Do not use.
        1: DOUT output = Secondary Data Input (Loopback) */
    BYTE S_WCLKOUT_CTRL:  2;
    /* Secondary Word Clock Output Control
        (*)00: Secondary Word Clock output = WCLK input
        01: Secondary Word Clock output = Generated DAC_FS
        10: Reserved. Do not use.
        11: Reserved. Do not use */
    BYTE WCLKOUT_CTRL:    2;
    /* WCLK Output Control
        (*)00: WCLK Output = Generated DAC_FS
        01: Reserved. Do not use.
        10: WCLK Output = Secondary Word Clock Input
        11: Reserved. Do not use */
    BYTE S_BCLKOUT_CTRL:  1;
    /* Secondary Bit Clock Output Control
        (*)0: Secondary Bit Clock = BCLK input
        1: Secondary Bit Clock = Generated Primary Bit Clock */
    BYTE BCLKOUT_CTRL:    1;
    /* BCLK Output Control
        (*)0: BCLK Output = Generated Primary Bit Clock
        1: BCLK Output = Secondary Bit Clock Input */
  };  
}TAS_AIF56, *PTAS_AIF56;

//4 Page 0 / Register 34: Digital Interface Misc. Setting Register - 0x00 / 0x22 [b'0]
#define REG_TAS_DIF            (TAS_PAGE(0) | 34)
typedef union tagTASDIF
{
  BYTE cData;
  struct
  {
    BYTE RSV40:               5; /* Reserved. Write only the default values */ 
    BYTE I2C_GENCALL_CONF:    1;
    /* I2C General Call Address Configuration
        (*)0: I2C General Call Address will be ignored
        1: I2C General Call Address accepted */
    BYTE RSV76:               2; /* Reserved. Write only the default values */ 
  };
}TAS_DIF, *PTAS_DIF;

//4 Page 0 / Register 37: DAC Flag Register 1 - 0x00 / 0x25 [Readonly b'0]
//4 Page 0 / Register 38: DAC Flag Register 2- 0x00 / 0x26 [Readonly b'0]
#define REG_TAS_DAC_FLAG            (TAS_PAGE(0) | 37)
typedef union tagTASDACFLAG
{
  WORD wData;
  struct
  {
    /* 0 */
    BYTE RSV40:           5; /* Reserved. Read as Always 0 */
    BYTE HPDRV_PWR:       1; 
    /* Headphone Driver (HPOUT) Power Status Flag
        (*)0: HPOUT driver powered down
        1: HPOUT driver powered up */
    BYTE RSV6:            1; /* Reserved. Read as Always 0 */    
    BYTE DAC_PWR:         1;
    /* DAC Power Status Flag
        (*)0: DAC powered down
        1: DAC powered up */

    /* 1 */
    BYTE RSV30:           4; /* Reserved. Read as Always 0 */
    BYTE DAC_PGA:         1;
    /* DAC PGA Status Flag
        (*)0: Gain applied in DAC PGA is not equal to Gain programmed in Control Register
        1: Gain applied in DAC PGA is equal to Gain programmed in Control Register" */
    BYTE RSV75:           3; /* Reserved. Read as Always 0 */
  };
}TAS_DAC_FLAG, *PTAS_DAC_FLAG;

//4 Page 0 / Registers 42: Sticky Flag Register 1- 0x00 / 0x2A [Readonly b'0]
#define REG_TAS_STICKY_FLAG1        (TAS_PAGE(0) | 42)
typedef union tagTASSTICKYFLAG1
{
  BYTE cData;
  struct
  {
    BYTE RSV60:             7; /* Reserved. Read as Always 0 */
    BYTE DAC_OVERFLOW:      1;
    /* DAC Overflow Status. This sticky flag will self clear on read
        (*)0: No overflow in DAC
        1: Overflow has happened in DAC since last read of this register */
  };
}TAS_STICKY_FLAG1, *PTAS_STICKY_FLAG1;

//4 [b'0] Page 0 / Registers 43: Interrupt Flags Register 1 - 0x00 / 0x2B [Readonly b'0]
#define REG_TAS_INT_FLAG1        (TAS_PAGE(0) | 43)
typedef union tagTASINTFLAG1
{
  BYTE cData;
  struct
  {
    BYTE RSV60:             7; /* Reserved. Read as Always 0 */
    BYTE DAC_OVERFLOW:      1;
    /* DAC Overflow Status.
        (*)0: No overflow in DAC
        1: Overflow condition is present in DAC at the time of reading the register */
  };
}TAS_INT_FLAG1, *PTAS_INT_FLAG1;

//4 Page 0 / Register 44: Sticky Flag Register 2 - 0x00 / 0x2C [Readonly b'0]
#define REG_TAS_STICKY_FLAG2        (TAS_PAGE(0) | 44)
typedef union tagTASSTICKYFLAG2
{
  BYTE cData;
  struct
  {
    BYTE RSV60:               7;
    BYTE HPOUT_OVERCUR_DET:   1;
    /* HPOUT Over Current Detect Flag
        0: Over Current not detected on HPOUT
        1: Over Current detected on HPOUT (will be cleared when the register is read) */
  };
}TAS_STICKY_FLAG2, *PTAS_STICKY_FLAG2;

//4 Page 0 / Register 46: Interrupt Flag Register 2 - 0x00 / 0x2E [Readonly b'0]
#define REG_TAS_INT_FLAG2        (TAS_PAGE(0) | 46)
typedef union tagTASINTFLAG2
{
  BYTE cData;
  struct
  {
    BYTE RSV60:              7; /* Reserved. Read as Always 0 */
    BYTE HPOUT_OVERCUR_DET:  1;
    /* HPOUT Over Current Detect Flag
        (*)0: Over Current not detected on HPOUT
        1: Over Current detected on HPOUT */
  };
}TAS_INT_FLAG2, *PTAS_INT_FLAG2;

//4 Page 0 / Register 48: INT1 Control Register - 0x00 / 0x30 [b'0] 
//4 Page 0 / Register 49: INT2 Interrupt Control Register - 0x00 / 0x31 [b'0]
#define REG_TAS_INT_CTRL        (TAS_PAGE(0) | 48)
typedef union tagTASINTCTRL
{
  WORD wData;
  struct
  {
    /* 0 */
    BYTE INT1_PULSECTRL:      1;    
    /* INT1 pulse control
      (*)0: INT1 is active high interrupt of 1 pulse of approx. 2ms duration
      1: INT1 is active high interrupt of multiple pulses, each of duration 2ms. 
         To stop the pulse train, read Page-0, Reg-42, or 44 */
    BYTE RSV0_21:             2; /* Reserved. Write only the default values */ 
    BYTE INT1_HPOVERCUR:      1;
    /* INT1 Interrupt for Over Current Condition
        (*)0: Headphone Over Current condition will not generate a INT1 interrupt.
        1: Headphone Over Current condition will generate a INT1 interrupt. */
    BYTE RSV0_74:             4; /* Reserved. Write only the default values */ 

    /* 1 */
    BYTE INT2_PULSECTRL:      1;
    /* INT2 pulse control
        (*)0: INT2 is active high interrupt of 1 pulse of approx. 2ms duration
        1: INT2 is active high interrupt of multiple pulses, each of duration 2ms. 
           To stop the pulse train, read Page-0, Reg-42, or 44 */
    BYTE RSV1_21:             2; /* Reserved. Write only the default values */ 
    BYTE INT2_HPOVERCUR:      1;
    /* INT2 Interrupt for Over Current Condition
        (*)0: Headphone Over Current condition will not generate a INT2 interrupt.
        1: Headphone Over Current condition will generate a INT2 interrupt. */
    BYTE RSV1_74:             4; /* Reserved. Write only the default values */ 
  };
}TAS_INT_CTRL, *PTAS_INT_CTRL;

//4 Page 0 / Register 52: GPIO/DOUT Control Register - 0x00 / 0x34 [b'0]
#define REG_TAS_GPIO_CTRL        (TAS_PAGE(0) | 52)
typedef union tagTASGPIOCTRL
{
  BYTE cData;
  struct
  {
    BYTE GPIO_OUTPUT:         1;
    /* GPIO as general purpose output control
        (*)0: GPIO pin is driven to '0' in general purpose output mode
        1: GPIO pin is driven to '1' in general purpose output mode */
    BYTE GPIO_INPUT_R:        1; /* Read Only */
    /* GPIO Input Pin state, 
       used along with GPIO as general purpose input */
    BYTE GPIO_CTRL:           4;
    /* GPIO Control
        0000: GPIO input/output disabled.
        (*)0001: GPIO input is used for secondary audio interface or clock input. Configure other
        registers to choose the functionality of GPIO input.
        0010: GPIO is general purpose input
        0011: GPIO is general purpose output
        0100: GPIO output is CLKOUT
        0101: GPIO output is INT1
        0110: GPIO output is INT2
        0111: GPIO output is 0
        1000: GPIO output is secondary bit-clock for Audio Interface.
        1001: GPIO output is secondary word-clock for Audio Interface.
        1010: GPIO output is 0
        1011-1101: Reserved. Do not use.
        1110: GPIO output is DOUT for Audio Interface according to Register 53 programming.
        1111: Reserved. Do not use. */
    BYTE RSV76:               2; /* Reserved. Write only the default values */ 
  };
}TAS_GPIO_CTRL, *PTAS_GPIO_CTRL;

//4 Page 0 / Register 53: DOUT Function Control Register - 0x00 / 0x35 [b'000_1_001_0]
#define REG_TAS_DOUT_CTRL        (TAS_PAGE(0) | 53)
typedef union tagTASDOUTCTRL
{
  BYTE cData;
  struct
  {
    BYTE DOUT_OUTPUT:         1;
    /* DOUT as General Purpose Output
        (*)0: DOUT General Purpose Output Value = 0
        1: DOUT General Purpose Output Value = 1 */
    BYTE DOUT_MUX:            3;
    /* DOUT MUX Control
        000: DOUT disabled
        (*)001: DOUT disabled
        010: DOUT is General Purpose Output
        011: DOUT is CLKOUT
        100: DOUT is INT1
        101: DOUT is INT2
        110: DOUT is Secondary BCLK
        111: DOUT is Secondary WCLK */
    BYTE DOUT_BUSKEEPER_DIS:  1;
    /* DOUT Bus Keeper Control
        0: DOUT Bus Keeper Enabled
        (*)1: DOUT Bus Keeper Disabled */
    BYTE RSV75:               3; /* Reserved. Write only the default values */ 
  };
}TAS_DOUT_CTRL, *PTAS_DOUT_CTRL;

//4 Page 0 / Register 54: DIN Function Control Register - 0x00 / 0x36 [b'00000_01_X]
#define REG_TAS_DIN_CTRL        (TAS_PAGE(0) | 54)
typedef union tagTASDINCTRL
{
  BYTE cData;
  struct
  {
    BYTE DIN_INPUT_R:         1; /* Read only */
    /* Value of DIN input pin. To be used when for General Purpose Input */
    BYTE DIN_CTRL:            2;
    /* DIN function control
        00: DIN pin is disabled
        (*)01: DIN is enabled for Primary Data Input or General Purpose Clock input
        10: DIN is used as General Purpose Input
        11: Reserved. Do not use */
    BYTE RSV73:               5; /* Reserved. Write only the default values */ 
  };
}TAS_DIN_CTRL, *PTAS_DIN_CTRL;

//4 Page 0 / Register 55: MISO Function Control Register - 0x00 / 0x37 [b'000_0001_0]
#define REG_TAS_MISO_CTRL        (TAS_PAGE(0) | 55)
typedef union tagTASMISOCTRL
{
  BYTE cData;
  struct
  {
    BYTE MISO_OUTPUT:         1;
    /* Value to be driven on MISO pin when used as General Purpose Output
        (*)0: MISO General Purpose Output Value = 0
        1: MISO General Purpose Output Value = 1 */
    BYTE MISO_CTRL:           4;
    /* MISO function control
        0000: MISO buffer disabled
        (*)0001: MISO is used for data output in SPI interface, is disabled for I2C interface
        0010: MISO is General Purpose Output
        0011: MISO is CLKOUT output
        0100: MISO is INT1 output
        0101: MISO is INT2 output
        0110: Reserved
        0111: Reserved
        1000: MISO is Secondary Data Output for Audio Interface
        1001: MISO is Secondary Bit Clock for Audio Interface
        1010: MISO is Secondary Word Clock for Audio Interface
        1011-1111: Reserved. Do not use */
    BYTE RSV73:               3; /* Reserved. Write only the default values */ 
  };
}TAS_MISO_CTRL, *PTAS_MISO_CTRL;

//4 Page 0 / Register 56: SCLK/DMDIN2 Function Control Register- 0x00 / 0x38 [b'00000_01_X]
#define REG_TAS_SCLK_CTRL        (TAS_PAGE(0) | 56)
typedef union tagTASSCLKCTRL
{
  BYTE cData;
  struct
  {
    BYTE SCLK_INPUT_R:        1; /* Read Only */
    /* Value of SCLK input pin when used as General Purpose Input */
    BYTE SCLK_CTRL:           2;
    /* SCLK function control
        00: SCLK pin is disabled
        (*)01: SCLK pin is enabled for SPI clock in SPI Interface mode or when in I2C Interface
        enabled for Secondary Data Input or Secondary Bit Clock Input or Secondary Word Clock.
        10: SCLK is enabled as General Purpose Input
        11: Reserved. Do not use */
    BYTE RSV73:               5; /* Reserved. Write only the default values */ 
  };
}TAS_SCLK_CTRL, *PTAS_SCLK_CTRL;

//4 Page 0 / Register 60: DAC Instruction Set - 0x00 / 0x3C [b'000_00001]
#define REG_TAS_DAC_INSTRUCT        (TAS_PAGE(0) | 60)
typedef union tagTASDACINSTRUCT
{
  BYTE cData;
  struct
  {
    BYTE DAC_PROC_BLOCK:      5;
    /* 0 0000: Reverved
       (*)0 0001: DAC Signal Processing Block PRB_P1
       0 0010: DAC Signal Processing Block PRB_P2
       0 0011: DAC Signal Processing Block PRB_P3
       0 0100-1 1111: Reserved. Do not use */
    BYTE RSV75:               3; /* Reserved. Write only the default values */ 
  };
}TAS_DAC_INSTRUCT, *PTAS_DAC_INSTRUCT;

//4 Page 0 / Register 63: DAC Channel Setup Register 1 - 0x00 / 0x3F [b'0_0_01_01_00]
//4 Page 0 / Register 64: DAC Channel Setup Register 2 - 0x00 / 0x40 [b'0_000_1_1_00] 
#define REG_TAS_DAC_CHANNEL        (TAS_PAGE(0) | 63)
#define REG_TAS_DAC_CHANNEL1       (TAS_PAGE(0) | 63)
#define REG_TAS_DAC_CHANNEL2       (TAS_PAGE(0) | 64)
typedef union tagTASDACCHANNEL
{
  WORD wData;
  struct
  {
    /* 0 */
    BYTE VOLUME_SOFTSTEP:     2;
    /* DAC Channel Volume Control's Soft-Step control
        (*)00: Soft-Stepping is 1 step per 1 DAC Word Clock
        01: Soft-Stepping is 1 step per 2 DAC Word Clocks
        10: Soft-Stepping is disabled
        11: Reserved. Do not use */
    BYTE RSV32:               2; /* Reserved. Write only the default values */ 
    BYTE DATA_PATH:           2;
    /* DAC Data path Control
        00: DAC data is disabled
        (*)01: DAC data is picked from Left Channel Audio Interface Data
        10: DAC data is picked from Right Channel Audio Interface Data
        11: DAC data is picked from Mono Mix of Left and Right Channel Audio Interface Data */
    BYTE RSV6:                1; /* Reserved. Write only the default values */ 
    BYTE PWR_EN:              1;
    /* DAC Channel Power Control
        (*)0: DAC Channel Powered Down
        1: DAC Channel Powered Up */    

    /* 1 */
    BYTE RSV10:               2; /* Reserved. Write only the default values */ 
    BYTE MUST_BE_1:           1; /* Reserved. Write only the 1 values */ 
    BYTE MUTE:                1;
    /* DAC Channel Mute Control
        0: DAC Channel not muted
        (*)1: DAC Channel muted */
    BYTE AUTO_MUTE:           3;
    /* DAC Auto Mute Control
        (*)000: Auto Mute disabled
        001: DAC is auto muted if input data is DC for more than 100 consecutive inputs
        010: DAC is auto muted if input data is DC for more than 200 consecutive inputs
        011: DAC is auto muted if input data is DC for more than 400 consecutive inputs
        100: DAC is auto muted if input data is DC for more than 800 consecutive inputs
        101: DAC is auto muted if input data is DC for more than 1600 consecutive inputs
        110: DAC is auto muted if input data is DC for more than 3200 consecutive inputs
        111: DAC is auto muted if input data is DC for more than 6400 consecutive inputs */
    BYTE RSV7:                1; /* Reserved. Write only the default values */ 
  };
}TAS_DAC_CHANNEL, *PTAS_DAC_CHANNEL;

//4 Page 0 / Register 65: DAC Channel Digital Volume Control Register - 0x00 / 0x41 [b'0]
#define REG_TAS_DAC_CHANNEL_VOL        (TAS_PAGE(0) | 65)
typedef union tagTASDACCHANNELVOL
{
  BYTE cData;
  struct
  {
    BYTE VOLUME;
    /* DAC Channel Digital Volume Control Setting
        0111 1111-0011 0001: Reserved. Do not use
        0011 0000: Digital Volume Control = +24dB
        0010 1111: Digital Volume Control = +23.5dB
        …
        0000 0001: Digital Volume Control = +0.5dB
        (*)0000 0000: Digital Volume Control = 0.0dB
        1111 1111: Digital Volume Control = -0.5dB
        ...
        1000 0010: Digital Volume Control = -63dB
        1000 0001: Digital Volume Control = -63.5dB
        1000 0000: Reserved. Do not use */
  };
}TAS_DAC_CHANNEL_VOL, *PTAS_DAC_CHANNEL_VOL;


// ---------------------------------------------------------------------------
// PAGE 1 :  DAC Routing, Power-Controls 
//           and MISC logic Releated Programmabilities.
// --------------------------------------------------------------------------- 
//4 Page 1 / Register 0: Page Select Register - 0x01 / 0x00 [b'0]
///#define REG_TAS_PAGE        (TAS_PAGE(1) | 0)

//4 Page 1 / Registers 1: REF, POR and LDO BGAP Control Register - 0x01 / 0x01 [b'0]
#define REG_TAS_REFPWR_CTRL        (TAS_PAGE(1) | 1)
typedef union tagTASREFPWRCTRL
{
  BYTE cData;
  struct
  {
    BYTE RSV0:                1; /* Reserved. Write only the default values */   
    BYTE LDOBGAP_PWR:         1;
    /* LDO bandgap power control
        (*)0: LDO bandgap not powered down
        1: LDO bandgap powered down */
    BYTE RSV2:                1; /* Reserved. Write only the default values */   
    BYTE POR_PWR:             1;
    /* POR power control
        (*)0: Do not power down the POR circuit
        1: Power down the POR circuit */
    BYTE MASTER_REF_PWR:      1;
    /* Master Reference control
        (*)0: Master Reference powered down
        1: Master Reference enabled */
    BYTE RSV75:               3; /* Reserved. Write only the default values */   
  };
}TAS_REFPWR_CTRL, *PTAS_REFPWR_CTRL;

//4 Page 1 / Register 2: LDO Control Register - 0x01 / 0x02 [b'00_00_1_1_0_0]
#define REG_TAS_LDO_CTRL        (TAS_PAGE(1) | 2)
typedef union tagTASLDOCTRL
{
  BYTE cData;
  struct
  {
    BYTE LDO_SEL_PIN:         1;
    /* LDO Select
        (*)0: LDO_SEL pin is low
        1: LDO_SEL pin is high */
    BYTE SHORT_DETECT:        1;
    /* Short Circuit detect control
        (*)0: No Short Circuit detected in AVDD LDO
        1: Short Circuit detected in AVDD LDO */  
    BYTE MUST_BE_1:           1; /* Reserved. Write only the default values */   
    BYTE PLL_HP_LS_PWR:       1;
    /*  0: PLL and HP Level Shifters powered up
        (*)1: PLL and HP Level Shifters powered down.
        This is to save leakage current issue when DVDD is powered up and AVDD is powered down */
    BYTE AVDD_LDO_CTRL:       2;
    /* AVDD LDO Control
        (*)00: AVDD LDO output is nominally 1.8V
        01: AVDD LDO output is nominally 1.6V
        10: AVDD LDO output is nominally 1.7V
        11: AVDD LDO output is nominally 1.5V */
    BYTE RSV76:               2; /* Reserved. Write only the default values */   
  };
}TAS_LDO_CTRL, *PTAS_LDO_CTRL;

//4 Page 1 / Register 3:  Playback Configuration Register 1 - 0x01 / 0x03 [b'0] 
#define REG_TAS_PLAYBACK        (TAS_PAGE(1) | 3)
typedef union tagTASPLAYBACK
{
  BYTE cData;
  struct
  {
    BYTE RSV10:               2; /* Reserved. Write only the default values */   
    BYTE DAC_PTM:             3;
    /* DAC PTM Control
        (*)000: DAC in mode PTM_P3, PTM_P4
        001: DAC in mode PTM_P2
        010: DAC in mode PTM_P1
        011-111: Reserved. Do not use */
    BYTE DAC_ENABLE_MODE:     1;
    /* (*)0: DAC is enabled with low power mode
       1: DAC is enabled with high performance mode */
    BYTE RSV76:               2; /* Reserved. Write only the default values */   
  };
}TAS_PLAYBACK, *PTAS_PLAYBACK;

//4 Page 1 / Register 8: DAC PGA Control Register- 0x01 / 0x08 [b'0]
#define REG_TAS_DAC_PAG        (TAS_PAGE(1) | 8)
typedef union tagTASDACPGA
{
  BYTE cData;
  struct
  {
    BYTE RSV50:               6; /* Reserved. Write only the default values */   
    BYTE MODE:                1;
    /* (*)0: normal mode.
       1: Soft-stepping time for all the PGA of DAC channel are doubled. */
    BYTE SOFTSTEP_ENABLE:     1;
    /* (*)0: Soft-stepping of all the PGA are enabled for DAC channel.
       1: Soft-stepping of all the PGA are disabled for DAC channel. */
  };
}TAS_DAC_PGA, *PTAS_DAC_PGA;

//4 Page 1 / Register 9: Output Drivers, AINL, AINR, Control Register - 0x01 / 0x09 [b'0] 
#define REG_TAS_HPOUT_AIN        (TAS_PAGE(1) | 9)
typedef union tagTASHPOUTAIN
{
  BYTE cData;
  struct
  {
    BYTE AINR_INPUT_EN:       1;
    /* (*)0: AINR input is disabled
       1: AINR input is enabled */
    BYTE AINL_INPUT_EN:       1;
    /* (*)0: AINL input is disabled
       1: AINL input is enabled */
    BYTE RSV42:               3;
    BYTE HPOUT_PWR:           1;
    /* (*)0: HPL output is powered down
       1: HPL output is powered up */
    BYTE RSV7:                2; /* Reserved. Write only the default values */
  };
}TAS_HPOUT_AIN, *PTAS_HPOUT_AIN;

//4 Page 1 / Register 10: Common Mode Control Register - 0x01 / 0x0A [b'0]
#define REG_TAS_COMM_MODE        (TAS_PAGE(1) | 10)
typedef union tagTASCOMMMODE
{
  BYTE cData;
  struct
  {
    BYTE RSV10:               2; /* Reserved. Write only the default values */
    BYTE HPOUT_DRIVE:         1;  
    /* (*)0: Headphone output full drive ability
       1: Headphone output half drive ability */
    BYTE RSV35:               3; /* Reserved. Write only the default values */
    BYTE FULLCHIPMODE:        1;
    /* (*)0: Full Chip Common Mode is 0.9V
       1: Full Chip Common Mode is 0.75V */
    BYTE RSV7:                1; /* Reserved. Write only the default values */
  };
}TAS_COMM_MODE, *PTAS_COMM_MODE;

//4 Page 1 / Register 11: HP Over Current Protection Configuration Register - 0x01 / 0x0B [b'000_1_000_0]
#define REG_TAS_HP_OCP        (TAS_PAGE(1) | 11)
typedef union tagTASHPOCP
{
  BYTE cData;
  struct
  {
    BYTE DRIVE:               1;
    /* (*)0: Output current will be limited if over current condition is detected
       1: Output driver will be powered down if over current condition is detected */
    BYTE DETECT:              3;
    /* (*)000: No debounce is used for Over Current detection
       001: Over Current detection is debounced by 8ms
       010: Over Current detection is debounce by 16ms
       011: Over Current detection is debounced by 32ms
       100: Over Current detection is debounced by 64ms
       101: Over Current detection is debounced by 128ms
       110: Over Current detection is debounced by 256ms
       111: Over Current detection is debounced by 512ms */
    BYTE MUST_BE_1:           1; /* Reserved. Write only the default values */
    BYTE RSV57:               3; /* Reserved. Write only the default values */
  };
}TAS_HP_OCP, *PTAS_HP_OCP;

//4 Page 1 / Register 12: HP Routing Selection Register - 0x01 / 0x0C [b'0]
#define REG_TAS_HP_ROUTE        (TAS_PAGE(1) | 12)
typedef union tagTASHPROUTE
{
  BYTE cData;
  struct
  {
    BYTE AINR_ATTENU:         1;
    /* (*)0: AINR attenuator is not routed to HP driver.
       1: AINR attenuator is routed to HP driver. */
    BYTE AINL_ATTENU:         1;
    /* (*)0: AINL attenuator is not routed to HP driver.
       1: AINL attenuator is routed to HP driver. */
    BYTE MIXER_P_CONNECT:     1;
    /* (*)0: Mixer P is not connected to HP attenuator
       1:Mixer P is connected to HP attenuator */
    BYTE DAC_OUTPUT_ROUTE:    1;
    /* (*)0: DAC output is not routed directly to HP driver.
       1: DAC output is routed directly to HP driver. */
    BYTE INOUT_ROUTE:         4;
    /* (*)0000: No analog routing to SPK driver and HP driver
       0001 - 0011 : Do not use
       0100: AINR routed to Mixer P
       0101: Do not use
       0110: AINL/R differential routed to SPK driver through Mixer A and Mixer B
       0111: Do no use
       1000: AINL routed to Mixer A
       1001: AINL/R differential routed to SPK driver through Mixer A and Mixer B
       1010 -1011: Do not use
       1100: AINL and AINR routed to Mixer A to HP driver
       1101 - 1111: Do not use */
  };
}TAS_HP_ROUTE, *PTAS_HP_ROUTE;

//4 Page 1 / Register 16: HP Driver Gain Setting Register - 0x01 / 0x10 [b'0_1_000000]
#define REG_TAS_HP_GAIN        (TAS_PAGE(1) | 16)
typedef union tagTASHPDRVGAIN
{
  BYTE cData;
  struct
  {
    BYTE GAIN:                6;
    /* 10 0000 - 11 1001: Reserved. Do not use
       11 1010: HP driver gain is -6dB (Note: It is not possible to mute HPR while programmed to -6dB)
       11 1011: HP driver gain is -5dB 11 1100: HP driver gain is -4dB
       11 1101: HP driver gain is -3dB
       …
       (*)00 0000: HP driver gain is 0dB
       ...
       00 0011: HP driver gain is 3dB
       00 0100: HP driver gain is 4dB
       00 0101: Hp driver gain is 5dB
       00 0110: HP driver gain is 6dB
       …
       00 1100: HP driver gain is 12dB
       …
       01 0010: HP driver gain is 18dB
       …
       01 1000: HP driver gain is 24dB
       …
       01 1100: HP driver gain is 28dB
       01 1101: HP driver gain is 29dB
       ...
       00 1110 - 01 1111: Reserved. Do not use */
    BYTE MUTE:                1;
    /* 0: HP driver is not muted
       (*)1: HP driver is muted */
    BYTE RSV7:                1; /* Reserved. Write only the default values */
  };
}TAS_HP_GAIN, *PTAS_HP_GAIN;

//4 Page 1 / Registers 20: Headphone Driver Startup Control Register - 0x01 / 0x14 [b'0]
#define REG_TAS_HP_STARTUP        (TAS_PAGE(1) | 20)
typedef union tagTASHPSTARTUP
{
  BYTE cData;
  struct
  {
    BYTE SOFTROUTING_TIME:      2;
    /* (*)00: Soft-routing step time = 0ms
       01: Soft-routing step time = 50ms
       10: Soft-routing step time = 100ms
       11: Soft-routing step time = 200ms */
    BYTE HPAMP_PWRUPTIME:       4;
    /* (*)0000: Slow power up of headphone amp's is disabled
       0001: Headphone amps power up slowly in 0.5 time constants
       0010: Headphone amps power up slowly in 0.625 time constants
       0011; Headphone amps power up slowly in 0.725 time constants
       0100: Headphone amps power up slowly in 0.875 time constants
       0101: Headphone amps power up slowly in 1.0 time constants
       0110: Headphone amps power up slowly in 2.0 time constants
       0111: Headphone amps power up slowly in 3.0 time constants
       1000: Headphone amps power up slowly in 4.0 time constants
       1001: Headphone amps power up slowly in 5.0 time constants
       1010: Headphone amps power up slowly in 6.0 time constants
       1011: Headphone amps power up slowly in 7.0 time constants
       1100: Headphone amps power up slowly in 8.0 time constants
       1101: Headphone amps power up slowly in 16.0 time constants ( do not use for Rchg=25K)
       1110: Headphone amps power up slowly in 24.0 time constants (do not use for Rchg=25K)
       1111: Headphone amps power up slowly in 32.0 time constants (do not use for Rchg=25K)
       Note: Time constants assume 47µF decoupling cap */
    BYTE HPAMP_PWRUPTIME_LOAD:  2;
    /* (*)00: Headphone amps power up time is determined with 25K resistance
       01: Headphone amps power up time is determined with 6K resistance
       10: Headphone amps power up time is determined with 2K resistance
       11: Reserved. Do not use */
  };
}TAS_HP_STARTUP, *PTAS_HP_STARTUP;

//4 Page 1 / Register 22: HP Volume Control Register - 0x01 / 0x16 [b'0]
#define REG_TAS_HP_VOLUME        (TAS_PAGE(1) | 22)
typedef union tagTASHPVOLUME
{
  BYTE cData;
  struct
  {  
    BYTE VOLUME:              7;
    /* HP Volume Control
        (*)000 0000: Volume Control = 0.0dB
        000 0001: Volume Control = -0.5dB
        000 0010: Volume Control = -1.0dB
        000 0011: Volume Control = -1.5dB
        000 0100: Volume Control = -2.0dB
        ...      
        110 1101: Volume Control = -56.7dB
        110 1110: Volume Control = -58.3dB
        110 1111: Volume Control = -60.1dB
        111 0000: Volume Control = -62.7dB
        111 0001: Volume Control = -64.3dB
        111 0010: Volume Control = -66.2dB
        111 0011: Volume Control = -66.7dB
        111 0100: Volume Control = -72.3dB
        111 0101: Volume Control = Mute
        111 0110-111 1111: Reserved. Do not use */  
    BYTE RSV7:                1; /* Reserved. Write only the default values */
  };
}TAS_HP_VOLUME, *PTAS_HP_VOLUME;

//4 Page 1 / Register 24: AINL Volume Control Register - 0x01 / 0x18 [b'0]
#define REG_TAS_AINL_VOLUME        (TAS_PAGE(1) | 24)
typedef union tagTASAINLVOLUME
{
  BYTE cData;
  struct
  {    
    BYTE VOLUME:              7;
    /* AINL Volume Control
        (*)000 0000: Volume Control = 0.0dB
        000 0001: Volume Control = -0.5dB
        000 0010: Volume Control = -1.0dB
        000 0011: Volume Control = -1.5dB
        000 0100: Volume Control = -2.0dB
        ...      
        110 1101: Volume Control = -56.7dB
        110 1110: Volume Control = -58.3dB
        110 1111: Volume Control = -60.1dB
        111 0000: Volume Control = -62.7dB
        111 0001: Volume Control = -64.3dB
        111 0010: Volume Control = -66.2dB
        111 0011: Volume Control = -66.7dB
        111 0100: Volume Control = -72.3dB
        111 0101: Volume Control = Mute
        111 0110-111 1111: Reserved. Do not use */  
    BYTE MIXER_P_M_EN:         1;
    /* (*)0: Mixer P and Mixer M not forcedly enabled
       1: Mixer P and Mixer M forcedly enabled
        Note: This a bit need to set "1" when not powered-on DAC and need to route AINL or AINR
        signal input to HP driver and SPK driver via Mixer P. */
  };
}TAS_AINL_VOLUME, *PTAS_AINL_VOLUME;

//4 Page 1 / Register 25: AINR Volume Control Register - 0x01 / 0x19 [b'0]
#define REG_TAS_AINR_VOLUME        (TAS_PAGE(1) | 25)
typedef union tagTASAINRVOLUME
{
  BYTE cData;
  struct
  {
    BYTE VOLUME:              7;
    /* AINR Volume Control
        (*)000 0000: Volume Control = 0.0dB
        000 0001: Volume Control = -0.5dB
        000 0010: Volume Control = -1.0dB
        000 0011: Volume Control = -1.5dB
        000 0100: Volume Control = -2.0dB
        ...      
        110 1101: Volume Control = -56.7dB
        110 1110: Volume Control = -58.3dB
        110 1111: Volume Control = -60.1dB
        111 0000: Volume Control = -62.7dB
        111 0001: Volume Control = -64.3dB
        111 0010: Volume Control = -66.2dB
        111 0011: Volume Control = -66.7dB
        111 0100: Volume Control = -72.3dB
        111 0101: Volume Control = Mute
        111 0110-111 1111: Reserved. Do not use */  
    BYTE RSV7:                1; /* Reserved. Write only the default values */
  };
}TAS_AINR_VOLUME, *PTAS_AINR_VOLUME;

//4 Page 1 / Register 45: Speaker Amplifier Control 1 - 0x01 / 0x2D [b'0]
#define REG_TAS_SPK_AMP        (TAS_PAGE(1) | 45)
typedef union tagTASSPKAMP
{
  BYTE cData;
  struct
  {
    BYTE RSV0:                  1; /* Reserved. Write only the default values */
    BYTE DRV_PWR:               1;
    /* Speaker Driver Power
        (*)0: SPK output driver is powered down
        1: SPK output driver is powered up */ 
    BYTE RSV72:                 6; /* Reserved. Write only the default values */
  };
}TAS_SPK_AMP, *PTAS_SPK_AMP;

//4 Page 1 / Register 46: Speaker Volume Control 1 - 0x01 / 0x2E [b'0]
#define REG_TAS_SPK_VOLUME        (TAS_PAGE(1) | 46)
typedef union tagTASSPKVOLUME
{
  BYTE cData;
  struct
  {  
    BYTE VOLUME:              7;
    /* SPK Volume Control
        (*)000 0000: Volume Control = 0.0dB
        000 0001: Volume Control = -0.5dB
        000 0010: Volume Control = -1.0dB
        000 0011: Volume Control = -1.5dB
        000 0100: Volume Control = -2.0dB
        ...      
        110 1101: Volume Control = -56.7dB
        110 1110: Volume Control = -58.3dB
        110 1111: Volume Control = -60.1dB
        111 0000: Volume Control = -62.7dB
        111 0001: Volume Control = -64.3dB
        111 0010: Volume Control = -66.2dB
        111 0011: Volume Control = -66.7dB
        111 0100: Volume Control = -72.3dB
        111 0101: Volume Control = Mute      
        111 0101 - 1111110: Reserved
        111 1111: Mute */  
    BYTE RSV7:                1; /* Reserved. Write only the default values */
  };
}TAS_SPK_VOLUME, *PTAS_SPK_VOLUME;

//4 Page 1 / Register 48: Speaker Amplifier Volume Control 2 - 0x01 / 0x30 [b'0]
#define REG_TAS_SPK_AMPVOLUME        (TAS_PAGE(1) | 48)
typedef union tagTASSPKAMPVOLUME
{
  BYTE cData;
  struct
  {
    BYTE RSV30:               4; /* Reserved. Write only the default values */
    BYTE VOLUME:              3;
    /* Left Speaker Amplifier (SPK) Volume Control:
        (*)000: SPK Driver is Muted (Default)
        001: SPK Driver Volume = 6 dB
        010: SPK Driver Volume = 12 dB
        011: SPK Driver Volume = 18 dB
        100: SPK Driver Volume = 24 dB
        101: SPK Driver Volume = 32 dB
        110 - 111: Reserved */  
    BYTE RSV7:                1; /* Reserved. Write only the default values */
  };
}TAS_SPK_AMPVOLUME, *PTAS_SPK_AMPVOLUME;

//4 Page 1 / Register 63: DAC Analog Gain Control Flag Register - 0x01 / 0x3F [b'0]
#define REG_TAS_DAC_AGCFLAG        (TAS_PAGE(1) | 63)
typedef union tagTASDACAGCFLAG
{
  BYTE cData;
  struct
  {
    BYTE RIGHT_MIXER:         1;
    /* Right Mixer PGA for AINR Volume Flag
        (*)0: Applied Volume is not equal to Programmed Volume
        1: Applied Volume is equal to Programmed Volume */
    BYTE LEFT_MIXER:          1;
    /* Left Mixer PGA for AINL Volume Flag
        (*)0: Applied Volume is not equal to Programmed Volume
        1: Applied Volume is equal to Programmed Volume */
    BYTE RSV2:                1; /* Reserved. Write only the default values */        
    BYTE AIN1L_PAG:           1;
    /* AIN1L Mix PGA for HP Applied Gain Flag
        (*)0: Applied Volume is not equal to Programmed Volume
        1: Applied Volume is equal to Programmed Volume */        
    BYTE RSV64:               3; /* Reserved. Write only the default values */        
    BYTE HP_GAIN:             1;
    /* HP Gain Flag
        (*)0: Applied Gain is not equal to Programmed Gain
        1: Applied Gain is equal to Programmed Gain */        
  };
}TAS_DAC_AGCFLAG, *PTAS_DAC_AGCFLAG;

//4 Page 1 / Register 122: Reference Power Up Delay - 0x01 / 0x7A [b'0]
#define REG_TAS_PWRUP_DELAY        (TAS_PAGE(1) | 122)
typedef union tagTASPWRUPDELAY
{
  BYTE cData;
  struct
  {
    BYTE PWRUP_CONF:          3;
    /* Reference Power Up configuration
        (*)000: Reference will power up slowly when analog blocks are powered up
        001: Reference will power up in 40ms when analog blocks are powered up
        010: Reference will power up in 80ms when analog blocks are powered up
        011: Reference will power up in 120ms when analog blocks are powered up
        100: Force power up of reference. Power up will be slow
        101: Force power up of reference. Power up time will be 40ms
        110: Force power up of reference. Power up time will be 80ms
        111: Force power up of reference. Power up time will be 120ms */
    BYTE RSV73:               5; /* Reserved. Write only the default values */
  };
}TAS_PWRUP_DELAY, *PTAS_PWRUP_DELAY;


/**
  * @}
  */


/** @defgroup dvTAS2505_Exported_constants 
  * @{
  */


/**
  * @}
  */

/** @defgroup dvTAS2505_Interrupt_Service_Routine
  * @{
  */

/**
  * @}
  */


/** @defgroup dvTAS2505_Exported_Functions
  * @{
  */


/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */

#endif // #if (USE_LCD_1024x600 ||  USE_LCD_1280x800)

#endif // #if !defined(_INCLUDED_DVTAS2505REGS_H_)  

/******************* (C) COPYRIGHT 1997-2019 (c) MadeFactory Inc. *****END OF FILE****/  
