SOURCE_DIR=src
INCLUDE_DIR=./include
CFLAGS=-I./include -W -Wall
CC=gcc
LDFLAGS=-lpthread
SERVER_OBJS= $(SOURCE_DIR)/afdx.o \
             $(SOURCE_DIR)/application_manager.o \
             $(SOURCE_DIR)/elaboration_thread.o \
             $(SOURCE_DIR)/network_thread.o \
             $(SOURCE_DIR)/queue.o \
             $(SOURCE_DIR)/registration.o \
             $(SOURCE_DIR)/scheduler.o \
             $(SOURCE_DIR)/simplog.o


all: afdx udp app1 app2 app3 app4


afdx: $(SERVER_OBJS)
	$(CC) $(CFLAGS) $(SERVER_OBJS) $(LDFLAGS) -o afdx


$(SOURCE_DIR)/simplog.o: $(SOURCE_DIR)/simplog.c
	$(CC) $(CFLAGS) -c $(SOURCE_DIR)/simplog.c -o $(SOURCE_DIR)/simplog.o


clean:
	rm -f $(SERVER_OBJS) afdx app1 app2 app3 app4 UDP_generator

$(SOURCE_DIR)/afdx.o : $(INCLUDE_DIR)/simplog.h \
                       $(INCLUDE_DIR)/globals.h \
                       $(INCLUDE_DIR)/internal_data.h


udp: $(SOURCE_DIR)/UDP_generator.c
	$(CC) $(CFLAGS) $(SOURCE_DIR)/UDP_generator.c -o UDP_generator


app1: $(SOURCE_DIR)/app1.c
	$(CC) $(CFLAGS) $(SOURCE_DIR)/app1.c -o app1


app2: $(SOURCE_DIR)/app2.c
	$(CC) $(CFLAGS) $(SOURCE_DIR)/app2.c -o app2


app3: $(SOURCE_DIR)/app3.c
	$(CC) $(CFLAGS) $(SOURCE_DIR)/app3.c -o app3


app4: $(SOURCE_DIR)/app4.c
	$(CC) $(CFLAGS) $(SOURCE_DIR)/app4.c -o app4

