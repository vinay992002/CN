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
/* size of control buffer to send/recv one file descriptor */
#define CONTROLLEN  CMSG_LEN(sizeof(int))
#define MAXLINE 128
#define SOCK_PATH "echo_socket"
//static struct cmsghdr   *cmptr = NULL,*cmptr1 = NULL;  /* malloc'ed first time */
void err_sys(const char* x) 
{ 
    perror(x); 
    exit(1); 
}
void err_ret(const char* x) 
{ 
    perror(x); 
    exit(1); 
}
void err_dump(const char* x) 
{ 
    perror(x); 
    exit(1); 
}
/*
 * Pass a file descriptor to another process.
 * If fd<0, then -fd is sent back instead as the error status.
 */
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
    recvmsg(socket, &message, 0);
  
     /* Iterate through header to find if there is a file descriptor */
     for(control_message = CMSG_FIRSTHDR(&message);
         control_message != NULL;
         control_message = CMSG_NXTHDR(&message,
                                       control_message))
     {
      if( (control_message->cmsg_level == SOL_SOCKET) &&
          (control_message->cmsg_type == SCM_RIGHTS) )
      {
       int sd  =  *((int *) CMSG_DATA(control_message));
      // csfd[ccount++] = sd;
       
      // printf("Recieved\n");
      return sd;
      }
     
 }
}

int main(){
    FILE *f;
    int sfd = socket(AF_UNIX,SOCK_STREAM,0);
    struct sockaddr_un serv_addr,remote;
    bzero((struct sockaddr_un*)&serv_addr,sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path,SOCK_PATH);
    //strncpy(serv_addr.sun_path, "socket", sizeof(serv_addr.sun_path)-1);
   // unlink(serv_addr.sun_path);
    int len = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
    
     if(connect(sfd,(struct sockaddr*)&serv_addr,len)<0)
    {
      printf("accept error!!\n");
      exit(0);
    }
    printf("connection succesful!!\n");
    //f= fopen("text.txt","r");
     int fd;
     int count =0;
     do
     {
      //printf("%c",ch);
    int rfd =   recv_fd(sfd);
    
      count =0;
      char buf[1024];

      printf("Enter no. of words to read:");
      int n ; scanf("%d",&n);
        bzero(buf,1024);
        if(read(rfd,buf,n)==0)
          break;
        printf("%s",buf);
        
      printf("\n");
      // sleep(1);
      send_fd(sfd,rfd);
      close(rfd);
      //int rfd = recv_fd(usfd);
      //f = fdopen(rfd,"r");
     }while(1);
    // fclose(f);
  //   recv_fd()

}