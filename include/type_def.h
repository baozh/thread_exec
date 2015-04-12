/*=====================================================================================
ģ����  ��
�ļ���  ��type_def.h
����ļ���
ʵ�ֹ��ܣ�ʵ�ָ������͵����ͱ���
����    ��������(bao.z.h.2008@gmail.com)
��Ȩ    ��
---------------------------------------------------------------------------------------
�޸ļ�¼��
����        �汾    �޸���      �߶���      �޸ļ�¼
2014/1/26   V1.0    ������                    ����
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



