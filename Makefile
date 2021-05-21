#make file - this is a comment section
#new comment
#kgnfhgk
CC=gcc  #compiler
TARGET=Zigbee_Bridge_App #target file name

all:
	$(CC) main.c cJSON.c -lmosquitto -lm -o $(TARGET)

clean:
	rm $(TARGET)
