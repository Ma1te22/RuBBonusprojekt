//
// Created by HoffmannAdm on 12.04.2024.
//

#ifndef EDUTERM_MONITOR_H
#define EDUTERM_MONITOR_H

#include <cstdint>

constexpr uint16_t MY_UDP_PORT = 49152;

typedef struct {
    int32_t cid;      // client's pid
    char fname[12];   // file name
    int64_t size;     // payload size
    char ctime[25];   // human-readable time string
    char logname[23]; // user´s login name
} job_s;


#endif //EDUTERM_MONITOR_H
//RUBBONUSPROJEKT_MONITOR_H
