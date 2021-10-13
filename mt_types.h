#ifndef __MT_TYPES_H__
#define __MT_TYPES_H__

//===========================================================================
// I02: INCLUDE FILES
//===========================================================================

//===========================================================================
// I03: CONSTANT DEFINITIONS
//===========================================================================

#ifndef FALSE
    #define FALSE 0
#endif

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef OFF
    #define OFF 0
#endif

#ifndef ON
    #define ON 1
#endif

#ifndef NULL
    #define NULL    ((void *)0x00)
#endif

#define BIT0		0x00000001
#define BIT1		0x00000002
#define BIT2		0x00000004
#define BIT3		0x00000008
#define BIT4		0x00000010
#define BIT5		0x00000020
#define BIT6		0x00000040
#define BIT7		0x00000080
#define BIT8		0x00000100
#define BIT9		0x00000200
#define BIT10		0x00000400
#define BIT11		0x00000800
#define BIT12		0x00001000
#define BIT13		0x00002000
#define BIT14		0x00004000
#define BIT15		0x00008000
#define BIT16		0x00010000
#define BIT17		0x00020000
#define BIT18		0x00040000
#define BIT19		0x00080000
#define BIT20		0x00100000
#define BIT21		0x00200000
#define BIT22		0x00400000
#define BIT23		0x00800000
#define BIT24		0x01000000
#define BIT25		0x02000000
#define BIT26		0x04000000
#define BIT27		0x08000000
#define BIT28		0x10000000
#define BIT29		0x20000000
#define BIT30		0x40000000
#define BIT31		0x80000000

#ifndef MIN
    #define MIN(x,y)    ( (x<y)?x:y)
#endif

#ifndef MAX
    #define MAX(x,y)    ( (x>y)?x:y)
#endif

//===========================================================================
// I04: DATA TYPES
//===========================================================================

#if defined (__KERNEL__)
#include <linux/types.h>
#endif



#if !defined(__cplusplus) && !defined(__KERNEL__) && !defined(_STDBOOL_H)
    typedef unsigned char  bool;
#endif

#if !defined (__ASM_ARM_TYPES_H) || !defined(__KERNEL__)
    typedef char  			c8;
    typedef unsigned char  		u8;
    typedef unsigned short 		u16;
    typedef unsigned int   		u32;
    typedef unsigned long long  	u64;

    typedef signed char  		s8;
    typedef signed short 		s16;
    typedef signed int   		s32;
    typedef signed long long 	s64;
#endif


//===========================================================================
// I04: EXTERN DATA BLOCK
//===========================================================================

//===========================================================================
// I05: FUNCTION PROTOTYPES
//===========================================================================

#endif /* __MT_TYPES_H__ */

//===========================================================================
// I06: END OF HEADER
//===========================================================================


