#include "sdk.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Readfiles the given file. </summary>
///
/// <remarks>	Kkh, 2013-04-02. </remarks>
///
/// <param name="fname">	[in] filename to read. </param>
/// <param name="len">  	[out] length of read. </param>
///
/// <returns>	null if it fails, else return memory buffer allocated by malloc </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

void* readfile(char* fname,int* len)
{
	FIL fp;
	int size;
	char* buf;
	U32 nRead;
	FRESULT res = f_open(&fp,fname,FA_READ|FA_OPEN_EXISTING);
	if( res != FR_OK )
	{
		DEBUGPRINTF("Cannot open : %s\r\n", fname);
		return 0;
	}
	size = f_size(&fp);
	buf=malloc(size);
	if(buf==0)
		return 0;
	f_read(&fp,buf,size,&nRead);
	f_close(&fp);
	*len=size;
	return buf;
}
