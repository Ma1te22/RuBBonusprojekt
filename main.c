#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <statgrab.h>

int main(void) {
    if (sg_init(0) != SG_ERROR_NONE) {
        fprintf(stderr, "sg_init failed\n");
        return EXIT_FAILURE;
    }

    size_t hostN, cpuN, memN, diskN = 0, netN = 0;

    sg_host_info *host = sg_get_host_info(&hostN);
    sg_cpu_percents *cpu = sg_get_cpu_percents(&cpuN);
    sg_mem_stats *mem = sg_get_mem_stats(&memN);

    sg_get_disk_io_stats_diff(&diskN);//diskN enthält Anzahl der Disks, die das System hat
    sg_get_network_io_stats_diff(&netN);
    sleep(1);

    sg_disk_io_stats *disk = sg_get_disk_io_stats_diff(&diskN);
    sg_network_io_stats *net = sg_get_network_io_stats_diff(&netN);

    if (!host || !cpu || !mem || hostN != 1 || memN != 1) { //!host bedeuted host ist null
        fprintf(stderr, "Error reading stats\n");
        sg_shutdown();
        return EXIT_FAILURE;
    }

    // Berechnung von Festplatten- und Netzwerkauslastung
    unsigned long long diskRead = 0, diskWrite = 0;
    unsigned long long netRx = 0, netTx = 0;

    for (size_t i = 0; i < diskN; i++) {
        if (disk[i].disk_name[0] == 'r' || disk[i].disk_name[0] == 'l')
            continue;

        diskRead += disk[i].read_bytes;
        diskWrite += disk[i].write_bytes;
    }

    for (size_t i = 0; i < netN; i++) {
        netRx += net[i].rx;
        netTx += net[i].tx;
    }

    // Ausgabe, die Werte müssen noch normalisiert werden
    printf("Cpu %.2f%% Uptime %.2fh\n",
           cpu->idle,
           (double)host->uptime);

    printf("Ram %.2f%% Cache %.2f%%\n",
           (double)mem->used,
           (double)mem->cache);

    printf("Disk R %llu W %llu\n",
           diskRead,
           diskWrite);

    printf("Net Rx %llu Tx %llu\n",
           netRx,
           netTx);

    sg_shutdown();
    return EXIT_SUCCESS;
}
