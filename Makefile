SOURCE_DIR=src
CFLAGS=-I./include/
CC=gcc

afdx: $(SOURCE_DIR)/afdx.o $(SOURCE_DIR)/network.o
	$(CC) $(CFLAGS) $(SOURCE_DIR)/afdx.o $(SOURCE_DIR)/network.o -o afdx

$(SOURCE_DIR)/afdx.o:
	$(CC) $(CFLAGS) -c $(SOURCE_DIR)/afdx.c -o $(SOURCE_DIR)/afdx.o

$(SOURCE_DIR)/network.o:
	$(CC) $(CFLAGS) -c $(SOURCE_DIR)/network.c -o $(SOURCE_DIR)/network.o


clean:
	rm $(SOURCE_DIR)/*.o afdx
