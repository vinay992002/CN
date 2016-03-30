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
#include <sys/time.h>
#include <sys/select.h>
#include <signal.h>
#include <pthread.h>
int fds[10] = {-1};
int l=0;
int recv_fd(
  int param) /* Socket from which the file descriptor is read */
{

 int socket = (param);

 int sent_fd;
 struct msghdr message;
 struct iovec iov[1];
 struct cmsghdr *control_message = NULL;
 char ctrl_buf[CMSG_SPACE(sizeof(int))];
 char data[1];
 int res;

 //printf("Waiting to receive fd %d\n",socket);
     memset(&message, 0, sizeof(struct msghdr));
     memset(ctrl_buf, 0, CMSG_SPACE(sizeof(int)));

     /* For the dummy data */
     iov[0].iov_base = data;
     iov[0].iov_len = sizeof(data);

     message.msg_name = NULL;
     message.msg_namelen = 0;
     message.msg_control = ctrl_buf;
     message.msg_controllen = CMSG_SPACE(sizeof(int));
     message.msg_iov = iov;
     message.msg_iovlen = 1;

     // if((res = recvmsg(socket, &message, 0)) <= 0)
         // printf("Does not receive\n");
   if( recvmsg(socket, &message, 0)<0){
    perror("recvmsg!!\n");
   }

     /* Iterate through header to find if there is a file descriptor */
     control_message = CMSG_FIRSTHDR(&message);
      if((control_message->cmsg_level == SOL_SOCKET) &&
          (control_message->cmsg_type == SCM_RIGHTS) )
      {
       int sd  =  *((int *) CMSG_DATA(control_message));
      // csfd[ccount++] = sd;
       
       //printf("Recieved\n");
      return sd;
      }
     
 return -1;
}
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

void *serv(void *num){
  fd_set readfd;
  int maxfd =0;
  int i;
  struct timeval t;
  t.tv_sec=1;
  t.tv_usec =100;
  while(1){
    //printf("fuckkkkkkkkkkkkkkk");
    //pthread_mutex_lock(&mutex);
  //  printf("ok\n");
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
    
  } }
}


int main(){
  struct sockaddr_un  userv_addr,local,remote;
  int usfd;
  usfd = socket(AF_UNIX,SOCK_STREAM,0);
      bzero((struct sockaddr_un*)&userv_addr,sizeof(userv_addr));
    userv_addr.sun_family = AF_UNIX;
    char socks[15];
    sprintf(socks,"%s","skserve");
    strcpy(userv_addr.sun_path,socks);
    int len = strlen(userv_addr.sun_path) + sizeof(userv_addr.sun_family);
    if(connect(usfd,(struct sockaddr*)&userv_addr,len)<0){
      printf("connect error!!\n");
      exit(0);
    }
    printf("connected to TIC\n");
    int csfd= socket (AF_UNIX,SOCK_STREAM,0);
     bzero((struct sockaddr_un*)&local,sizeof(local));
    local.sun_family = AF_UNIX;
     bzero(socks,15);
    sprintf(socks,"%s","sksep");
    strcpy(local.sun_path,socks);
  
    len = strlen(local.sun_path) + sizeof(local.sun_family);
    printf("connecting to multiplex S\n");
  if(connect(csfd,(struct sockaddr*)&local,len)<0){
    printf("connect error!!\n");
    exit(0);
  }   
  printf("connected to Multiplex S");
  int cllen=0;
  listen(csfd,5);
  pthread_t pd;
  pthread_create(&pd,NULL,serv,(void*)&cllen);
  
    
    while(1){
      int sfd = recv_fd(usfd);
      printf("client recieved");
      if(l>=1){
        printf("Multiplexes are full...\n");
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
      else
      {
        fds[l++]=sfd;
        printf("servicing the client\n");
      }
    }
}