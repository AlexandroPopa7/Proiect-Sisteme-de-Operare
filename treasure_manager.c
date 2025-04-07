#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

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
  printf("%d",comoara.Value);
  return comoara;
}


void add(const char *huntID){

  

  
}

void list(const char *huntID){
  
}

void view(const char *huntID,int ID){

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

  new_treasure();
  return 0;
}
