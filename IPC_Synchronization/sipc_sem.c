// Inter Thread communication
// Using semaphore
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>

#define     MAX_STR     100

// global var instead of shared memory
char            msg[MAX_STR];

// semaphores
sem_t   RequestSem, ReplySem, MutexSem;


void
ThreadUsleep(int usecs)
{
	pthread_cond_t		cond;
	pthread_mutex_t		mutex;
	struct timespec		ts;
	struct timeval		tv;

	if (pthread_cond_init(&cond, NULL) < 0)  {
		perror("pthread_cond_init");
		pthread_exit(NULL);
	}
	if (pthread_mutex_init(&mutex, NULL) < 0)  {
		perror("pthread_mutex_init");
		pthread_exit(NULL);
	}

	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec + usecs/1000000;
	ts.tv_nsec = (tv.tv_usec + (usecs%1000000)) * 1000;
	if (ts.tv_nsec >= 1000000000)  {
		ts.tv_nsec -= 1000000000;
		ts.tv_sec++;
	}

	if (pthread_mutex_lock(&mutex) < 0)  {
		perror("pthread_mutex_lock");
		pthread_exit(NULL);
	}
	if (pthread_cond_timedwait(&cond, &mutex, &ts) < 0)  {
		perror("pthread_cond_timedwait");
		pthread_exit(NULL);
	}

	if (pthread_cond_destroy(&cond) < 0)  {
		perror("pthread_cond_destroy");
		pthread_exit(NULL);
	}
	if (pthread_mutex_destroy(&mutex) < 0)  {
		perror("pthread_mutex_destroy");
		pthread_exit(NULL);
	}
}

void 
thread1(void *dummy)
// sipc1 의 역할을 하는 Thread
// request 받으면 출력하고, reply 보냄
{
    printf("Thread1: Start.....\n");

    // request data 있으면 진입
    if (sem_wait(&RequestSem) < 0)
    {
        perror("sem_wait");
        exit(1);
    }
    if (sem_wait(&MutexSem) < 0)
    {
        perror("sem_wait");
        exit(1);
    }


    //request 받았을 시 ( critical section 진입 )
    printf("Thread1 got an request.....\n");
    printf("Request from thread2 : %s\n", msg);

    // write reply on glabal var
    sprintf(msg, "Reply from thread1\n");
    
    // Post reply sem & mutex sem
    if (sem_post(&MutexSem) < 0)
    {
        perror("sem_post");
        exit(1);
    }
    if (sem_post(&ReplySem) < 0)
    {
        perror("sem_post");
        exit(1);
    }

    ThreadUsleep(3000);
    
    
    pthread_exit(NULL);
}

void
thread2(void *dummy) 
// sipc2 의 역할을 하는 Thread
// request 를 보내고, reply 를 받아 출력함
{
    char    *replyMsg[MAX_STR];

    printf("Thread2: Start.....\n");


    if (sem_wait(&MutexSem) < 0)
    {
        perror("sem_wait");
        exit(1);
    }

    // critical section 진입
    // request 작성
    sprintf(msg, "This is request from Thread2....\n");

    // post 주고 mutex 해제
    if (sem_post(&MutexSem) < 0) 
    {
        perror("sem_post");
        exit(1);
    }
    if (sem_post(&RequestSem) < 0)
    {
        perror("sem_post");
        exit(1);
    }

    

    printf("Sent a request...\n");

    // reply 기다림(초깃갑ㅅ == 0, thread1 post 이후 )
    if (sem_wait(&ReplySem) < 0)
    {
        perror("sem_wait");
        exit(1);
    }
    
    // reply 받았을 시
    printf("Received reply from Thread1 : %s\n", msg);
    

    ThreadUsleep(3000);

    pthread_exit(NULL);
}



int main()
{
	pthread_t	tid1, tid2;
    msg[0] = NULL;
    // RequestSem, ReplySem 둘다 0으로 초기화
    if (sem_init(&RequestSem,0,0) < 0)
    {
        perror("sem_init");
        exit(1);
    }
    if (sem_init(&ReplySem,0,0) < 0)
    {
        perror("sem_init");
        exit(1);
    }
    // MutexSem 1로 초기화( binary sem )
    if (sem_init(&MutexSem,0,1) < 0)
    {
        perror("sem_init");
        exit(1);
    }

    if (pthread_create(&tid1, NULL, (void *)thread1, (void *)NULL) < 0)
    {
        perror("pthread_create");
        exit(1);
    }
    if (pthread_create(&tid2, NULL, (void *) thread2, (void *)NULL) < 0)
    {
        perror("pthread_create");
        exit(1);
    }
    
    if (pthread_join(tid1, NULL) < 0)  {
		perror("pthread_join");
		exit(1);
	}
	if (pthread_join(tid2, NULL) < 0)  {
		perror("pthread_join");
		exit(1);
	}

    printf("Request and Reply Done succesfully....\n");
    printf("Destroy semaphores...\n");


	if (sem_destroy(&RequestSem) < 0)  {
		perror("sem_destroy");
	}
	if (sem_destroy(&ReplySem) < 0)  {
		perror("sem_destroy");
	}
	if (sem_destroy(&MutexSem) < 0)  {
		perror("sem_destroy");
	}

    return 0;
}
