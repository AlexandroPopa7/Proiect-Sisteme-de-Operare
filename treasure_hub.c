#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>

#define TREASURE_SPACE 6

pid_t monitor_pid = -1; // Indicates monitor is stopped when -1
volatile sig_atomic_t done = 0;
int monitor_shutting_down = 0; // 0 means not in the process of shutting down
int pfd[2];

void handler_done() {
    done = 1;
}

void stop_monitor() {
    if (monitor_shutting_down) {
        if (write(1, "Monitorul e in proces de inchidere.Nu se accepta alte comenzi\n",
                  strlen("Monitorul e in proces de inchidere.Nu se accepta alte comenzi\n")) < 0) {
            perror("Eroare la scrierea pe ecran\n");
            exit(-1);
        }
        return;
    }

    if (monitor_pid == -1) {
        printf("Monitorul nu este in functiune\n");
        return;
    }

    monitor_shutting_down = 1;
    if (write(1, "Monitorul este in curs de oprire...\n",
              strlen("Monitorul este in curs de oprire...\n")) < 0) {
        perror("Eroare la scrierea pe ecran a mesajului de oprire monitor\n");
        exit(-1);
    }
    if (kill(monitor_pid, SIGTERM) < 0) {
        perror("Eroare la transmiterea semnalului de oprire catre monitor\n");
        exit(-1);
    }

    if (usleep(5000000) == -1) {
        perror("Eroare la usleep\n");
        exit(-1);
    }
    monitor_pid = -1;
    monitor_shutting_down = 0;
}

void list_hunts() {
    if (monitor_shutting_down) {
        if (write(1, "Monitorul e in proces de inchidere.Nu se accepta alte comenzi\n",
                  strlen("Monitorul e in proces de inchidere.Nu se accepta alte comenzi\n")) < 0) {
            perror("Eroare la scrierea pe ecran\n");
            exit(-1);
        }
        return;
    }
    close(pfd[0]);
    struct dirent **hunts;
    int k = scandir(".", &hunts, NULL, alphasort);
    if (k < 0) {
        perror("Eroare la citirea fisierelor din director \n");
        exit(-1);
    }

    for (int i = 2; i < k; i++) {
        struct stat path;
        if (stat(hunts[i]->d_name, &path) < 0) {
            perror("Eroare stat in list_hunts\n");
            exit(-1);
        }

        if (S_ISDIR(path.st_mode)) {
            char buff[200] = "\n Nume hunt: ";
            strcat(buff, hunts[i]->d_name);
            if (write(pfd[1], buff, strlen(buff)) < 0) {
                perror("Eroare scriere pe ecran\n");
                exit(-1);
            }
            write(pfd[1], "\n", strlen("\n"));
            struct dirent **dir;
            int p = scandir(hunts[i]->d_name, &dir, NULL, alphasort);
            if (p < 0) {
                perror("Eroare la accesarea directoarelor hunt\n");
                exit(-1);
            }
            for (int j = 2; j < p; j++) {
                int lines = 0;
                if (strcmp(dir[j]->d_name, "treasures") == 0) {
                    char buff[1024];
                    sprintf(buff, "./%s/%s", hunts[i]->d_name, dir[j]->d_name);

                    int fd = open(buff, O_RDWR);
                    if (fd < 0) {
                        perror("Eroare la deschiderea fisierului\n");
                        exit(-1);
                    }
                    char ch;
                    while (read(fd, &ch, 1) == 1) {
                        if (ch == '\n') {
                            lines++;
                        }
                    }
                    int comori = lines / TREASURE_SPACE;
                    char aux[50];
                    sprintf(aux, "Numarul de comori: %d\n", comori);
                    if (write(pfd[1], aux, strlen(aux)) < 0) {
                        perror("Eroare afisare numar de comori din hunt\n");
                        exit(-1);
                    }
		    if(close(fd)<0){
		      perror("Eroare inchidere fisier\n");
		      exit(-1);
		    }
                }
            }

            for (int j = 0; j < p; j++) {
                free(dir[j]);
            }
            free(dir);
        }
    }
    for (int i = 0; i < k; i++) {
        free(hunts[i]);
    }
    free(hunts);
   
    kill(getppid(), SIGTERM);
}

void view_treasure() {
    if (monitor_shutting_down) {
        if (write(1, "Monitorul e in proces de inchidere.Nu se accepta alte comenzi\n",
                  strlen("Monitorul e in proces de inchidere.Nu se accepta alte comenzi\n")) < 0) {
            perror("Eroare la scrierea pe ecran\n");
            exit(-1);
        }
        return;
    }

    char executabil[50];
    char comoara[50];
    char hunt[50];

    if (write(1, "Introduceti numele hunt-ului dorit: \n",
              strlen("Introduceti numele hunt-ului dorit: \n")) < 0) {
        perror("Eroare afisare mesaj pentru view_treasure\n");
        exit(-1);
    }
    if (read(0, hunt, sizeof(hunt)) < 0) {
        perror("Eroare la citirea hunt-ului dorit\n");
        exit(-1);
    }
    hunt[strcspn(hunt, "\n")] = '\0';

    struct stat path;
    if (stat(hunt, &path) < 0 || !S_ISDIR(path.st_mode)) {
        if (write(pfd[1], "Eroare: Hunt-ul specificat nu exista\n",
                  strlen("Eroare: Hunt-ul specificat nu exista\n")) < 0) {
            perror("Eroare scriere pe ecran\n");
            exit(-1);
        }
        kill(getppid(), SIGTERM);
        return;
    }

    if (write(1, "Introduceti ID-ul treasure-ului dorit: \n",
              strlen("Introduceti ID-ul treasure-ului dorit: \n")) < 0) {
        perror("Eroare afisare mesaj pentru view_treasure\n");
        exit(-1);
    }
    if (read(0, comoara, sizeof(comoara)) < 0) {
        perror("Eroare la citirea treasure-ului dorit\n");
        exit(-1);
    }
    comoara[strcspn(comoara, "\n")] = '\0';

    if (write(1, "Introduceti cum doriti sa se numeasca executabilul format(se va compila treasure_manager):  \n",
              strlen("Introduceti cum doriti sa se numeasca executabilul format(se va compila treasure_manager):  \n")) < 0) {
        perror("Eroare afisare mesaj pentru view_treasure\n");
        exit(-1);
    }
    if (read(0, executabil, sizeof(executabil)) < 0) {
        perror("Eroare la citirea hunt-ului dorit\n");
        exit(-1);
    }
    if (write(1, "\n", strlen("\n")) < 0) {
        perror("Eroare scriere pe ecran\n");
        exit(-1);
    }
    executabil[strcspn(executabil, "\n")] = '\0';

    pid_t ch;
    if ((ch = fork()) < 0) {
        perror("Eroare crearea unui proces nou in view_treasure\n");
        exit(-1);
    }
    if (ch == 0) {
        execl("/usr/bin/gcc", "/usr/bin/gcc", "-Wall", "-o", executabil, "treasure_manager.c", NULL);
        perror("Eroare la compilarea din view_treasure\n");
        exit(-1);
    }
    wait(&ch);
    if ((ch = fork()) < 0) {
        perror("Eroare crearea unui proces nou\n");
        exit(-1);
    }
    char exec[50] = "./";
    strcat(exec, executabil);

    if (ch == 0) {
      close(pfd[0]);
      dup2(pfd[1],1);
      close(pfd[1]);
        execl(exec, exec, "--view", hunt, comoara, NULL);
        perror("Eroare executare treasure_manager\n");
        exit(-1);
    }
    wait(&ch);

    kill(getppid(), SIGTERM);
}

void list_treasures() {
    if (monitor_shutting_down) {
        if (write(1, "Monitorul e in proces de inchidere.Nu se accepta alte comenzi\n",
                  strlen("Monitorul e in proces de inchidere.Nu se accepta alte comenzi\n")) < 0) {
            perror("Eroare la scrierea pe ecran\n");
            exit(-1);
        }
        return;
    }

    char executabil[50];
    char hunt[50];

    if (write(1, "Introduceti numele hunt-ului dorit: \n",
              strlen("Introduceti numele hunt-ului dorit: \n")) < 0) {
        perror("Eroare afisare mesaj pentru view_treasure\n");
        exit(-1);
    }
    if (read(0, hunt, sizeof(hunt)) < 0) {
        perror("Eroare la citirea hunt-ului dorit\n");
        exit(-1);
    }
    hunt[strcspn(hunt, "\n")] = '\0';
    struct stat path;
    if (stat(hunt, &path) < 0 || !S_ISDIR(path.st_mode)) {
        if (write(pfd[1], "Hunt-ul specificat nu exista\n",
                  strlen("Hunt-ul specificat nu exista\n")) < 0) {
            perror("Eroare scriere pe ecran\n");
            exit(-1);
        }
        kill(getppid(), SIGTERM);
        return;
    }

    if (write(1, "Introduceti cum doriti sa se numeasca executabilul format(se va compila treasure_manager):  \n",
              strlen("Introduceti cum doriti sa se numeasca executabilul format(se va compila treasure_manager):  \n")) < 0) {
        perror("Eroare afisare mesaj pentru view_treasure\n");
        exit(-1);
    }
    if (read(0, executabil, sizeof(executabil)) < 0) {
        perror("Eroare la citirea hunt-ului dorit\n");
        exit(-1);
    }
    if (write(1, "\n", strlen("\n")) < 0) {
        perror("Eroare scriere pe ecran\n");
        exit(-1);
    }
    executabil[strcspn(executabil, "\n")] = '\0';

    pid_t ch;
    if ((ch = fork()) < 0) {
        perror("Eroare crearea unui proces nou in view_treasure\n");
        exit(-1);
    }
    if (ch == 0) {
        execl("/usr/bin/gcc", "/usr/bin/gcc", "-Wall", "-o", executabil, "treasure_manager.c", NULL);
        perror("Eroare la compilarea din view_treasure\n");
        exit(-1);
    }
    wait(&ch);
    if ((ch = fork()) < 0) {
        perror("Eroare crearea unui proces nou\n");
        exit(-1);
    }
    char exec[50] = "./";
    strcat(exec, executabil);

    if (ch == 0) {
      close(pfd[0]);
      dup2(pfd[1],1);
      close(pfd[1]);
        execl(exec, exec, "--list", hunt, NULL);
        perror("Eroare executare treasure_manager\n");
        exit(-1);
    }
    wait(&ch);

    kill(getppid(), SIGTERM);
}

void calculate_score() {
    if (monitor_shutting_down) {
        if (write(1, "Monitorul e in proces de inchidere.Nu se accepta alte comenzi\n",
                  strlen("Monitorul e in proces de inchidere.Nu se accepta alte comenzi\n")) < 0) {
            perror("Eroare la scrierea pe ecran\n");
            exit(-1);
        }
        return;
    }

    char executabil[50];
    if (write(1, "Introduceti cum doriti sa se numeasca executabilul format(se va compila calculate_score.c): \n",
              strlen("Introduceti cum doriti sa se numeasca executabilul format(se va compila calculate_score.c): \n")) < 0) {
      perror("Eroare afisare mesaj pentru calculate"); 
            exit(-1);
        }
    if (read(0, executabil, sizeof(executabil)) < 0) {
        perror("Eroare la citirea exec-ului dorit\n");
        exit(-1);
    }
    executabil[strcspn(executabil, "\n")] = '\0';
    if (write(1, "\n", strlen("\n")) < 0) {
        perror("Eroare scriere pe ecran\n");
        exit(-1);
    }

    struct dirent **files;
    int n=scandir(".",&files,NULL,alphasort);
    if(n<0){
      perror("Eroare scandir in calculate_score\n");
      exit(-1);
    }
    for(int i=2;i<n;i++){

      struct stat file;
      if(stat(files[i]->d_name,&file)<0){
	perror("Eroare stat in calculate_score");
	exit(-1);
      }
      if(S_ISDIR(file.st_mode )){
	pid_t ch;
	ch=fork();
	if(ch<0){
	  perror("Eroare in fork calculate_score\n");
	  exit(-1);
	}
	if(ch==0){
	  execl("/usr/bin/gcc","/usr/bin/gcc","-Wall","-o",executabil,"calculate_score.c",NULL);
	  perror("Eroare la compilarea din calculate_score\n");
	  exit(-1);
	}
	wait(&ch);
	if( (ch=fork()) <0){
    perror("Eroare crearea unui proces nou\n");
    exit(-1);
  }
  char exec[50]="./";
  strcat(exec,executabil);

  if(ch==0){
    close(pfd[0]);
    dup2(pfd[1], 1);
    close(pfd[1]);
    execl(exec,exec,files[i]->d_name,NULL);
    perror("Eroare executare calculate_score\n");
    exit(-1);
  }
  wait(&ch);
      }
      
    }
    
    for(int i=0;i<n;i++){
      free(files[i]);
    }
    free(files);
    kill(getppid(), SIGTERM);
}


void send_signal(int signal) {
    if (monitor_shutting_down) {
        if (write(1, "Monitorul e in proces de inchidere. Nu se accepta alte comenzi\n",
                  strlen("Monitorul e in proces de inchidere. Nu se accepta alte comenzi\n")) < 0) {
            perror("Eroare scriere ecran\n");
            exit(-1);
        }
        return;
    }

    if (monitor_pid == -1) {
        if (write(1, "Monitorul este oprit. Nu se poate realiza comanda ceruta\n",
                  strlen("Monitorul este oprit. Nu se poate realiza comanda ceruta\n")) < 0) {
            perror("Eroare la scrierea problemei din send_signal\n");
            exit(-1);
        }
        return;
    }
    kill(monitor_pid, signal);
}

void start_monitor() {
    if (monitor_shutting_down) {
        if (write(1, "Monitorul e in proces de inchidere.Nu se accepta alte comenzi\n",
                  strlen("Monitorul e in proces de inchidere.Nu se accepta alte comenzi\n")) < 0) {
            perror("Eroare la scrierea pe ecran\n");
            exit(-1);
        }
        return;
    }

    if (monitor_pid != -1) {
        if (write(1, "Monitorul e deja in functiune\n",
                  strlen("Monitorul e deja in functiune\n")) < 0) {
            perror("Eroare scriere pe ecran\n");
            exit(-1);
        }
        return;
    }

    monitor_pid = fork();
    if (monitor_pid < 0) {
        perror("Eroare la functia fork\n");
        exit(1);
    }
    if (monitor_pid == 0) {
        struct sigaction sg;
        sg.sa_handler = list_hunts;
        sigaction(SIGUSR1, &sg, NULL);
        sg.sa_handler = view_treasure;
        sigaction(SIGUSR2, &sg, NULL);
        sg.sa_handler = list_treasures;
        sigaction(SIGALRM, &sg, NULL);
        sg.sa_handler = calculate_score;
        sigaction(SIGPIPE, &sg, NULL);
        sg.sa_handler = SIG_DFL;
        sigaction(SIGTERM, &sg, NULL);
        while (1) pause();
    }
}

int main(int argc, char **argv) {
    if (argc != 1) {
        perror("Eroare utilizare program: ./apelare_program");
        exit(-1);
    }

    if(pipe(pfd)<0){
      perror("Eroare pipe\n");
      exit(-1);
    }
    
    char buffer[2048];
    memset(buffer,0,sizeof(buffer));
    struct sigaction sa;
    sa.sa_handler = handler_done;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);

    char input[256];

    while (1) {
        if (monitor_shutting_down) {
            if (write(1, "Monitorul e in proces de inchidere. Nu se accepta alte comenzi\n",
                      strlen("Monitorul e in proces de inchidere. Nu se accepta alte comenzi\n")) < 0) {
                perror("Eroare scriere ecran\n");
                exit(-1);
            }
            continue;
        }

        if (write(1, "Command: ", strlen("Command: ")) < 0) {
            perror("Eroare scriere terminal\n");
            exit(-1);
        }
        fflush(stdout);

        if (read(0, input, sizeof(input)) < 0) {
            perror("Eroare preluare comanda din terminal\n");
            break;
        }
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "start_monitor") == 0) {
            start_monitor();
        } else if (strcmp(input, "stop_monitor") == 0) {
            stop_monitor();
        } else if (strcmp(input, "list_hunts") == 0) {
            if (monitor_pid == -1) {
                if (write(1, "Monitorul este oprit. Nu se poate efectua operatia.\n",
                          strlen("Monitorul este oprit. Nu se poate efectua operatia.\n")) < 0) {
                    perror("Eroare scriere pe ecran\n");
                    exit(-1);
                }
            } else {
                done = 0;
                send_signal(SIGUSR1);
                while (done != 1) {
                    pause();
                }
	if(read(pfd[0], buffer, sizeof(buffer))<0){
		  perror("Eroare la citirea din pipe");
		  exit(-1);
		}
      write(1, buffer, strlen(buffer));
      memset(buffer, 0, sizeof(buffer));

            }
        } else if (strcmp(input, "view_treasure") == 0) {
            if (monitor_pid == -1) {
                if (write(1, "Monitorul este oprit. Nu se poate efectua operatia.\n",
                          strlen("Monitorul este oprit. Nu se poate efectua operatia.\n")) < 0) {
                    perror("Eroare scriere pe ecran\n");
                    exit(-1);
                }
            } else {
                done = 0;
                send_signal(SIGUSR2);
                while (!done) {
                    pause();
                }
		if(read(pfd[0], buffer, sizeof(buffer))<0){
		  perror("Eroare la citirea din pipe");
		  exit(-1);
		}
      write(1, buffer, strlen(buffer));
      memset(buffer, 0, sizeof(buffer));
		
            }
        } else if (strcmp(input, "list_treasures") == 0) {
            if (monitor_pid == -1) {
                if (write(1, "Monitorul este oprit. Nu se poate efectua operatia.\n",
                          strlen("Monitorul este oprit. Nu se poate efectua operatia.\n")) < 0) {
                    perror("Eroare scriere pe ecran\n");
                    exit(-1);
                }
            } else {
                done = 0;
                send_signal(SIGALRM);
                while (done != 1) {
                    pause();
                }
	if(read(pfd[0], buffer, sizeof(buffer))<0){
		  perror("Eroare la citirea din pipe");
		  exit(-1);
		}
      write(1, buffer, strlen(buffer));
      memset(buffer, 0, sizeof(buffer));
            }
        } else if (strcmp(input, "calculate_score") == 0) {
            if (monitor_pid == -1) {
                if (write(1, "Monitorul este oprit. Nu se poate efectua operatia.\n",
                          strlen("Monitorul este oprit. Nu se poate efectua operatia.\n")) < 0) {
                    perror("Eroare scriere pe ecran\n");
                    exit(-1);
                }
            } else {
                done = 0;
                send_signal(SIGPIPE);
                while (done != 1) {
                    pause();
                }
if(read(pfd[0], buffer, sizeof(buffer))<0){
		  perror("Eroare la citirea din pipe");
		  exit(-1);
		}
      write(1, buffer, strlen(buffer));
      memset(buffer, 0, sizeof(buffer));
            }
        } else if (strcmp(input, "exit") == 0) {
            if (monitor_pid != -1) {
                if (write(1, "Monitorul este inca in functiune, nu se poate iesi din program\n",
                          strlen("Monitorul este inca in functiune, nu se poate iesi din program\n")) < 0) {
                    perror("Eroare scriere ecran\n");
                    exit(-1);
                }
            } else {
                if (write(1, "Iesire treasure_hub...\n",
                          strlen("Iesire treasure_hub...\n")) < 0) {
                    perror("Eroare scriere pe ecran\n");
                    exit(-1);
                }
		close(pfd[1]);
		close(pfd[0]);
                break;
            }
        } else {
            if (write(1, "Unknown command\n", strlen("Unknown command\n")) < 0) {
                perror("Eroare scriere ecran\n");
                exit(-1);
            }
        }
    }

    return 0;
}
