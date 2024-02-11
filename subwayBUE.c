#include <stdio.h>
#include <string.h>
#include "subwayADT.h"
#include "htmlTable.h"

#define MAX_CHARS 50 
#define DELIM ";"
#define CHANGE_LINE "\n"
#define DATE_DELIM "/"
#define HOUR_DELIM ":"
#define OPENBR '('
#define SPACE ' '
#define LINE_SPACE 2

/*-------------------------------------------------------------------------- PROTOTYPES --------------------------------------------------------------------------------------------*/

// The function reads the stations in order to extract the station id, it´s name and line.
// It returns the ADT with the information collected.
subADT readStations(FILE * stations, subADT sub);

// The function reads the data from the turnstiles.csv archive and exctracts from it the station id, the date (day, month, year), 
// the amount of passengers and the hour. 
void readTurnstiles(subADT sub, FILE * turnstiles);

// It copies the station´s name and returns the stations name.
char * copyName(char * name, char * aux);

//This function merges the stations name with it´s line. It recives a char pointer where the result is going to be,
// the station name and subway line.
void joinStationLine(char * res, char * station, char * line);

void query1(subADT sub);
void query2(subADT sub);
void query3(subADT sub);
void query4(subADT sub);

/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

int main(int numArg, char * argv[]){
    if(numArg < 3 || numArg > 5){
        fprintf(stderr, "Error, amount of arguments not valid\n");
        exit(ARGERR);
    }

    FILE * fileTurnstiles = fopen(argv[1], "rt");
    FILE * fileStations = fopen(argv[2], "rt");

    if(fileTurnstiles == NULL || fileStations == NULL){
        fprintf(stderr, "Error, could not open files\n");
        exit(OPENERR);
    }
    
    int year_start = 0, year_end = 0;

    if (numArg == 4) {
        year_start = atoi(argv[3]);
        if (year_start <= 0) { // If it only recives the start year it check if it is a valid date
            fprintf(stderr, "Error, imput date not valid\n");
            exit(ERRDATE);
        }
    }else if (numArg == 5) { 
        year_start = atoi(argv[3]);
        year_end = atoi(argv[4]);
        if (year_end <= 0 || year_start <= 0 || year_start > year_end) {// If it recives the start and end year, it checks for validity.
            fprintf(stderr, "Error, imput date not valid\n");
            exit(ERRDATE);
        }
    }

    subADT sub = newSub(year_start, year_end); //creates the ADT

    if(sub == NULL){
        fprintf(stderr, "Error, could not assign memory\n");
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

    freeSub(sub);
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
        size_t id = atoi(temp); // It changes the number from a string to a integer

        name = copyName(name, NULL);

        temp = strtok(NULL, DELIM);
        char line = temp[0];

        strtok(NULL, CHANGE_LINE);

        addStations(sub, line, name, id);
        if(errno != OK){
            fprintf(stderr, "Error in addStation\n");
            exit(errno);
        }
    }
    free(name);
    return sub;
}

void readTurnstiles(subADT sub, FILE * turnstiles){
    char text[MAX_CHARS];
    char day, month, end;
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

        strtok(NULL, DELIM); // Skip´s hour start

        temp = strtok(NULL, HOUR_DELIM);
        end = atoi(temp);
        strtok(NULL, DELIM); // Skip´s the minutes in the hour end

        temp = strtok(NULL, DELIM);
        id = atoi(temp);

        temp = strtok(NULL, CHANGE_LINE);
        numPassen = atoi(temp);

        addDataTrips(sub, day, month, year, id, numPassen, end);
        if(errno != OK){
            fprintf(stderr, "Error in addDataTrips\n");
            exit(errno);
        }
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
    htmlTable table1 = newTable("query1.html", 2, "Línea", "Pasajeros"); //Creates the HTML tables 

    if(query1Arch == NULL || table1 == NULL){
        fprintf(stderr, "Error, could not open files");
        exit(OPENERR);
    }

    fputs("Línea;Pasajeros\n", query1Arch);

    toBeginLines(sub);
    if(errno != OK){
        fprintf(stderr, "Error in toBeginLines\n");
        exit(errno);
    }

    char line[LINE_SPACE];
    char res[MAX_CHARS];
    while(hasNextLine(sub)){
        if(errno != OK){
            fprintf(stderr, "Error in hasNextLine\n");
            exit(errno);
        }
        size_t totalLinePassen = nextLine(sub, line);

        fprintf(query1Arch, "%s;%ld\n", line, totalLinePassen);
        sprintf(res, "%ld", totalLinePassen); // Changes the value of totalLinePassen from a size_t to a string.
        addHTMLRow(table1, &line, res); 
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

    fputs("Línea;Top1Pasajeros;Top2Pasajeros;Top3Pasajeros\n", query2Arch);

    toBeginTopbyLine(sub);
    if(errno != OK){
        fprintf(stderr, "Error in toBeginTopbyLine\n");
        exit(errno);
    }
    
    while(hasNextTopbyLine(sub)){
        if(errno != OK){
            fprintf(stderr, "Error in hasNextTopbyLine\n");
            exit(errno);
        }
        char * stations[TOP];
        char line[LINE_SPACE];

        line[0] = nextTopbyLine(sub, stations);
        line[1] = '\0'; // Null terminates the string.

        fprintf(query2Arch, "%s;%s;%s;%s\n", line, stations[0], stations[1], stations[2]);
        addHTMLRow(table2, line, stations[0], stations[1], stations[2]);
    }

    fclose(query2Arch);
    closeHTMLTable(table2);
}


void query3(subADT sub){
    FILE * query3Arch = fopen("query3.csv", "wt");
    htmlTable table3 = newTable("query3.html", 5, "Día", "TopMañana", "TopMediodía", "TopTarde", "TopNoche");

    if(query3Arch == NULL || table3 == NULL){
        fprintf(stderr, "Error, could not open files\n");
        exit(OPENERR);
    }

    fputs("Día;TopMañana;TopMediodía;TopTarde;TopNoche\n", query3Arch);

    toBeginTopPeriod(sub);
    if(errno != OK){
        fprintf(stderr, "Error in toBeginTopPeriod\n");
        exit(errno);
    }
    
    char * days[CANTWEEKDAYS] = {"LUNES", "MARTES", "MIERCOLES", "JUEVES", "VIERNES", "SABADO", "DOMINGO"};
    char * stations[CANTPERIODS];
    char lines[CANTPERIODS][LINE_SPACE];
    char res[CANTPERIODS][MAX_CHARS];
    
    for(int i=0; i<CANTWEEKDAYS; i++){
        for(int j=0; j<CANTPERIODS; j++){
            stations[j] = getTopStationPeriod(sub, j, i, lines[j]);
            if(errno != OK){
                fprintf(stderr, "Error in getTopStationPeriod\n");
                exit(errno);
            }
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

    fputs("Estación;TopPromedio;Año;Mes\n", query4Arch);

    toBeginAvgTop(sub);
    if(errno != OK){
        fprintf(stderr, "Error in toBeginAvgTop\n");
        exit(errno);
    }
    
    char station[MAX_CHARS];
    char line[LINE_SPACE];
    size_t year= 0; 
    char month= 0;
    char res[MAX_CHARS];
    char resAvg[MAX_CHARS];
    char resYear[MAX_CHARS];
    char resMonth[MAX_CHARS];

    while(hasNextAvgTop(sub)){
        if(errno != OK){
            fprintf(stderr, "Error in hasNextAvgTop\n");
            exit(errno);
        }

        float avg = NextAvgTop(sub, station, line, &year, &month);
        joinStationLine(res, station, line);

        fprintf(query4Arch, "%s;%.2f;%ld;%d\n", res, avg, year, month);

        sprintf(resAvg, "%f", avg);
        sprintf(resYear, "%ld", year);
        sprintf(resMonth, "%d", month);

        addHTMLRow(table4, station, line, resAvg, resYear, month); 
    }

    fclose(query4Arch);
    closeHTMLTable(table4);
}


void joinStationLine(char * res, char * station, char * line){
    int dim = strlen(station);
    strcpy(res, station);

    if(strcmp(station, NOTOPSTATION) != 0){ // If the station name is the same as NOTOPSTATION ("S/D") then it means that ther is no line data.
                                            // To not print rubbish we omit it
        res[dim++] = SPACE;
        res[dim++] = OPENBR;
        strcpy(res+dim, line);
        res[++dim] = OPENBR+1;
        res[++dim] = '\0';
    }
}
