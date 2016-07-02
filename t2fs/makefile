CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src

OBJS= $(LIB_DIR)/util.o $(LIB_DIR)/bitmap.o $(LIB_DIR)/t2fs.o
DEPS= $(LIB_DIR)/apidisk.o

$(LIB_DIR)/%.o:$(SRC_DIR)/%.c
	$(CC) -c $< -o $@ -Wall

all: $(OBJS)
	ar crs $(LIB_DIR)/libt2fs.a $(OBJS) $(DEPS)

clean:
	rm -rf $(OBJS) $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~

