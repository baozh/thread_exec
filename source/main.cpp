
#include <iostream>
#include "type_def.h"
//#include "osp.h"
//#include "locker.h"
//#include "product_consume.h"
#include "threadpool.h"

CThreadsManager g_cThreadsManager;





using namespace std;

int main(void)
{
// 	Task k;
// 	k.StartThread();
// 	
// 	OspTaskDelay(10000);
// 
// 	k.StopTread();

	u32 dwTest = 1;
	s32 nTest = 2;

	if (dwTest - 2 > 0)
	{
		printf("true\n");
	}
	else
	{
		printf("false\n");
	}
	
	system("pause");

	return 0;
}

