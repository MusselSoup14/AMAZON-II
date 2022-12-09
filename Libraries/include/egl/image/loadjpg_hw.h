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

BOOL jpeg_hw_decode_raw(U32 imageaddr,U32 len,U32 outputbuf,U32 lpitch,U32 bpp);
SURFACE* loadjpg_hw(char* filename);
SURFACE* loadjpgp_hw(U32 imageaddr,U32 filesize);
BOOL drawjpg_hw(char *filename, SURFACE* dst, int x, int y);
BOOL drawjpgp_hw(void* imgbuf, U32 filesize, SURFACE* dst, int x, int y);