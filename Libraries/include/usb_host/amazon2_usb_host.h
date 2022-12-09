#ifndef __GMX2K_H__
#define __GMX2K_H__

#define CONFIG_USB_STORAGE
#define CONFIG_USB_OHCI
#define CONFIG_DOS_PARTITION

//#define CLK_SRC     (25000000)
//#define CLK_SRC     (25175000)
//#define CLK_SRC     (28322000)
//#define CLK_SRC     (36861000)
//#define CLK_SRC     (48000000)
//#define CLK_SRC     (51840000)
//#define CLK_SRC     (100000000)
//#define BUS_CLK     (CLK_SRC/2)

typedef signed char __s8;
//typedef unsigned char __u8;

typedef signed short __s16;
//typedef unsigned short __u16;

typedef signed int __s32;
//typedef unsigned int __u32;

typedef signed long long __s64;
//typedef unsigned long long __u64;

//typedef unsigned char boolean;

//typedef signed char s8;
//typedef unsigned char u8;

//typedef signed short s16;
//typedef unsigned short u16;

//typedef signed int s32;
//typedef unsigned int u32;

//typedef signed long long s64;
//typedef unsigned long long u64;

//typedef unsigned char uchar;
//typedef unsigned short ushort;
//typedef unsigned long ulong;

//typedef unsigned int	size_t;

/* MACRO for read/write a memory */
#define __raw_readb(a) *((volatile __u8 *)(a))
#define __raw_readw(a) *((volatile __u16 *)(a))
#define __raw_readl(a) *((volatile __u32 *)(a))

#define __raw_writeb(d, a) *((volatile __u8  *)(a))=(__u8)(d)
#define __raw_writew(d, a) *((volatile __u16 *)(a))=(__u16)(d)
#define __raw_writel(d, a) *((volatile __u32 *)(a))=(__u32)(d)

#define gmx2k_readb(off)		__raw_readb((off))
#define gmx2k_readw(off)		__raw_readw((off))
#define gmx2k_readl(off)		__raw_readl((off))

#define gmx2k_writeb(val, off)	__raw_writeb(val, (off))
#define gmx2k_writew(val, off)	__raw_writew(val, (off))
#define gmx2k_writel(val, off)	__raw_writel(val, (off))

#ifndef	 FALSE
#define	 FALSE		0
#endif
#ifndef	 TRUE
#define	 TRUE		1
#endif
#ifndef	 NULL
#define	 NULL		0
#endif

#endif // __EAGLE_H__

