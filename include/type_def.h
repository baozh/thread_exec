/*=====================================================================================
模块名  ：
文件名  ：type_def.h
相关文件：
实现功能：实现各个整型的类型别名
作者    ：包增辉(bao.z.h.2008@gmail.com)
版权    ：
---------------------------------------------------------------------------------------
修改记录：
日期        版本    修改人      走读人      修改记录
2014/1/26   V1.0    包增辉                    创建
========================================================================================*/




#ifndef TYPE_DEF_H
#define TYPE_DEF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



typedef int		s32,BOOL32;

#ifdef SUPPORT_64BIT
	typedef unsigned int   u32;
#else
	typedef unsigned long  u32;
#endif

typedef unsigned char	u8;
typedef unsigned short  u16;
typedef short           s16;
typedef char     s8;


#ifdef SUPPORT_64BIT
	typedef long int       s64;
#else
	#ifndef _MSC_VER
	typedef long long      s64;
	#else
	typedef __int64        s64;
	#endif
#endif


#ifdef SUPPORT_64BIT
	typedef unsigned long int       u64;
#else
	#ifndef _MSC_VER
		typedef unsigned long long      u64;
	#else
		typedef unsigned __int64      u64;
	#endif
#endif





#ifndef _MSC_VER
#ifndef LPSTR
#define LPSTR   char *
#endif
#ifndef LPCSTR
#define LPCSTR  const char *
#endif
#endif



#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif



#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* TYPE_DEF_H */



