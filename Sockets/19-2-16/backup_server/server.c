#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stropts.h>
#include <string.h>
#include <fcntl.h>
#include <curses.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#define port 8100
#define SOCK_PATH "sockets"
static int send_fd(
  int socket, /* Socket through which the file descriptor is passed */
  int fd_to_send) /* File descriptor to be passed, could be another socket */
{
 struct msghdr message;
 struct iovec iov[1];
 struct cmsghdr *control_message = NULL;
 char ctrl_buf[CMSG_SPACE(sizeof(int))];
 char data[1];

 memset(&message, 0, sizeof(struct msghdr));
 memset(ctrl_buf, 0, CMSG_SPACE(sizeof(int)));

 /* We are passing at least one byte of data so that recvmsg() will not return 0 */
 data[0] = ' ';
 iov[0].iov_base = data;
 iov[0].iov_len = sizeof(data);

 message.msg_name = NULL;
 message.msg_namelen = 0;
 message.msg_iov = iov;
 message.msg_iovlen = 1;
 message.msg_controllen =  CMSG_SPACE(sizeof(int));
 message.msg_control = ctrl_buf;

 control_message = CMSG_FIRSTHDR(&message);
 control_message->cmsg_level = SOL_SOCKET;
 control_message->cmsg_type = SCM_RIGHTS;
 control_message->cmsg_len = CMSG_LEN(sizeof(int));

 *((int *) CMSG_DATA(control_message)) = fd_to_send;

 return sendmsg(socket, &message, 0);
}
int fds[10]={-1};
int l=0;
void *serv(void *num){
	fd_set readfd;
	int maxfd =0;
	int i;
	struct timeval t;
	t.tv_sec = 1;
	t.tv_usec = 100;
	while(1){
		//printf("fuckkkkkkkkkkkkkkk");
		//pthread_mutex_lock(&mutex);
		//printf("ok\n");
		maxfd =0;
		if(l>0){
		FD_ZERO(&readfd);
		//printf("%d",l);
		for(i=0;i<l;i++)
		{
			FD_SET(fds[i],&readfd);
			maxfd = maxfd>fds[i]?maxfd:fds[i];
		}
		//pthread_mutex_unlock(&mutex);
		int ret = select(maxfd+1,&readfd,NULL,NULL,&t);
		if(ret>0){
			for(i=0;i<l;i++){
				if(FD_ISSET(fds[i],&readfd))
				{
					//FD_CLR(fds[i],&readfd);
					char buf[1024];
					bzero(buf,1024);

					if(recv(fds[i],buf,1024,MSG_DONTWAIT)==0)
					{
						printf("client exiting\n");
						fds[i]=-1;
					}
					printf("%s\n",buf );
					if(strcmp(buf,"-1")==0)
					{
						fds[i]=-1;
						printf("client exiting\n");
					}
					else{
						int j;
						char temp[1024];
						for(j=0;j<strlen(buf);j++)
						{
							if(buf[j]>=97)
								temp[j] = buf[j]-32;
							else
								temp[j]=buf[j];
			//printf("%c",temp[i]);
						}
						temp[j]='\0';
						send(fds[i],temp,strlen(temp),0);
					}

				}
			}
		}
		
	}	}
}
int main(){
	struct sockaddr_in serv_addr,cli_addr;
	int sfd = socket(AF_INET,SOCK_STREAM,0);
	if(sfd<0)
	{
		printf("socket error!!\n");
		exit(0);
	}	
	// to initialie the server address
	bzero((char *) &serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);//htons  converts a port number in host byte order to a port number in network byte order
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(sfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr))<0)
	{
		printf("bind Error!!\n");
		exit(0);
	}
	char rbuffer[256],temp[256];

	listen(sfd,5);
	// for unix socket
	int usfd = socket(AF_UNIX,SOCK_STREAM,0);
    struct sockaddr_un userv_addr,remote;
    bzero((struct sockaddr_un*)&userv_addr,sizeof(userv_addr));
    userv_addr.sun_family = AF_UNIX;
    strcpy(userv_addr.sun_path,SOCK_PATH);
    //strncpy(serv_addr.sun_path, "socket", sizeof(serv_addr.sun_path)-1);
   // unlink(serv_addr.sun_path);
    int len = strlen(userv_addr.sun_path) + sizeof(userv_addr.sun_family);
    
     if(connect(usfd,(struct sockaddr*)&userv_addr,len)<0)
    {
      printf("accept error!!\n");
      exit(0);
    }
    
    int clcnt =0;
    pthread_t pread;
    	pthread_create(&pread,NULL,serv,(void*)&clcnt);
    while(1){
    	int cllen=0;
    	fds[l++] = accept(sfd,(struct sockaddr*)&cli_addr,&cllen);
    	
    	if(fds[l-1]<0){
    		printf("accept error!!\n");
    		exit(0);
    	}
    	if(l > 2){
    		break;
    	}
    	

    }
    int i=0;
    for(i=0;i<3;i++)
    {
    	send_fd(usfd,fds[i]);
    	
    	close(fds[i]);
    	fds[i]=-1;
    }
    l=0;
    int leng;
struct ucred ucr;

leng = sizeof(struct ucred);

if (getsockopt(usfd,SOL_SOCKET, SO_PEERCRED, &ucr, &leng) == -1) {
    //getsockopt failed

	exit(0);
}
printf("Credentials from SO_PEERCRED: pid=%ld, euid=%ld, egid=%ld\n",
    (long) ucr.pid, (long) ucr.uid, (long) ucr.gid);
	sleep(2);
	kill(ucr.pid,SIGUSR1);
}