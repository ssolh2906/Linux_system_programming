#define	MY_ID		00			// set differently on multi-user enviroment

#define	SHM_KEY		(0x9000 + MY_ID)
#define	SHM_SIZE	1024
#define	SHM_MODE	(SHM_R | SHM_W | IPC_CREAT)
