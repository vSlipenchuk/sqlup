all: sqlup

clean:
	rm *.o sqlup

sqlup: main.c common.c db_sqlite.c
	$(CC) main.c common.c db_sqlite.c -ldl -lpthread -o sqlup