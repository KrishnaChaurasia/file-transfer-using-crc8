#include<iostream>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/uio.h>
#include<unistd.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/syscall.h>

using namespace std;
#define PORT 5555

void computecrc();
int size=40;      //32+8
int sizep=9;
char mesg[40]={' '};
char tempmsg[100]={' '};
char poly[9]={'1','0','0','0','0','0','1','1','1'};
int ack=0,nack=0;
int flag=0;

int main(int argc,char * argv[])
{
  int fd;
  struct sockaddr_in server,client;
  struct hostent *hp;

  server.sin_family=AF_INET;
  server.sin_addr.s_addr=INADDR_ANY;
  server.sin_port=htons(PORT);

  fd=socket(AF_INET,SOCK_STREAM,0);
  if(fd<0){
    cout<<"Error creating socket\n";
    return 0;
  }
    cout<<"Socket Created\n";

  hp=gethostbyname(argv[1]);
  bcopy((char*)hp->h_addr,(char*)&server.sin_addr.s_addr,hp->h_length);

  int con;
  con=connect(fd,(struct sockaddr*)&server,sizeof(struct sockaddr));

  if(con<0){
    cout<<"Error connecting\n";
    return 0;
  }
  
  cout<<"\nConnection has been made\n\n";
  int rec;  
  while(rec=recv(fd,mesg,sizeof(mesg),0)){
    if(rec<0){
      cout<<"Error receiving\n";
      return 0;
    }
    cout<<"Message received is mesg : "<<mesg<<endl;
    
    int i=0,j=0;
    while(mesg[i]!='\0'){
      tempmsg[j++]=mesg[i++];
    }
    computecrc();
///    if(flag==0) send(fd,"ACK",3,0);
//    else send(fd,"NACK",4,0);
 }
  cout<<"ACK Count : "<<ack<<endl;
  cout<<"NACK Count : "<<nack<<endl;
  close(fd);
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
  
  flag=0;
  for(int i=0;i<sz;i++)
    if(tempmsg[i]=='1'){
      flag++;
      break;
    }
  if(flag==0){ cout<<"No error\n\n"; ack++;}
  else {cout<<"Message contains error\n\n"; nack++;}
}
