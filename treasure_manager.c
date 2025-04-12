#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>  //pentru lucrul cu directoare
#include <time.h>

 

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

  char Treasure_file[200];
  char director[100];
  char logg_hunt[200];
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
    char tr[10]="treasures";
    
    sprintf(Treasure_file,"%s/%s",director,tr);
    sprintf(logg_hunt,"%s/%s%s",director,"logg_hunt-",huntID);
    
    if(symlink(logg_hunt,logg_hunt_root)==-1){
      perror("Eroare la crearea legaturii simbolice dintre logg_hunt si logg_hunt_root\n");
      exit(-2);
    }
  }

  else{
    char buff[50]="treasures";
    sprintf(Treasure_file,"%s/%s",huntID,buff);
    sprintf(logg_hunt,"%s/%s%s",huntID,"logg_hunt-",huntID);
  }

  
  int fd=open(Treasure_file,O_APPEND|O_RDWR,0644);
  if(fd<0){
    fd=open(Treasure_file,O_CREAT|O_APPEND|O_RDWR,0644);
    if(fd<0){
    perror("Eroare deschidere fisier\n");
    exit(-1);
    }
  }

  char aux[200];
int k= snprintf(aux, sizeof(aux),"ID: %d\nNume: %sCoordonate: %.2f %.2f\nIndiciu: %sValoare:%d\n\n",comoara.ID, comoara.Nume,comoara.X_coordinate, comoara.Y_coordinate,comoara.Clue, comoara.Value);

 if( write(fd, aux, k)==-1){
   perror("Eroare la scrierea comorii in fisierul sau\n");
 }

    
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

  char buffer[50];
  struct stat st;
  sprintf(buffer,"Nume hunt: %s\n",huntID);
  if(write(1,buffer,strlen(buffer))==-1){
    perror("Eroare la afisarea numelui hunt-ului\n");
    exit(-1);
  }
  memset(buffer,0,sizeof(buffer));
  sprintf(buffer,"./%s/treasures",huntID);
  if(lstat(buffer,&st)==-1){
    perror("Eroare la preluarea datelor cu lstat\n");
    exit(-5);
  }

  char aux[200];
  sprintf(aux,"Dimensiunea fisierului: %ld\n",st.st_size);
  if(write(1,aux,strlen(aux))<0){
    perror("Eroare la afisarea dimensiunii fisierului");
    exit(-1);
  }
  memset(aux,0,sizeof(aux));
  
  sprintf(aux,"Ultima modificare: %s\n",ctime(&st.st_mtime)); //functia ctime ajuta sa faca din acel camp ceva relevant
  if(write(1,aux,strlen(aux))<0){
      perror("Eroare in scriere pe ecran\n");
      exit(-1);
    }
  
memset(aux,0,sizeof(aux));

  int fd=open(buffer,O_RDONLY);
  if(fd<0){
    perror("Eroare la deschiderea fisierului\n");
    exit(-1);
  }

    int bytes_read;
    while((bytes_read=read(fd,aux,sizeof(aux))) >0){
      if(write(1,aux,strlen(aux))<0){
	perror("Eroare la scrierea pe ecran a comorilor\n");
	exit(-1);
      }
      memset(aux,0,sizeof(aux));
    }

    if(close(fd)<0){
      perror("Eroare la inchiderea fisierului\n");
      exit(-1);
    }
  
}




void view(const char *huntID,int ID){

  DIR *dir=opendir(huntID);
  char Treasure_file[200];

  if(dir == NULL){
    perror("Nu exista acest hunt\n");
  }

  sprintf(Treasure_file,"./%s/treasures",huntID);
  
  int fd=open(Treasure_file,O_RDONLY);
  if(fd<0){
    perror("Nu s-a gasit aceasta comoara\n");
    exit(-1);
  }

  char buffer[512];
  char aux[10];
  sprintf(aux,"ID: %d",ID);
  char ch;
  int i=0;
  int k=0;
  
  
  while(read(fd,&ch,1)==1 ) {

    buffer[i]=ch;
    i++;
    
    if(buffer[i-1]=='\n' && buffer[i-2]=='\n'){
      
    if(strstr(buffer,aux)!=NULL){
      k=1;
      if(write(1,buffer,strlen(buffer)-1)<0){
      perror("Eroare printare pe ecran \n");
      exit(-1);
    }
      break;
    }
    memset(buffer,0,sizeof(buffer));
    i=0;
    }
    }
  
  if(k==0){
    write(1,"Comoara nu a fost gasita\n",strlen("Comoara nu a fost gasita\n"));
  }

  if( close(fd)==-1){
  perror("Eroare inchidere fisier\n");
  exit(-2);
}
  }


void remove_hunt(const char *huntID){

struct dirent **nameList;
 char buff[100];
 sprintf(buff,"./%s",huntID);
  int n;
  n=scandir(buff,&nameList,NULL,alphasort);
  if(n<0){
    perror("Eroare la citirea intrarilor in director\n");
  }
  else{

    free(nameList[0]); //nu avem nevoie de aceste intrari
    free(nameList[1]); //sunt directoarele speciale "." si ".." 
    
    for(int i=2;i<n;i++){
      char buffer[512];
      sprintf(buffer,"%s/%s",buff,nameList[i]->d_name);

      if(unlink(buffer) <0){
	perror("Eroare la stergerea fisierului\n");
	exit(-2);
      }
      free(nameList[i]);
    }
    free(nameList);
  }

  if(rmdir(buff)<0){
    perror("Eroare la stergerea directorului\n");
    exit(-3);
  }
  char root_log[100];
  sprintf(root_log,"./root_logg_file-%s",huntID);
  if(unlink(root_log)<0){
    perror("Eroare la stergerea fisierului de log din root");
    exit(-2);
  } 
}

void remove_treasure(const char *huntID,int ID){

  char source[50];
  char aux[50];
  char log_file[50];
 
  sprintf(source,"./%s/treasures",huntID);
  sprintf(aux,"./%s/aux",huntID);
  sprintf(log_file,"./%s/logg_hunt-%s",huntID,huntID);
  
  char ID_wanted[10];
  sprintf(ID_wanted,"ID: %d",ID);

  int fd=open(source,O_RDONLY);
  if(fd<0){
    perror("Eroare la dechiderea fisierului pentru citire\n");
    exit(-1);
  }

  int fd_aux=open(aux,O_CREAT|O_APPEND|O_WRONLY,0644);
  if(fd_aux<0){
    perror("Eroare la deschiderea fisierului temporar pentru scriere\n");
    exit(-1);
  }

  char buffer[512];
  char c;
  int i=0;
  int k=0;

  while(read(fd,&c,1)==1){
    buffer[i++]=c;

    if(i>=2 && buffer[i-1]=='\n' && buffer[i-2]=='\n'){

      if(strstr(buffer,ID_wanted)==NULL){
	
	if(write(fd_aux,buffer,strlen(buffer))<0){
	  perror("Eroare la scrierea in fisierul auxiliar\n");
	  exit(-1);
	}
      }
      else{
	k=1;
      }
      memset(buffer,0,sizeof(buffer));
      i=0;
    }
  }

  if(k==0){
    if( write(1,"Nu s-a gasit comoara dorita pentru sters\n",strlen("Nu s-a gasit comoara dorita pentru sters\n"))<0){
      perror("Eroare la afisarea mesajului\n");
      exit(-2);
    }
    unlink(aux);
    return;
  }

  int fd_log=open(log_file,O_APPEND|O_WRONLY);
  if(fd_log<0){
    perror("Eroare la deschiderea fisierului de logg\n");
    exit(-1);
  }

  char deleted_info[100];
  sprintf(deleted_info,"S-a sters comoara cu ID-ul %d\n",ID);
  if(write(fd_log,deleted_info,strlen(deleted_info))<0){
    perror("Eroare la scrierea in fisierul logg\n");
    exit(-2);
  }
  
  if(close(fd_log) <0){
    perror("Eroare la inchiderea fisierului logg\n");
    exit(-2);
  }

  if(close(fd)<0){
    perror("Eroare la inchiderea fisierului de citire\n");
    exit(-2);
  }
  if(close(fd_aux)<0){
    perror("Eroare la inchiderea fisierului auxiliar pentru citire");
    exit(-2);
  }

  if(rename(aux,source) ==-1){
    perror("Eroare la inlocuirea fisierului auxiliar cu cel original\n");
    exit(-1);
  }
  
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
 else if(strcmp(argv[1],"--remove_hunt")==0){
   if(argc!=3){
     perror("Utilizare incorecta: ./apelare_program --remove_hunt hunt_id");
     exit(-3);
   }
   else{
     remove_hunt(argv[2]);
   }
 }
 else if(strcmp(argv[1],"--remove_treasure")==0){
   if(argc!=4){
     perror("Utilizare eronata: apelare_program --remove_treasure hunt_id id");
   }
   else{
     remove_treasure(argv[2],atoi(argv[3]));
   }
 }
 else{
   perror("Comanda necunoscuta\n Exemplu de comanda: ./apelare_program --add h1");
   exit(-1);
 }


  return 0;
}
