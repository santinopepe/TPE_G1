#include <stdio.h>
#include <string.h>

#define MAX_CHARS 50
#define DELIM ";"
#define CHANGE_LINE "\n"
#define DATE_DELIM "/"

subADT readStations(FILE * stations);
void readTurnstiles(subADT sub, FILE * turnstiles);
char * copyName(char * name, char * aux);

void query1(subADT sub);
void query2(subADT sub);
void query3(subADT sub);


int main(int numArg, char * argv[]){
    if(numArg <= 3 && numArg >= 5){ //Manu fijate que nos pueden 0 anios 1 anio 2 anios.
        //falta manejo de errores
    }

    FILE * fileTurnstiles = fopen(argv[1], "rt");
    FILE * fileStations = fopen(argv[2], "rt");

    if(fileTurnstiles == NULL || fileStations == NULL){
         //falta manejo de errores
    }
    

    subADT sub = readStations(fileStations);
    readTurnstiles(sub, fileTurnstiles);

    fclose(fileTurnstiles);
    fclose(fileStations);

    query1(sub);
    query2(sub);
    query3(sub);

    free(sub);
}

subADT readStations(FILE * stations){
    subADT sub = newSub();

    if(sub == NULL){
        //falta manejo de errores
    }

    char text[MAX_CHARS];

    fscanf(stations, "%s\n", text);

    char * temp;
    char * name = NULL;
    while(fgets(text, MAX_CHARS, stations) != NULL){
        temp = strtok(text, DELIM);
        if(temp == NULL){
            //falta manejo de errores
        }
        size_t id = atoi(temp);

        name = copyName(name, NULL);

        char line = strtok(NULL, DELIM);

        strtok(NULL, CHANGE_LINE);

        createStations(sub, line, name, id);
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
            //falta manejo de errores
        }
        day = atoi(temp);

        temp = strtok(NULL, DATE_DELIM);
        month = atoi(temp);

        temp = strtok(NULL, DELIM);
        year = atoi(temp);

        temp = strtok(NULL, DELIM);
        strcpy(start, temp);

        temp = strtok(NULL, DELIM);
        strcpy(end, temp);

        temp = strtok(NULL, DELIM);
        id = atoi(temp);

        temp = strtok(NULL, DELIM);
        numPassen = atoi(temp);

        addData(sub, day, month, year, id, numPassen, start, end);
    }
}

char * copyName(char * name, char * aux){
    char * tok = strtok(aux, DELIM);
    if(tok == NULL){
        //falta manejo de errores
    }

    name = realloc(name, strlen(tok)+1);
    if(name == NULL){
        //falta manejo de errores
    }
    strcpy(name, tok);

    return name;
}
