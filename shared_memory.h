//Implementierungsvorschlag, ihr könnt gerne abweichen wie auch immer ihr wollt.

#include "semaphore.h"


//Öffnet den shared memory und gibt den Anfang des Inhalts zurück
shmcontent *getSharedMemory();

//Shared memory erstellen/öffnen und den Anfang des Inhalts zurückgeben.
shmcontent *initSharedMemory();

//Shared memory vom Prozess lösen/schliessen
void detachSharedMemory (shmcontent *addr);

