
#ifndef _EXAMPLEBASE_H_
#define _EXAMPLEBASE_H_

#include "../common/defs.h"



class adxM3_Example
{
public:

	static ErrorStatus BufferCompare(const void* _pBuffer1, const void* _pBuffer2, unsigned int BufferLength)
	{
		const UINT8* pBuffer1 = static_cast<const UINT8*>(_pBuffer1);
		const UINT8* pBuffer2 = static_cast<const UINT8*>(_pBuffer2);

		while (BufferLength--)
		{
			if (*pBuffer1 != *pBuffer2)
			{
				return ERROR;
			}
			++pBuffer1;
			++pBuffer2;
		}
		return SUCCESS;
	}

	static const unsigned int BUFSIZE = 100;
	static UINT16 TXBUF1[BUFSIZE];
	static UINT16 TXBUF2[BUFSIZE];
	static UINT16 RXBUF1[BUFSIZE];
	static UINT16 RXBUF2[BUFSIZE];
	static volatile unsigned int TXIDX1, TXIDX2, RXIDX1, RXIDX2;
	static volatile UINT16 ParityWord1, ParityWord2;
};


#endif  /*_EXAMPLEBASE_H_*/

