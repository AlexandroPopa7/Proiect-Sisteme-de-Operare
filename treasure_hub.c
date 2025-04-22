#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

pid_t monitor_pid = -1;

void handle_sigchld(int sig) {
    int status;
    waitpid(monitor_pid, &status, 0);
    printf("Monitor terminated with status %d\n", status);
    monitor_pid = -1;
}

void start_monitor() {
    if (monitor_pid != -1) {
      if(write(1,"Monitorul e deja in functiune\n",strlen("Monitorul e deja in functiune\n"))<0){
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
        printf("Procesul fiu monitor PID: %d\n", getpid());
        while (1) pause();
    }

    printf("Proces parinte PID %d\n", monitor_pid);
}

void stop_monitor() {
    if (monitor_pid == -1) {
        printf("Monitorul nu functioneaza\n");
        return;
    }

    kill(monitor_pid, SIGTERM); 
}

int main(int argc, char **argv) {
    if (argc != 1) {
        perror("Eroare utilizare program: ./apelare_program");
        exit(-1);
    }

    struct sigaction sa;
    sa.sa_handler = handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGCHLD, &sa, NULL);

    char input[256];

    while (1) {
      if(write(1,"Command: ",strlen("Command: "))<0){
	perror("Eroare scriere terminal\n");
	exit(-1);
      }
	fflush(stdout);

        if(read(1,input, sizeof(input))<0)
	  {perror("Eroare preluare comanda din terminal\n");
	    break;
	  }
       input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "start_monitor") == 0) {
            start_monitor();
        } else if (strcmp(input, "stop_monitor") == 0) {
            stop_monitor();
        } else if (strcmp(input, "exit") == 0) {
            if (monitor_pid != -1) {
	      if(write(1,"Monitorul este inca in functiune, nu se poate iesi din program\n",strlen("Monitorul este inca in functiune, nu se poate iesi din program\n"))<0){
		perror("Eroare scriere ecran\n");
		exit(-1);
	      }
																					  
            } else {
	      if(write(1,"Iesire treasure_hub...\n",strlen("Iesire treasure_hub...\n"))<0){
		perror("Eroare scriere pe ecran\n");
		exit(-1);
	      }
                break;
            }
        } else {
	  if(write(1,"Unknown command\n",strlen("Unknown command\n"))<0){
	    perror("Eroare scriere ecran\n");
	    exit(-1);
	  }
        }
    }

    return 0;
}
