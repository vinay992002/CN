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
int ports[3] = {8001,8002,8003};
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
void handler(){
  printf("Multiplexes are full!!!\n");
  printf("counter is closing.......\n");
  exit(0);
}
int main(){
  struct sockaddr_in serv_addr[3],cli_addr;
  struct sockaddr_un  userv_addr,ucli_addr,remote,local;
  int i=0;
  int sfd[3];
  char socks[15];
  int usfd;
  usfd = socket(AF_UNIX,SOCK_STREAM,0);
      bzero((struct sockaddr_un*)&userv_addr,sizeof(userv_addr));
    userv_addr.sun_family = AF_UNIX;
    int uefd = socket(AF_UNIX,SOCK_STREAM,0);
    bzero((struct sockaddr_un*)&local,sizeof(local));
    local.sun_family= AF_UNIX;

  bzero(socks,15);
    sprintf(socks,"%s","skservc");
    strcpy(userv_addr.sun_path,socks);
    bzero(socks,15);
    sprintf(socks,"%s","skserve");
    strcpy(local.sun_path,socks);
    unlink(userv_addr.sun_path);
    int len = strlen(userv_addr.sun_path) + sizeof(userv_addr.sun_family);
    if(bind(usfd,(struct sockaddr*)&userv_addr,len)<0)
    {
      printf("bind error!!\n");
      exit(0);
    }
    unlink(local.sun_path);
    len = strlen(local.sun_path) + sizeof(local.sun_family);
    if(bind(uefd,(struct sockaddr*)&local,sizeof(local))<0)
    {
      printf("bind error!!\n");
      exit(0);
    }
  for(i=0;i<3;i++){
    sfd[i] = socket(AF_INET,SOCK_STREAM,0);
    if(sfd[i]<0 )
    {
      printf("socket error!!\n");
      exit(0);
    }
    //strncpy(serv_addr.sun_path, "socket", sizeof(serv_addr.sun_path)-1);
   // unlink(userv_addr[i].sun_path);
    
    printf("%d\n",ports[i]);
    bzero((char *) &serv_addr[i],sizeof(serv_addr[i]));
  serv_addr[i].sin_family = AF_INET;

  serv_addr[i].sin_port = htons(ports[i]);//htons  converts a port number in host byte order to a port number in network byte order
  serv_addr[i].sin_addr.s_addr = INADDR_ANY;
  if(bind(sfd[i],(struct sockaddr *) &serv_addr[i],sizeof(serv_addr[i]))<0)
  {
    printf("bind Error!!\n");
    exit(0);
  }
  listen(sfd[i],5);
  }
  listen(usfd,5);
  listen(uefd,5);
    printf("please open multiplex C for starting counter!!\n");
    int cllen=0,rlen=0;
    int unsfd = accept(usfd,(struct sockaddr *)&ucli_addr,&cllen);
    printf("conected!\nplease open multiplex E\n");
    int unefd = accept(uefd,(struct sockaddr*)&remote,&rlen);
    printf("connected!!\n");
    fd_set readfd;
    int maxfd;
    struct timeval t;
    t.tv_sec=1;
    t.tv_usec = 100;
    signal(SIGUSR1,handler);
    while(1){
      maxfd =0;
      FD_ZERO(&readfd);
      for(i=0;i<3;i++)
      {
        FD_SET(sfd[i],&readfd);
        maxfd = maxfd>sfd[i]?maxfd:sfd[i];
      }
      int ret = select(maxfd+1,&readfd,NULL,NULL,&t);
      if(ret>0){
        for(i=0;i<3;i++){
          if(FD_ISSET(sfd[i],&readfd)){
            int cllength=0;
            int nsfd = accept(sfd[i],(struct sockaddr*)&cli_addr,&cllength);
            if(nsfd<0){
              printf("accept error!!\n");
              exit(0);
            }
            send_fd(unsfd,nsfd);
          }
        }
      }
    }
}