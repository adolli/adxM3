/*************************************************************************
PURPOSE:
	in this head file, some key words are defined as macro. using the macro
	directly in your programme to make work more effiency.
	该文件仅适用于c++编译环境

*************************************************************************/

#ifndef _DEFS_H_
#define _DEFS_H_

#include <stdint.h>





/* 类成员函数行为修饰符
 * @modifier	__MEM_CONST_ 不改变内存，但可能改变寄存器
 * @modifier	__REG_CONST_ 不改变寄存器，也不改变内存
 */
#define __MEM_CONST_	const
#define	__REG_CONST_	const

// typedefs

// 匈牙利式类型定义
typedef unsigned char		Digit;
typedef unsigned char		BYTE;
typedef unsigned short		HWORD;
typedef unsigned int		WORD;
typedef unsigned long long	DWORD;
typedef unsigned int		UINT;
typedef int					INT;
typedef float				FLOAT;
typedef double				DOUBLE;
typedef void				VOID, *LPVOID, *MEMADDR;
typedef void				*PRESULT;


// stdint类型定义
typedef uint8_t				UINT8;
typedef uint16_t			UINT16;
typedef uint32_t			UINT32;
typedef uint64_t			UINT64;

typedef int8_t				INT8;
typedef int16_t				INT16;
typedef int32_t				INT32;
typedef int64_t				INT64;

// adxM3类型定义
typedef	UINT8				PinIndex;
typedef	UINT32				PinPos, PinMask;
typedef INT8				Coord;
typedef UINT32				BIT;						// BitBand操作的位类型定义
typedef UINT32				AddressType;


// 函数调用方式说明字
#define __INIT__
#define CALLBACK
#define ARMAPI				extern "C"


// 其他提示性关键字
#define __user_entry__


/**
 * IO definitions
 *
 * define access restrictions to peripheral registers
 */

#ifdef __cplusplus
  #define     __I     volatile                /*!< defines 'read only' permissions      */
#else
  #define     __I     volatile const          /*!< defines 'read only' permissions      */
#endif
#define     __O     volatile                  /*!< defines 'write only' permissions     */
#define     __IO    volatile                  /*!< defines 'read / write' permissions   */


/*
 * 标志或状态枚举类型定义
 */
enum FlagStatus {RESET = 0, SET = !RESET};

enum FunctionalState {DISABLE = 0, ENABLE = !DISABLE};

enum ErrorStatus {ERROR = 0, SUCCESS = !ERROR};



// boolean constant defs
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef NULL
#define NULL (void*)0
#endif

// varible manipulator
#define MAKEBYTE(hi, lo)	((((BYTE)((hi) & 0xf)) << 4) | (((BYTE)((lo) & 0xf))))
#define MAKEWORD(hi, lo)    ((WORD)(((BYTE)((lo) & 0xff)) | ((WORD)((BYTE)((hi) & 0xff))) << 8))
#define MAKELONG(hi, lo)    ((LONG)(((WORD)((lo) & 0xffff)) | ((DWORD)((WORD)((hi) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)((DWORD)(l) & 0xffff))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xffff))
#define LOBYTE(w)           ((BYTE)((WORD)(w) & 0xff))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xff))
#define LODIGIT(b)			((BYTE)(b) & 0xf)
#define HIDIGIT(b)			(((BYTE)(b) >> 4) & 0xf)

// 将LPVOID类型转换成对应的数据类型读取
#define GETUINT8(lp)		(*(UINT8*)(lp))
#define GETUINT16(lp)		(*(UINT16*)(lp))
#define GETUINT32(lp)		(*(UINT32*)(lp))
#define GETINT8(lp)			(*(INT8*)(lp))
#define GETINT16(lp)		(*(INT16*)(lp))
#define GETINT32(lp)		(*(INT32*)(lp))

// 将MEMADDR类型转换成别的指针
#define	TOBYTE(pMem)		((BYTE*)(pMem))
#define	TOWORD(pMem)		((WORD*)(pMem))
#define	TODWORD(pMem)		((DWORD*)(pMem))

// bit manipulator
#define _BV(n)				((UINT32)(1L << (n)))
#define _BF(a, b)			((_BV((a) + 1) - 1) & ~(_BV(b) - 1))


// 循环移位函数
// _cxxx_ 字节型	_sxxx_ 16位整数		_ixxx_ 32位整型		_lxxx_ 32位整型
// _xxxl_ 左移		_xxxr_ 右移
#define _crol_(byte,nbit)	(((byte)<<(nbit))|((byte)>>( 8-(nbit))))
#define _cror_(byte,nbit)	(((byte)>>(nbit))|((byte)<<( 8-(nbit))))
#define _srol_(word,nbit)	(((word)<<(nbit))|((word)>>(16-(nbit))))
#define _sror_(word,nbit)	(((word)>>(nbit))|((word)<<(16-(nbit))))
#define _irol_(uint,nbit)	(((uint)<<(nbit))|((uint)>>(32-(nbit))))
#define _iror_(uint,nbit)	(((uint)>>(nbit))|((uint)<<(32-(nbit))))
#define _lrol_(dword,nbit)	(((dword)<<(nbit))|((dword)>>(32-(nbit))))
#define _lror_(dword,nbit)	(((dword)>>(nbit))|((dword)<<(32-(nbit))))

#define _nop_()				__asm("NOP \n\t")





// 成员函数特殊说明字（仅仅注释作用）

// 重载函数
#define OVERLOAD
// 重写函数
#define OVERWRITE





union WordStruct
{
	WORD word;
	HWORD hword[2];
	BYTE byte[4];
};





#endif  /*_DEFS_H_*/
