# TPE_G1_PI

## Compilation:

In order to compile the program into an executable, run the file make in the terminal of the directory in which  makefile is located:

make or make all

This action generates the executable SubwayBUE

## Running:
To run the program first enter the turnstiles file and then the file with the stations information. 
Being turnstiles.csv the turnstiles file and stations.csv a file with the station information, for each case:

#### No year limit for Query 4:

./subwayBUE turnstiles.csv stations.csv

#### Starting year for Query 4:

./subwayBUE turnstiles.csv stations.csv "Starting_Year"

#### Starting and Ending years for Query 4:

./subwayBUE turnstiles.csv stations.csv "Starting_Year" "Ending_Year"

Please make sure the files turnstiles.csv and stations.csv are all in the same directory as the executable or provide the path to said files.

## Clean:
To remove the executables that were generated previously, run the clean command prefixing make, for each case:
#### Clean Queries.csv:
In order to only remove the files of the queries of csv extension, run the command:
make cleanQueries

#### Clean Queries.html:
In order to only remove the files of the queries of html extension, run the command:
make cleanHTML

#### Clean both extensions:
In order to remove the files of the queries of both extensions, run the command:
make clean

## Return values:
If the return value is 0 then it means everything went as planed. If not, then the exit values go as listed below:

MEMERR: Memory error, exit value 1. 

TOKENERR: Token error, exit value 2.

ARGERR: Error in the quantity of archives, exit value 3.

OPENERR: Error opening archives, exit value 4.

NEXTERR: Error on the iterator, exit value 5.

ERRID: Error on a invalid id, exit value 6.

ERRDATE: Error on a invalid date, exit value 7.

PARAMERR: Error in the parameter provided by the frontend, exit value 8.
