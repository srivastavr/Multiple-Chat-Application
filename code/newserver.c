#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
 

char buffer[256],employ[100],str[100],pass[30];
struct sockaddr_in serv_addr, cli_addr;
int n,nu=1,i;

FILE *fp;
 
struct userinfo
{
  char username[100];
  int socket;
}u[1000];
 
pthread_t thread;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

 
 
void error(char *msg) 
 {
 perror(msg); 
 exit(0);
 }
 
 
 
 
void* server(void*);
 
 
int main (int argc, char *argv[]) 
 {
    

   int i,sockfd, newsockfd[1000], portno, clilen,no=0,n;
   try:
   fp=fopen("employ.txt","w");
  // fprintf(fp,"server started\n");
   printf("server started\n");
   fclose(fp);
 
  if (argc<2) 
  {
  fprintf (stderr,"ERROR! Provide A Port!\n");
  exit(1);
  }
   
 sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
 if (sockfd<0) 
   error("ERROR! Cannnot Open Socket");
   
  bzero((char *) &serv_addr, sizeof(serv_addr));
   
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
   
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0)
    error("ERROR! On Binding");
   
  listen(sockfd, 5);
 
  clilen = sizeof(cli_addr);
 
 
 
while(1)
{
 newsockfd[no] = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
  
 if (newsockfd<0)
  error("ERROR! On Accepting");
 
  pthread_mutex_lock(&mutex);
  pthread_create(&thread,NULL,server,(void*)&newsockfd[no]);
  no+=1;
 
}
   
  
 close(newsockfd[no]);
 //exit(1);
  
 close(sockfd);
 goto try;
 return 0;
   
 }
 
void* server(void* sock)
{
  char to[100],from[100],name[100];
  int newsockfd=*((int*)sock),j=0,m;
 
  fp=fopen("employ.txt","r+");
 
  checking:
  m=1;
  bzero(employ,100);
  bzero(to,100);
  bzero(from,100);
  bzero(str,100);
  recv(newsockfd, employ,100,0);
  recv(newsockfd, pass,30,0);
  while(fscanf(fp,"%s",str)!=EOF)
  {
    n=strncmp(employ,str,strlen(str));
    if(n==0)
    {
      fscanf(fp,"%s",str);
      n=strncmp(pass,str,strlen(str));
      if(n==0)
      {
       m=2;
       break;
      }
     else
     {
      send(newsockfd,"USERNAME EXISTS",15,0);
      m=0;
      break;}
   fscanf(fp,"%s",str);
   }
 }
  if(m==0)
  goto checking;
  if(m==1)
   {
    fclose(fp);
    send(newsockfd,"server accepted your credentials",15,0);
    printf("Entered as client:");
printf("%s ",employ);
    bzero(u[nu].username,100);
    u[nu].socket=newsockfd;
    strncpy(u[nu].username,employ,strlen(employ));
    nu++;
    }
  if(m==2)
  {
    fclose(fp);
    send(newsockfd,"USER LOGGED IN",14,0);
    for(i=1;i<nu;i++)
      if(strcmp(employ,u[i].username)==0)
      break;
    u[i].socket=newsockfd;
  }
 pthread_mutex_unlock(&mutex);
 
 bzero(buffer, 256);
 int newsockfd1;
 while(1)
 { 
   n = recv(newsockfd, buffer, 255, 0);
   if(n<0)
     error("ERROR! Reading From Socket");
     if(strncmp(buffer,"bye",3)==0)
     {
       close(newsockfd);
       pthread_exit(NULL);
     } 
    
  i=3;
 
  strcpy(name,buffer);
  while(name[i]!=':')
   {
     to[i-3]=name[i];
     i++;
   }
  to[i-3]='\0';
  j=0;
  bzero(buffer,256);
  while(name[i]!='|')
    {
      buffer[j]=name[i];
      i++;
      j++;
    }
  buffer[j]='\0';
  j=0;
  for(i+=1;name[i]!='\0';i++)
   {
     from[j]=name[i];
     j++;
   }
  from[j-1]='\0';
  printf("To %s  From %s Message %s",to,from,buffer);
  
  for(j=1;j<nu;j++)
   {
     if((strncmp(u[j].username,to,strlen(to)))==0)
      {
         newsockfd1=u[j].socket;
         break;
      }
   }  
  strcat(from,buffer);
  bzero(buffer,256);
  strcpy(buffer,"From ");
  strcat(buffer,from); 
  
  n=send(newsockfd1,buffer,sizeof buffer,0);
  if(n<0)
   {
     send(newsockfd, "SENDING FAILED : employ LOGGED OUT",32,0); 
   }
  else
   {
     n = send(newsockfd, "Message sent", 18, 0);
     if (n<0)
     error("ERROR! Writing To Socket");}
  }
  close(newsockfd);
  pthread_exit(NULL);
}
