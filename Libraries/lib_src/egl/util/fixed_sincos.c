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
#include "egl_typedef.h"
#define SIZE_OPT

/* integer sin/cos tables*/
#ifdef SIZE_OPT
static short isin[91] =
#else
static short isin[360] =
#endif
{
	0, 17, 35, 53, 71, 89, 107, 124, 142, 160,
	177, 195, 212, 230, 247, 265, 282, 299, 316, 333,
	350, 366, 383, 400, 416, 432, 448, 464, 480, 496,
	512, 527, 542, 557, 572, 587, 601, 616, 630, 644,
	658, 671, 685, 698, 711, 724, 736, 748, 760, 772,
	784, 795, 806, 817, 828, 838, 848, 858, 868, 877,
	886, 895, 904, 912, 920, 928, 935, 942, 949, 955,
	962, 968, 973, 979, 984, 989, 993, 997, 1001, 1005,
	1008, 1011, 1014, 1016, 1018, 1020, 1021, 1022, 1023, 1023,
	1024
#ifdef SIZE_OPT
};
#else
		, 1023, 1023, 1022, 1021, 1020, 1018, 1016, 1014, 1011,
	1008, 1005, 1001, 997, 993, 989, 984, 979, 973, 968,
	962, 955, 949, 942, 935, 928, 920, 912, 904, 895,
	886, 877, 868, 858, 848, 838, 828, 817, 806, 795,
	784, 772, 760, 748, 736, 724, 711, 698, 685, 671,
	658, 644, 630, 616, 601, 587, 572, 557, 542, 527,
	512, 496, 480, 464, 448, 432, 416, 400, 383, 366,
	350, 333, 316, 299, 282, 265, 247, 230, 212, 195,
	177, 160, 142, 124, 107, 89, 71, 53, 35, 17,
	0, -17, -35, -53, -71, -89, -107, -124, -142, -160,
	-177, -195, -212, -230, -247, -265, -282, -299, -316, -333,
	-350, -366, -383, -400, -416, -432, -448, -464, -480, -496,
	-512, -527, -542, -557, -572, -587, -601, -616, -630, -644,
	-658, -671, -685, -698, -711, -724, -736, -748, -760, -772,
	-784, -795, -806, -817, -828, -838, -848, -858, -868, -877,
	-886, -895, -904, -912, -920, -928, -935, -942, -949, -955,
	-962, -968, -973, -979, -984, -989, -993, -997, -1001, -1005,
	-1008, -1011, -1014, -1016, -1018, -1020, -1021, -1022, -1023, -1023,
	-1024, -1023, -1023, -1022, -1021, -1020, -1018, -1016, -1014, -1011,
	-1008, -1005, -1001, -997, -993, -989, -984, -979, -973, -968,
	-962, -955, -949, -942, -935, -928, -920, -912, -904, -895,
	-886, -877, -868, -858, -848, -838, -828, -817, -806, -795,
	-784, -772, -760, -748, -736, -724, -711, -698, -685, -671,
	-658, -644, -630, -616, -601, -587, -572, -557, -542, -527,
	-512, -496, -480, -464, -448, -432, -416, -400, -383, -366,
	-350, -333, -316, -299, -282, -265, -247, -230, -212, -195,
	-177, -160, -142, -124, -107, -89, -71, -53, -35, -17
};
#endif

//return (int)(float * 1024)
int egl_isin(int angle)
{
	angle %=360;
	if(angle<0)
		angle+=360;
#ifdef SIZE_OPT
	if(angle>270)
	{
		angle-=270;
		return -isin[90-angle];
	}
	else if(angle>180)
	{
		angle-=180;
		return -isin[angle];
	}
	else if(angle>90)
	{
		angle-=90;
		return isin[90-angle];
	}
	return isin[angle];
	
#else
	return isin[angle];
#endif
}
//return (int)(float * 1024)
int egl_icos(int angle)
{
#ifdef SIZE_OPT
	angle %=360;
	if(angle<0)
		angle+=360;
	if(angle>270)
	{
		angle-=270;
		return isin[angle];
	}
	else if(angle>180)
	{
		angle-=180;
		return -isin[90-angle];
	}
	else if(angle>90)
	{
		angle-=90;
		return -isin[angle];
	}
	return isin[90-angle];
#else
	angle+=90;
	angle %=360;
	if(angle<0)
		angle+=360;
	return isin[angle];
#endif
}
