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
      if((control_message->cmsg_level == SOL_SOCKET) &&
          (control_message->cmsg_type == SCM_RIGHTS) )
      {
       int sd  =  *((int *) CMSG_DATA(control_message));
      // csfd[ccount++] = sd;
       
       //printf("Recieved\n");
      return sd;
      }
     
 }
}
int main(){

}