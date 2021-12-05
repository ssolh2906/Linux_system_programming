#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define	ARRAY_SIZE	40000
#define	MALLOC_SIZE	100000
#define	SHM_SIZE	100000
#define	SHM_MODE	(SHM_R | SHM_W)

// data segment 에 저장 (전역변수와 static변수...)
char	Array[ARRAY_SIZE];


// VA 상에 어디 저장 되는지 
main()
{
	// 로컬변수이므로 스택 저장
	int		shmid;
	char	*ptr, *shmptr;

	// malloc으로 할당 > Haep 에 저장 
	if ((ptr = (char *)malloc(MALLOC_SIZE)) == NULL)  {
		perror("malloc");
		exit(1);
	}

	// shared memory 만들기 
	// 인자 :이름(IPC_PRIVATE :나만 쓰겠다는 뜻, ps종료 시 pa 자동 반납 without IPC_RMID), BUT 2개 ps간 못씀
	// 따라서 fork 할 때 쓴다.
	// arg2 : SIZE, arg3: MODE  
	if ((shmid = shmget(IPC_PRIVATE, SHM_SIZE, SHM_MODE)) < 0)  {
		perror("shmget");
		exit(1);
	}
	if ((shmptr = shmat(shmid, 0, 0)) == (void *) -1)  {
	// 정상 : sm 포인터 반환
	// 포인터는 stack 과 heap 중간 부분에 저장된다. 
		perror("shmat");
		exit(1);
	}

	printf("Array[] from %p to %p\n", &Array[0], &Array[ARRAY_SIZE]);
	printf("Stack around %p\n", &shmid);
	printf("Malloced from %p to %p\n", ptr, ptr+MALLOC_SIZE);
	printf("Shared memory attached from %p to %p\n", shmptr, shmptr+SHM_SIZE);

	// IPC_PRIVATE 이므로 자동삭제되지만 예제이므로 넣음... 
	if (shmctl(shmid, IPC_RMID, 0) < 0)  {
		perror("shmctl");
		exit(1);
	}
}
