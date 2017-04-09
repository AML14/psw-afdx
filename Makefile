SOURCE_DIR=src
CFLAGS=-I./include/
CC=gcc

afdx: $(SOURCE_DIR)/afdx.o $(SOURCE_DIR)/network.o $(SOURCE_DIR)/simplog.o
	$(CC) $(CFLAGS) $(SOURCE_DIR)/afdx.o $(SOURCE_DIR)/network.o $(SOURCE_DIR)/simplog.o -o afdx

$(SOURCE_DIR)/afdx.o: $(SOURCE_DIR)/afdx.c
	$(CC) $(CFLAGS) -c $(SOURCE_DIR)/afdx.c -o $(SOURCE_DIR)/afdx.o

$(SOURCE_DIR)/network.o: $(SOURCE_DIR)/network.c
	$(CC) $(CFLAGS) -c $(SOURCE_DIR)/network.c -o $(SOURCE_DIR)/network.o

$(SOURCE_DIR)/simplog.o: $(SOURCE_DIR)/simplog.c
	$(CC) $(CFLAGS) -c $(SOURCE_DIR)/simplog.c -o $(SOURCE_DIR)/simplog.o


clean:
	rm $(SOURCE_DIR)/*.o afdx
