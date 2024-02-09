#include <stdio.h>
#include <string.h>
#include "subwayADT.h"
#include "htmlTable.h"

#define MAX_CHARS 50
#define DELIM ";"
#define CHANGE_LINE "\n"
#define DATE_DELIM "/"
#define HOUR_DELIM ":"
#define CANTPERIODS 4 
#define CANTWEEKDAYS 7
#define OPENBR '('
#define SPACE ' '

subADT readStations(FILE * stations, subADT sub);
void readTurnstiles(subADT sub, FILE * turnstiles);
char * copyName(char * name, char * aux);
void joinStationLine(char * res, char * station, char * line);

void query1(subADT sub);
void query2(subADT sub);
void query3(subADT sub);
void query4(subADT sub);


int main(int numArg, char * argv[]){
    if(numArg < 3 && numArg > 5){
        fprintf(stderr, "Error, amount of arguments not valid");
        exit(ARGERR);
    }

    FILE * fileTurnstiles = fopen(argv[1], "rt");
    FILE * fileStations = fopen(argv[2], "rt");

    if(fileTurnstiles == NULL || fileStations == NULL){
        fprintf(stderr, "Error, could not open files");
        exit(OPENERR);
    }
    
    int year_start = 0, year_end = 0;

    if (numArg == 4) {
        year_start = atoi(argv[3]);
        if (year_start <= 0) {
            fprintf(stderr, "Error, imput date not valid");
            exit(ERRDATE);
        }
    }else if (numArg == 5) {
        year_end = atoi(argv[4]);
        if (year_end <= 0 && year_start <= 0 && year_start >= year_end) {
            fprintf(stderr, "Error, imput date not valid");
            exit(ERRDATE);
        }
    }

    subADT sub = newSub(year_start, year_end);
    if(sub == NULL){
        fprintf(stderr, "Error, could not assign memory");
        exit(MEMERR);
    }

    sub = readStations(fileStations, sub);
    readTurnstiles(sub, fileTurnstiles);

    fclose(fileTurnstiles);
    fclose(fileStations);

    query1(sub);
    query2(sub);
    query3(sub);
    query4(sub);

    free(sub);
}

subADT readStations(FILE * stations, subADT sub){
    char text[MAX_CHARS];

    fscanf(stations, "%s\n", text);

    char * temp;
    char * name = NULL;
    while(fgets(text, MAX_CHARS, stations) != NULL){
        temp = strtok(text, DELIM);
        if(temp == NULL){
            fprintf(stderr, "Token error");
            exit(TOKENERR);
        }
        size_t id = atoi(temp);

        name = copyName(name, NULL);

        temp = strtok(NULL, DELIM);
        char line = temp[0];

        strtok(NULL, CHANGE_LINE);

        addStations(sub, line, name, id);
    }
    free(name);
    return sub;
}

void readTurnstiles(subADT sub, FILE * turnstiles){
    char text[MAX_CHARS];
    char day, month, start, end;
    size_t year, id, numPassen;
    char * temp;
    fscanf(turnstiles, "%s\n", text);

    while(fgets(text, MAX_CHARS, turnstiles) != NULL){
        temp = strtok(text, DATE_DELIM);
        if(temp == NULL){
            fprintf(stderr, "Token error");
            exit(TOKENERR);
        }
        day = atoi(temp);

        temp = strtok(NULL, DATE_DELIM);
        month = atoi(temp);

        temp = strtok(NULL, DELIM);
        year = atoi(temp);

        temp = strtok(NULL, HOUR_DELIM);
        start = atoi(temp);
        strtok(NULL, DELIM); //salteo los minutos

        temp = strtok(NULL, HOUR_DELIM);
        end = atoi(temp);
        strtok(NULL, DELIM); //salteo los minutos

        temp = strtok(NULL, DELIM);
        id = atoi(temp);

        temp = strtok(NULL, CHANGE_LINE);
        numPassen = atoi(temp);

        addDataTrips(sub, day, month, year, id, numPassen, start, end);
    }
}

char * copyName(char * name, char * aux){
    char * tok = strtok(aux, DELIM);
    if(tok == NULL){
        fprintf(stderr, "Token error");
        exit(TOKENERR);
    }

    name = realloc(name, strlen(tok)+1);
    if(name == NULL){
        fprintf(stderr, "Error, could not assign memory");
        exit(MEMERR);    
    }
    strcpy(name, tok);

    return name;
}


void query1(subADT sub){
    FILE * query1Arch = fopen("query1.csv", "wt");
    htmlTable table1 = newTable("query1.html", 2, "Línea", "Pasajeros");

    if(query1Arch == NULL || table1 == NULL){
        fprintf(stderr, "Error, could not open files");
        exit(OPENERR);
    }

    fputs("Línea;Pasajeros", query1Arch);

    toBeginLines(sub);

    char * line=NULL;
    while(hasNextLine(sub)){
        
        size_t totalLinePassen = nextLine(sub, line);

        fprintf(query1Arch, "%s;%ld\n", line, totalLinePassen);
        addHTMLRow(table1, line, totalLinePassen); //NO ME ACUERDO SI ERA CON STRINGS

        nextLine(sub, line);
    }

    fclose(query1Arch);
    closeHTMLTable(table1);
}

void query2(subADT sub){
    FILE * query2Arch = fopen("query2.csv", "wt");
    htmlTable table2 = newTable("query2.html", 4, "Línea", "Top1Pasajeros", "Top2Pasajeros", "Top3Pasajeros");

    if(query2Arch == NULL || table2 == NULL){
        fprintf(stderr, "Error, could not open files");
        exit(OPENERR);
    }

    fputs("Línea;Top1Pasajeros;Top2Pasajeros;Top3Pasajeros", query2Arch);

    toBeginTopbyLine(sub);
    
    while(hasNextTopbyLine(sub)){
        char * stations[3];
        char line[1];
        line[0] = nextTopbyLine(sub, stations);
        line[1] = 0; 

        fprintf(query2Arch, "%s;%s;%s;%s\n", line, stations[0], stations[1], stations[2]);
        addHTMLRow(table2, line, stations[0], stations[1], stations[2]); //NO ME ACUERDO SI ERA CON STRINGS

        nextLine(sub, line);
    }

    fclose(query2Arch);
    closeHTMLTable(table2);
}


void query3(subADT sub){
    FILE * query3Arch = fopen("query3.csv", "wt");
    htmlTable table3 = newTable("query3.html", 5, "Día", "TopMañana", "TopMediodía", "TopTarde", "TopNoche");

    if(query3Arch == NULL || table3 == NULL){
        fprintf(stderr, "Error, could not open files");
        exit(OPENERR);
    }

    fputs("Día;TopMañana;TopMediodía;TopTarde;TopNoche", query3Arch);

    toBeginTopPeriod(sub);
    
    char * days[CANTWEEKDAYS] = {"LUNES", "MARTES", "MIERCOLES", "JUEVES", "VIERNES", "SABADO", "DOMINGO"};
    char * stations[CANTPERIODS];
    char * lines[CANTPERIODS];
    char res[CANTPERIODS][MAX_CHARS];
    
    for(int i=0; i<CANTWEEKDAYS; i++){
        for(int j=0; j<CANTPERIODS; j++){
            stations[j] = getTopStationPeriod(sub, j, i, lines[j]);
            joinStationLine(res[j], stations[j], lines[j]);
        }
        
        fprintf(query3Arch, "%s;%s;%s;%s;%s\n", days[i], res[0], res[1], res[2], res[3]);
        addHTMLRow(table3, days[i], res[0], res[1], res[2], res[3]); 
    }
    

    fclose(query3Arch);
    closeHTMLTable(table3);
}

void query4(subADT sub){
    FILE * query4Arch = fopen("query4.csv", "wt");
    htmlTable table4 = newTable("query4.html", 4, "Estación", "TopPromedio", "Año", "Mes");

    if(query4Arch == NULL || table4 == NULL){
        fprintf(stderr, "Error, could not open files");
        exit(OPENERR);
    }

    fputs("Estación;TopPromedio;Año;Mes", query4Arch);

    toBeginAvgTop(sub);
    
    char * station=NULL;
    char * line=NULL;
    size_t * year=NULL; 
    char * month=NULL;
    char res[MAX_CHARS];

    while(hasNextAvgTop(sub)){
        float avg = NextAvgTop(sub, station, line, year, month);
        joinStationLine(res, station, line);
        fprintf(query4Arch, "%s;%f;%ld;%s\n", res, avg, (*year), month);
        addHTMLRow(table4, station, line, avg, year, month); 
    }
    

    fclose(query4Arch);
    closeHTMLTable(table4);
}


void joinStationLine(char * res, char * station, char * line){
    int dim = strlen(station);
    strcpy(res, station);
    res[dim++] = SPACE;
    res[dim++] = OPENBR;
    strcpy(res+dim, line);
    res[++dim] = OPENBR+1;
    res[++dim] = 0;
}