#include <statgrab.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<string.h>
#include<semaphore.h>
#include<fcntl.h>
#include<signal.h>


 typedef struct {
     float memtotal;
     float cpu;
     float ram;
     float uptime;
     int prozesse;
}data;

int terminieren=0;

void sigTermHandler(int signo) {
    if (signo==SIGTERM) {
        terminieren=1;
    }
}

int main (){
    signal(SIGTERM, sigTermHandler);

    size_t cpuN, memN, hostN;

    if (sg_init(0) != SG_ERROR_NONE) {
        fprintf(stderr, "sg_init failed\n");
        return EXIT_FAILURE;
    }

    sem_t *shmzugriff=sem_open("/shmzugriff", O_CREAT, 0666, 1);
    if (shmzugriff == SEM_FAILED) {
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
    int shm_id=shmget(345345, sizeof(data), 0666 | IPC_CREAT| IPC_EXCL);
    if (shm_id==-1) {
        shm_id=shmget(345345, 0, 0);
        if (shm_id<0) {
            sem_post(shmzugriff);
            perror("Fehler bei shmget in reader.c");
            return EXIT_FAILURE;
        }
    }
    void *address;
    address =shmat(shm_id, NULL, 0666 | IPC_CREAT| IPC_EXCL);
    sem_post(shmzugriff);


    while(terminieren==0) {
        sem_wait(gelesen);
        sg_cpu_percents *cpu=sg_get_cpu_percents(&cpuN); //cpu zeigt auf Struktur
        sg_mem_stats *mem = sg_get_mem_stats(&memN);
        sg_host_info *host = sg_get_host_info(&hostN);
        sg_process_count *ps = sg_get_process_count();

        if (!cpu || !mem || !host || !ps) {
            perror("Fehler bei auslesen der Systemdaten");
            sg_shutdown();
            return EXIT_FAILURE;
        }

        data aktData={
            mem->total,
            100-((*cpu).idle),
            100*((float)mem->used)/((float)mem->total),
            (host->uptime)/3600.0,
            ps->total
        };
        sem_wait(shmzugriff);
        //printf("geschrieben");
        memcpy(address, &aktData, sizeof(aktData));
        sem_post(shmzugriff);
        sem_post(geschrieben);

        sleep(1);
    }

    sg_shutdown();
    shmdt(address);
    sem_close(shmzugriff);
    sem_close(geschrieben);
    sem_close(gelesen);

}
