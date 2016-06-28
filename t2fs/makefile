CC=gcc
LIB_DIR=./lib
BIN_DIR=./bin
SRC_DIR=./src
OBJ_DIR=./obj
CFLAGS=-g -Wall

all: t2fs

t2fs: main.o t2fs.o util.o
	$(CC) -o $(BIN_DIR)/t2fs $(OBJ_DIR)/main.o $(OBJ_DIR)/t2fs.o $(OBJ_DIR)/util.o $(LIB_DIR)/apidisk.o $(LIB_DIR)/bitmap2.o
	@echo "\ncompilou\n"
        
t2fs.o: $(SRC_DIR)/util.c 
	$(CC) -c $^ -o $(OBJ_DIR)/$@ -Wall

util.o: $(SRC_DIR)/util.c 
	$(CC) -c $^ -o $(OBJ_DIR)/$@ -Wall 

main.o: $(SRC_DIR)/main.c 
	$(CC) -c $^ -o $(OBJ_DIR)/$@ -Wall


clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/t2fs
