CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src
TEST_DIR=./teste

OBJS= $(LIB_DIR)/openlib.o $(LIB_DIR)/fatlib.o $(LIB_DIR)/filelib.o $(LIB_DIR)/t2fs.o
DEPS= $(LIB_DIR)/apidisk.o

$(LIB_DIR)/%.o:$(SRC_DIR)/%.c
	@echo "Compilando $<"
	$(CC) -c $< -o $@ -Wall

all: $(OBJS)
	@echo "Linkando"
	ar crs $(LIB_DIR)/libt2fs.a $(OBJS) $(DEPS)

reseta_disco:
	cp disco_32_4_128.dsk ./bin/t2fs_disk.dat

testes: $(BIN_DIR)/delete $(BIN_DIR)/identify_get_ $(BIN_DIR)/mkdir_chdir_create_write_open_read_close $(BIN_DIR)/readdir $(BIN_DIR)/teste_80_arquivos $(BIN_DIR)/readdir $(BIN_DIR)/teste_cria_80_dir

rmtestes:
	rm -rf $(BIN_DIR)/delete $(BIN_DIR)/identify_get_ $(BIN_DIR)/mkdir_chdir_create_write_open_read_close $(BIN_DIR)/readdir $(BIN_DIR)/teste_80_arquivos $(BIN_DIR)/readdir $(BIN_DIR)/teste_cria_80_dir

$(BIN_DIR)/delete: 
	$(CC) -o $(BIN_DIR)/delete $(TEST_DIR)/delete.c -L$(LIB_DIR) -lt2fs -Wall
$(BIN_DIR)/identify_get_: 
	$(CC) -o $(BIN_DIR)/identify_get_ $(TEST_DIR)/identify_get_.c -L$(LIB_DIR) -lt2fs -Wall
$(BIN_DIR)/mkdir_chdir_create_write_open_read_close: 
	$(CC) -o $(BIN_DIR)/mkdir_chdir_create_write_open_read_close $(TEST_DIR)/mkdir_chdir_create_write_open_read_close.c -L$(LIB_DIR) -lt2fs -Wall
$(BIN_DIR)/readdir: 
	$(CC) -o $(BIN_DIR)/readdir $(TEST_DIR)/readdir.c -L$(LIB_DIR) -lt2fs -Wall
$(BIN_DIR)/teste_80_arquivos: 
	$(CC) -o $(BIN_DIR)/teste_80_arquivos $(TEST_DIR)/teste_80_arquivos.c -L$(LIB_DIR) -lt2fs -Wall
$(BIN_DIR)/readdir: 
	$(CC) -o $(BIN_DIR)/readdir $(TEST_DIR)/readdir.c -L$(LIB_DIR) -lt2fs -Wall
$(BIN_DIR)/teste_cria_80_dir: 
	$(CC) -o $(BIN_DIR)/teste_cria_80_dir $(TEST_DIR)/teste_cria_80_dir.c -L$(LIB_DIR) -lt2fs -Wall

clean:
	rm -rf $(OBJS) $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~


