#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm.h"


main()
{
	int		shmid;
	char	*ptr, *pData;
	int		*pInt;


	// get
	// key, size 등은 .h에 저장되어있음 
	if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0)  {
		perror("shmget");
		exit(1);
	}
	// arg2,3 = 0,0 : OS 니가 알아서 정해...
	if ((ptr = shmat(shmid, 0, 0)) == (void *) -1)  {
		perror("shmat");
		exit(1);
	}

	// 동기화 
	// 쓰는 ps끝나고 출력 해야함
	
	pInt = (int *)ptr;
	// int 로 4바이트 읽고
	// 초기화만 됏으면 0이니까 무한루프 > sipc2 실행 


	while ((*pInt) == 0)
		;

	// pData = flag 뒤
	pData = ptr + sizeof(int);
	*pData = 'c';
	printf("Received request: %s.....", pData);
	sprintf(pData, "This is a reply from %d.", getpid());
	// 다썼다고 알림!
	*pInt = 0;
	printf("Replied.\n");

	// sipc2 가 사용할 때까지 1초 기다려주고 
	sleep(1);
	
	// 쉐어드메모리 해제(없앰)
	if (shmctl(shmid, IPC_RMID, 0) < 0)  {
		perror("shmctl");
		exit(1);
	}
}
