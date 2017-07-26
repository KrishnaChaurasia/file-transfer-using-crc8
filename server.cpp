#include<iostream>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>
#include<sys/uio.h>
#include<unistd.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/syscall.h>
#include<stdlib.h>
using namespace std;

#define PORT 5555
#define BACKLOG 3

void computecrc();
void corrupt();

int size=40;   //32+8
int sizep=9;
char mesg[40]={' '};
char tempmsg[100]={' '};
char poly[9]={'1','0','0','0','0','0','1','1','1'};
char buf[4]={' '};

int main(){

  srand(time(NULL));
  int fd,fd2;
  struct sockaddr_in server,client;

  int bytes;

  fd=socket(AF_INET,SOCK_STREAM,0);
  if(fd<0){
    cout<<"Error creating socket\n";
    return 0;
  }
  
  cout<<"Socket successfully created\n\n";
  server.sin_family=AF_INET;
  server.sin_port=htons(PORT);
  server.sin_addr.s_addr=INADDR_ANY; 
  
  int bnd;

  bnd=bind(fd,(struct sockaddr*)&server,sizeof(server));
  if(bnd<0){
    cout<<"Error binding socket\n";
    return 0;
  }
  cout<<"Binding successful\n\n";
  
  if(listen(fd,BACKLOG)<0){
    cout<<"Error listening\n";
    return 0;
  }

  cout<<"Server is listening\n";
  socklen_t len=sizeof(client);
  fd2=accept(fd,(struct sockaddr*)&client,&len);
  if(fd2<0){
    cout<<"Error accepting\n";
      return 0;
  }
  
  cout<<"Accept successful\n";
  
  int from;
  from=open("h.txt",O_RDONLY);
  if(from<0){
    cout<<"Error opening file\n";
    return 0;
  }
  int n=1;
  int s;
  while((n=read(from,mesg,32))!=0){
    if(mesg[0] !='0' && mesg[0]!='1') break;
    for(int i=0;i<size-sizep+1;i++){
      tempmsg[i]=mesg[i];
    }
    cout<<"mesg read from file : "<<mesg<<endl;
    for(int i=size-sizep+1;i<size;i++)
      tempmsg[i]='0';
    computecrc();
    cout<<"mesg with crc appended: "<<mesg<<endl;
    corrupt();
    cout<<"mesg after corruption :"<<mesg<<endl<<endl<<endl;
    s=send(fd2,mesg,sizeof(mesg),0);
    //s=write(fd2,mesg,40);
    //cout<<"Message sent from server : "<<mesg<<endl;
    if(s<0){
      cout<<"Error sending\n";
      return 0;
    }
    for(int i=0;i<32;i++)
      mesg[i]=' ';
    
    //reply=recv(fd,buf,sizeof(buf),0);
    //cout<<reply<<endl;
  }
  
  
  close(fd2);
  close(fd);
  shutdown(fd,0);
  cout<<"\nSocket closed\n";
  return 0;
}

void computecrc(){
  int j=0; int ctr=0; int l=0;
  int count=0;
  int sz=size;

  while(sz>=sizep){
    ++count;l=0;j=0;ctr=0;
    for(int i=0;i<sizep;i++){
      if(tempmsg[j]==poly[i]){
	if(ctr!=0){
	  ++ctr;
	  tempmsg[l++]='0';       //push '0' in temporary array
	}
      }
      else {
	tempmsg[l++]='1';	  //push '1' in temporary array 
	++ctr;
      }
      j++;
    }
    for(int i=sizep;i<=sz;i++){
      tempmsg[l++]=tempmsg[i];
    }
    sz=l-1;
  }
  //  cout<<"tempmsg : ";
  //for(int i=0;i<sz;i++) cout<<tempmsg[i];cout<<endl;
  if(sz<sizep-1){
    int t=sizep-sz-1;
    int i=size-sizep+1;
    while(t>0)
      {
	mesg[i++]='0';
	t--;
      }
    j=0;
    while(tempmsg[j]!='\0')  mesg[i++]=tempmsg[j++];
  }
  else
    {
      int i=size-sizep+1;
      int j=0;
      while(tempmsg[j]!='\0')  mesg[i++]=tempmsg[j++];
    }
}

void corrupt(){
  int check[40]={0};
  int corruptbits=rand()%30;
  cout<<"Number of bits to corrupt :"<<corruptbits<<endl;
  while(corruptbits>0){
    int bitno=rand()%40;
    if(check[bitno]==0){
      if(mesg[bitno]=='1')
	mesg[bitno]='0';
      else
	mesg[bitno]='1';
      check[bitno]=1;
      corruptbits--;
    }
  }
}