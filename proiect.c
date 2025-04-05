#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>


int main(int argc,char **argv){
  if(argc!=2){
    perror("Eroare utilizare program");
    exit(-1);
  }
  int fd=open(argv[1],O_RDONLY);
  if(fd<0){
    fd=open(argv[1],O_CREAT| O_RDONLY,0777);
    return 0;
  }
  char buffer[100];
  while(read(fd,buffer,sizeof(buffer))){
      write(1,buffer,strlen(buffer));
    }
  close(fd);
  fd=open(argv[1],O_APPEND|O_WRONLY);
  memset(buffer,0,sizeof(buffer));
  while(read(0,buffer,sizeof(buffer))){
    write(fd,buffer,strlen(buffer));
    }

  int fd_log=open("logg1_txt",O_CREAT|O_WRONLY,0777);
  if(fd_log<0){
    perror("Eroare deschidere fisier");
    exit(-1);
  }
  struct stat st;
  lstat(argv[1],&st);
  
  
  write(fd_log,"Acest fisier contine\n",strlen("Acest fisier contine\n"));
  memset(buffer,0,sizeof(buffer));
  sprintf(buffer,"%ld",st.st_size);
	   write(fd_log,buffer,sizeof(buffer));
	   memset(buffer,0,sizeof(buffer));
  write(fd_log," biti\n",strlen(" biti\n"));
  sprintf(buffer,"%d",st.st_uid);
  write(fd_log,buffer,sizeof(buffer));
  memset(buffer,0,sizeof(buffer));
  write(fd_log," ID user\n",strlen(" ID user\n"));
  write(fd_log," Ultima modificare la:",sizeof("Ultima modificare la:"));
  

  close(fd_log);
  close(fd);
  return 0;
}
