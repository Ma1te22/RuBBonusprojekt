#include <string.h>

//Eigenschaften des EduTerm displays:
//4 Zeilen pro Seite
#define PAGE_ROWS 4
//20 Zeichen pro Zeile
#define PAGE_COLUMNS 20
//Eine page besteht also aus 4*(20+1) chars (NULL byte beachten!)
typedef char page_t[PAGE_ROWS][PAGE_COLUMNS +1];

//Das EduTerm akzeptiert im ursprungszustand diese Datenstruktur:
typedef struct {
    page_t page;
    int pageNo; //Seitennummer, für das Paging. Beginnt bei 0, geht standardmäßig bis 9
} dataframe;

//Funktion um eine Seite zu leeren
void pageclear (page_t *page){
    memset(page, ' ', sizeof(page));
    for (int i=0; i<PAGE_ROWS; i++)
        *page[i][PAGE_COLUMNS] = '\0';
}
