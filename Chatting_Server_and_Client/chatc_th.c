// chatt client use threads
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "chat.h"

#define MAX_BUF     256

int             Sockfd;
pthread_mutex_t Mutex;
pthread_t	tid1, tid2;


void RecvMsg()
{
    char    buf[MAX_BUF];
    int     n;
    while (1)
    {
        // if : get new packet from server
        if ((n = recv(Sockfd, buf, MAX_BUF, 0)) < 0)
        // args : sock dp, *buf, nbytes, flags
        {
            perror("receive");
            exit(1);
        }
        if (n == 0)
        {
            // Server terminated
            // close socket
            fprintf(stderr, "Server terminated.....\n");
            pthread_mutex_lock(&Mutex);
            close(Sockfd);
            pthread_mutex_lock(&Mutex);
            pthread_exit(NULL);
        } else {
        // case : received msg
            printf("%s", buf);
        }
    }


}
// exit recv thread
void SendMsg()
{
    int     n;
    char    buf[MAX_BUF];
    while (1)
    {
        fgets(buf, MAX_BUF, stdin);

        if ((n = send(Sockfd, buf, strlen(buf)+1,0)) < 0)
        {
            perror("send");
            exit(1);
        }
        
    }
    
}



void
ChatClient(void)
{
    char    buf[MAX_BUF];
    int     n;

    // Log in (make  Login msg)
	printf("Enter ID: ");
	fflush(stdout);
	fgets(buf, MAX_BUF, stdin);
	*strchr(buf, '\n') = '\0';

	// Log In : Send Packet (first packet : ID)
	if (send(Sockfd, buf, strlen(buf)+1, 0) < 0)  {
		perror("send");
		exit(1);
	}
	printf("Press ^C to exit\n");

    // while logged in

    // thread1 : get msg from server
    if (pthread_create(&tid1, NULL, (void *)RecvMsg, (void *)NULL) < 0)  {
        perror("pthread_create");
        exit(1);
    }
    
    // thread2 : send msg to server
    if (pthread_create(&tid2, NULL, (void *)SendMsg, (void *)NULL) < 0)  {
        perror("pthread_create");
        exit(1);
    }

    if (pthread_join(tid1, NULL))
    {
        perror("pthread_join");
        exit(1);
    }
    
    if (pthread_join(tid2, NULL))
    {
        perror("pthread_join");
        exit(1);
    }
}

void
CloseClient(int signo)
{
    // close socket

    // cancel threads
    if(pthread_cancel(tid1)) 
    {
        perror("pthread_cancel");
        exit(1);
    }


    if(pthread_cancel(tid2)) 
    {
        perror("pthread_cancel");
        exit(1);
    }


    close(Sockfd);

	if (pthread_mutex_destroy(&Mutex) < 0)  {
		perror("pthread_mutex_destroy");
		exit(1);
	}

	printf("\nChat client terminated.....\n");

	exit(0);
    

}

int main(int argc, char const *argv[])
{
    struct sockaddr_in  servAddr;
    struct hostent      *hp;
    // check args 
	if (argc != 2)  {
		fprintf(stderr, "Usage: %s ServerIPaddress\n", argv[0]);
		exit(1);
	}

    // Mutex init
    if (pthread_mutex_init(&Mutex, NULL) < 0)  {
		perror("pthread_mutex_init");
		exit(1);
	}

    // socket
	if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  {
		perror("socket");
		exit(1);
	}

    // assign memory
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sin_family = PF_INET;
	servAddr.sin_port = htons(SERV_TCP_PORT);

    // DNS
	if (isdigit(argv[1][0]))  {
		servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	}
	else  {
		if ((hp = gethostbyname(argv[1])) == NULL)  {
			fprintf(stderr, "Unknown host: %s\n", argv[1]);
			exit(1);
		}
		memcpy(&servAddr.sin_addr, hp->h_addr, hp->h_length);
	}

	// connect
	if (connect(Sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  {
		perror("connect");
		exit(1);
	}

	signal(SIGINT, CloseClient);

	ChatClient();

    return 0;
}
