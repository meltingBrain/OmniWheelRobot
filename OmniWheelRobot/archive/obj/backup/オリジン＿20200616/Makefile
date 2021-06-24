#DIR_FONTS = ./Fonts
DIR_OBJ = ./obj
DIR_BIN = ./bin

OBJ_C = $(wildcard ${DIR_OBJ}/*.cpp)
OBJ_O = $(patsubst %.cpp,${DIR_BIN}/%.o,$(notdir ${OBJ_C}))

TARGET = ads1256_test
#BIN_TARGET = ${DIR_BIN}/${TARGET}

CC = g++

DEBUG = -g -O0 -Wall
CFLAGS += $(DEBUG) 

LIB = -lwiringPi -lpthread -lm

${TARGET}:${OBJ_O}
	$(CC) $(CFLAGS) $(OBJ_O) -o $@  $(LIB)

${DIR_BIN}/%.o : $(DIR_OBJ)/%.cpp
	$(CC) $(CFLAGS) -c  $< -o $@  $(LIB)


	
clean :
	rm $(DIR_BIN)/*.* 
	rm $(TARGET) 
