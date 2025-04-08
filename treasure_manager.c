#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>  //pentru lucrul cu directoare
 

typedef struct treasure{

  int ID;
  char Nume[50];
  float X_coordinate;
  float Y_coordinate;
  char Clue[100];
  int Value;
  
} Treasure;  //structura pentru retinerea de informatii in cadrul unei comori

Treasure new_treasure(){
  Treasure comoara;
  char buffer[100];
  write(1,"Introduceti ID-ul comorii:",strlen("Introduceti ID-ul comorii:"));
  read(0,buffer,sizeof(buffer));
  comoara.ID=atoi(buffer);
  memset(buffer,0,sizeof(buffer));

  write(1,"Introduceti numele comorii:",strlen("Introduceti numele comorii:"));
  read(0,buffer,sizeof(buffer));
  buffer[sizeof(buffer)+1]='\n';
  strcpy(comoara.Nume,buffer);
  memset(buffer,0,sizeof(buffer));

  write(1,"Introduceti coordonatele x ale comorii:",strlen("Introduceti coordonatele x ale comorii:"));
  read(0,buffer,sizeof(buffer));
  comoara.X_coordinate=atof(buffer);
  memset(buffer,0,sizeof(buffer));

  write(1,"Introduceti coordonatele y ale comorii:",strlen("Introduceti coordonatele y ale comorii:"));
  read(0,buffer,sizeof(buffer));
  comoara.Y_coordinate=atof(buffer);
  memset(buffer,0,sizeof(buffer));

 write(1,"Introduceti indiciul comorii:",strlen("Introduceti indiciul comorii:"));
  read(0,buffer,sizeof(buffer));
  buffer[sizeof(buffer)+1]='\n';
  strcpy(comoara.Clue,buffer);
  memset(buffer,0,sizeof(buffer));

  write(1,"Introduceti valoarea comorii:",strlen("Introduceti valoarea comorii:"));
  read(0,buffer,sizeof(buffer));
  comoara.Value=atoi(buffer);
  memset(buffer,0,sizeof(buffer));

  return comoara;
}


void add(const char *huntID){

  char Treasure_file[100];
  char director[100];
  char logg_hunt[100];
  char logg_hunt_root[100];
  
  sprintf(logg_hunt_root , "./%s-%s" ,"root_logg_file",huntID) ;
  DIR *dir=opendir(huntID);
   Treasure comoara=new_treasure();
  if(dir==NULL){
    sprintf(director  ,  "./%s" , huntID) ;
    if(mkdir(director,S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH)==-1){
      perror("Eroare creare director nou\n");
      exit(-1);
    }
    dir=opendir(director);
    if(dir==NULL){
      perror("Eroare la deschiderea directorului nou creat\n");
    }
 
    sprintf(Treasure_file,"%s/%s",director,"treasure");
    sprintf(logg_hunt,"%s/%s",director,"log");
    
    if(symlink(logg_hunt,logg_hunt_root)==-1){
      perror("Eroare la crearea legaturii simbolice dintre logg_hunt si logg_hunt_root\n");
      exit(-2);
    }
  }

  else{
    char buff[50];
    sprintf(buff,"treasure %d",comoara.ID);
    sprintf(Treasure_file,"%s/%s",huntID,buff);
    sprintf(logg_hunt,"%s/%s",huntID,"logg_hunt");
  }

  
  int fd=open(Treasure_file,O_RDWR,0644);
  if(fd<0){
    fd=open(Treasure_file,O_CREAT|O_RDWR,0644);
    if(fd<0){
    perror("Eroare deschidere fisier\n");
    exit(-1);
    }
  }

  dprintf(fd, "ID: %d\nNume: %sCoordonate: %.2f %.2f\n Indiciu: %s Valoare:%d\n",
        comoara.ID, comoara.Nume,
	  comoara.X_coordinate, comoara.Y_coordinate,comoara.Clue,comoara.Value);

  
  if(close(fd)==-1){
    perror("Eroare la inchiderea fisierului comorii\n");
    exit(-2);
  }

  fd=open(logg_hunt,O_WRONLY|O_APPEND,0644);
  if(fd<0){
    fd=open(logg_hunt,O_CREAT|O_WRONLY|O_APPEND,0644);
    if(fd<0){
      perror("Eroare la deschiderea fisierului de logg/n");
      exit(-3);
    }
  }

  char buffer[100];
  sprintf(buffer,"S-a adaugat comoara cu ID-ul %d si numele %s \n",comoara.ID,comoara.Nume);
  if(write(fd,buffer,strlen(buffer))==-1){
    perror("Eroare in scrierea fisierului logg");
    exit(-3);
  }

  if(close(fd)==-1){
    perror("Eroare la inchiderea fisierului de logg");
    exit(-2);
  }

  if(closedir(dir)==-1){
    perror("Eroare la inchiderea directorului");
    exit(-4);
  }  
}

void list(const char *huntID){
 
}



void view(const char *huntID,int ID){

  DIR *dir=opendir(huntID);
  char Treasure_file[200];

  if(dir == NULL){
    perror("Nu exista acest hunt\n");
  }

  sprintf(Treasure_file,"./%s/treasure %d",huntID,ID);
  //printf("%s", Treasure_file);
  int fd=open(Treasure_file,O_RDONLY);
  if(fd<0){
    perror("Nu s-a gasit aceasta comoara\n");
    exit(-1);
  }

  char buffer[200];
  int bytes_read;
  while(bytes_read=read(fd,buffer,sizeof(buffer))) {
    write(1,buffer,bytes_read);
      
    }

  close(fd);
  }   
     


int main(int argc,char **argv){
  if(argc < 2){
    perror("Utilizare complet eronata \n");
    exit(-1);
  }
 else if(strcmp(argv[1],"--add")==0){
    if(argc<3){
      perror("Utilizare eronata: apelare_program --add hunt_id");
    }
    else{
      add(argv[2]);
    }
  }

 else if(strcmp(argv[1],"--list")==0){
    if(argc<3){
      perror("Utilizare eronata: apelare_program --list hunt_id");
      exit(-2);
    }
    else{
      list(argv[2]);
    }
  }

 else if(strcmp(argv[1],"--view")==0){
    if(argc<4){
      perror("Utilizare eronata: apelare_program --view hunt_id id");
      exit(-3);
    }
    else{
      view(argv[2],atoi(argv[3]));
    }
  }
 else{
   perror("Comanda necunoscuta");
   exit(-1);
 }


  return 0;
}
