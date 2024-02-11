COMPILER 	= gcc
FLAGS 		= -pedantic -std=c99 -Wall -fsanitize=address -g
OBJS 		= subwayBUE.c subwayADT.c htmlTable.c
QUERIES 	= query1.csv query2.csv query3.csv query4.csv
HTML        = query1.html query2.html query3.html query4.html
OUT         = subwayBUE

all: $(OBJS)
	$(COMPILER) $(OBJS) -o $(OUT) $(FLAGS)

clean: cleanQueries cleanHTML

cleanQueries:
	rm -rf $(QUERIES)

cleanHTML:
	rm -rf $(HTML)
