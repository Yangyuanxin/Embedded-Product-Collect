#include <stdio.h>
#include <string.h>
#include <netdb.h> 
#include <arpa/inet.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <termios.h>

#define HOST "agnss.techtotop.com"

int main() {
  int nfd,sfd;
  int i=0,j=0;
  char temp[1024]={0};   
  char data[8192]={0};   
  struct sockaddr_in addr;
  struct termios options;  
  struct hostent *p; 
  char *ip;
  char *dev = "/dev/ttyS0";
  
  nfd = socket(AF_INET, SOCK_STREAM, 0);
  p = gethostbyname(HOST); 
  ip = inet_ntoa(*(struct in_addr *)p->h_addr_list[0]);

  bzero(&addr,sizeof(struct sockaddr_in));
  addr.sin_family=AF_INET;
  addr.sin_addr.s_addr = inet_addr(ip);
  addr.sin_port=htons(9011);
  
  connect(nfd,(struct sockaddr *)(&addr), sizeof(struct sockaddr));
  
  write(nfd, "all", 3);
  
  while(1) {
    i=read(nfd,temp,1024);
    if(0==i)
      break;
    memcpy(data+j,temp,i);
    j+=i;
  }
  
  close(nfd);

  sfd=open(dev,O_RDWR|O_NOCTTY|O_NDELAY);
  
  tcgetattr(sfd, &options);  
  cfsetispeed(&options, B9600);  
  cfsetospeed(&options, B9600);  
  options.c_cflag |= (CLOCAL | CREAD);  
  options.c_cflag &= ~PARENB;  
  options.c_cflag &= ~CSTOPB;  
  options.c_cflag &= ~CSIZE;  
  options.c_cflag |= CS8;  
  options.c_cflag &= ~CRTSCTS;  
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  
  options.c_iflag &= ~(IXON | IXOFF | IXANY);  
  options.c_oflag &= ~OPOST;  

  tcflush(sfd,TCIFLUSH);
  tcsetattr(sfd, TCSANOW, &options); 
   
  write(sfd,data,j);
   
  close(sfd);

//  FILE *fp=fopen("data","wb");
//  fwrite(data,j,1,fp);
//  fclose(fp);

  return 0;
}

