#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

char User[50][50];
char Value[50]={0};
int index1=0;

int main(int argc, char **argv) {

  if(argc!=2){
    perror("Utilizare incorecta a programului: apel_program hunt\n");
    exit(-1);
  }
  
  
    struct dirent **hunts;
    int n = scandir(".", &hunts, NULL, alphasort);
    if (n < 0) {
        perror("Eroare la scandir\n");
        exit(-1);
    }

    for (int i = 2; i < n; i++) {

      if(strcmp(hunts[i]->d_name,argv[1])==0){
      
        struct stat path;
        if (stat(hunts[i]->d_name, &path) < 0) {
            perror("Eroare stat\n");
            continue;
        }

        if (S_ISDIR(path.st_mode )) {
            char treasure_path[600];
	    char nume[300];
	    sprintf(nume,"Nume hunt: %s\n",hunts[i]->d_name);
            snprintf(treasure_path, sizeof(treasure_path), "./%s/treasures", hunts[i]->d_name);
	    if(write(1,nume,strlen(nume))<0){
	      perror("Eroare scriere pe ecran");
	    }
            int fd = open(treasure_path, O_RDONLY);
            if (fd < 0) {
                fprintf(stderr, "Eroare la deschiderea fisierului %s\n", treasure_path);
                continue;
            }

            
            char buffer[256] = {0};
            char c;
            int j = 0;
	    int indexAux=-1;
            while (read(fd, &c, 1) == 1) {
                buffer[j++] = c;
		
                if (c == '\n') {
		  if(strstr(buffer,"Nume: ")){
		    char aux[50];
		    strcpy(aux,buffer+6);
		    aux[strlen(aux)-1]='\0';
		    int SW=0;
		    for(int k=0;k<index1;k++){
		      if(strcmp(User[k],aux)==0){
			indexAux=k;
			SW=1;
			break;
		      }
		    }
		    if(SW==0){
		      indexAux=index1;
		      strcpy(User[index1],aux);
		      User[index1][sizeof(User[index1])-1]='\0';
		      index1++;
		    }
		  }
                    else if (strstr(buffer, "Valoare")) {
                        int value = 0;
                        char *value_str = buffer + 8;
                        while (*value_str >= '0' && *value_str <= '9') {
                            value = value * 10 + (*value_str - '0');
                            value_str++;
                        }
                        //total_score += value;
			Value[indexAux]+=value;
                    }
                    memset(buffer, 0, sizeof(buffer));
                    j = 0;
                }
            }
            if(close(fd)<0){
	      perror("Eroare la inchiderea fisierului\n");
	      exit(-1);
	    }

	    for (int s = 0; s < index1; s++) {
        char output[512];
        int len = sprintf(output, "%s : %d\n", User[s], Value[s]);
      
        if (write(1, output, len) < 0) {
            perror("Eroare la scriere");
            exit(-1);
        }
    }	    
	    /*
            char output[512];
            snprintf(output, sizeof(output), "Hunt %s: %d\n", hunts[i]->d_name, total_score);
            if (write(1, output, strlen(output)) < 0) {
                perror("Eroare la scrierea scorului\n");
                exit(-1);
            } */
	}
        }
    }

    for (int i = 0; i < n; i++) {
        free(hunts[i]);
    }
    free(hunts);

    return 0;
}
