#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <err.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
//#include "shared_memory.h"
#include "dataframe.h"
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<semaphore.h>
#include<fcntl.h>

int terminieren=0;

void sigTermHandler(int signo) {
    if (signo==SIGTERM) {
        terminieren=1;
    }
}
//IP Addresse des EduTerm wird als Argument übergeben
int main (int argc, char **argv){
//EMSYS cheeta42
    signal(SIGTERM, sigTermHandler);
    int error;
    //Komplettes UDP Socket setup
    //Muss nicht mehr bearbeitet werden!
    const char *address = argv[1];
    const char *port = "49152";
    struct addrinfo hints;
    struct addrinfo *result, *current_result;


    if (argc < 2 || strlen(argv[1]) < 1)
        errx(EXIT_FAILURE, "Missing address as argument");

    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_DGRAM; // Datagram socket
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          // Any protocol

    error = getaddrinfo(address, port, &hints, &result);
    if (error != 0)
        err(EXIT_FAILURE, "getaddrinfo: %s\n", gai_strerror(error));

    /* getaddrinfo() returns a list of address structures.  Try each address
     * until we successfully connect(2). If socket(2) (or connect(2)) fails,
     * we (close the socket and) try the next address. */

    int socket_fd;
    for (current_result = result; current_result != NULL; current_result = current_result->ai_next) {
        socket_fd = socket(current_result->ai_family, current_result->ai_socktype, current_result->ai_protocol);
        if (socket_fd == -1)
            continue;
        //Try connecting to the suggested socket
        if (connect(socket_fd, current_result->ai_addr, current_result->ai_addrlen) != -1)
            break;  // Success
        close(socket_fd);
    }
    if (current_result == NULL)  // No address succeeded
        errx(EXIT_FAILURE, "Could not connect");
    freeaddrinfo(result);  //No longer needed

    dataframe frame;


    //TODO: Shared Memory auslesen, frame(s) befüllen und absenden
    struct data {
        float cpu;
        float ram;
        float uptime;
        int prozesse;
    };

    sem_t *shmzugriff=sem_open("/shmzugriff", O_CREAT, 0666, 1);
    if (shmzugriff== SEM_FAILED) {
        perror("Fehler bei sem_open");
        return EXIT_FAILURE;
    }

    sem_t *gelesen=sem_open("/gelesen", O_CREAT, 0666, 0);
    if (gelesen== SEM_FAILED) {
        perror("Fehler bei sem_open");
        return EXIT_FAILURE;
    }

    sem_t *geschrieben=sem_open("/geschrieben", O_CREAT, 0666, 0);
    if (geschrieben== SEM_FAILED) {
        perror("Fehler bei sem_open");
        return EXIT_FAILURE;
    }


    sem_wait(shmzugriff);
    int shm_id=shmget(345345,10000, 0666 | IPC_CREAT| IPC_EXCL);
    if (shm_id==-1) {
        shm_id=shmget(345345, 0, 0);
        if (shm_id<0) {
            perror("Fehler bei shmget in reader.c");
            return EXIT_FAILURE;
        }
    }
    void *shmaddress = shmat(shm_id, NULL, 0666 | IPC_CREAT| IPC_EXCL);
    sem_post(shmzugriff);
    sem_post(gelesen);
    frame.pageNo=0;
    while (terminieren==0) {
        struct data buf;
        sem_wait(geschrieben);
        sem_wait(shmzugriff);
        memcpy(&buf, shmaddress,sizeof(buf)); //maximal Größe des structs lesen
        sem_post(shmzugriff);
        sprintf(frame.page[0], "RAM: %.2f", buf.ram);
        sprintf(frame.page[1], "Cpu-Last: %.2f", buf.cpu);
        sprintf(frame.page[2], "Prozesse: %d", buf.prozesse);
        sprintf(frame.page[3], "Uptime: %0.2fh", buf.uptime);
        printf("%s %s %s %s \n", frame.page[0], frame.page[1], frame.page[2], frame.page[3]);
        sem_post(gelesen);
        if (write(socket_fd, &frame, sizeof(frame)) != sizeof(frame))
            err(EXIT_FAILURE, "partial/failed write\n");
        sleep(1);
    }
    shmdt(shmaddress);

    //ToDo Ende

    //Hier wird ein frame an das EduTerm gesendet!
    if (write(socket_fd, &frame, sizeof(frame)) != sizeof(frame))
        err(EXIT_FAILURE, "partial/failed write\n");

    //UDP Socket wieder schliessen
    close(socket_fd);

}
