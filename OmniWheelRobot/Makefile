DIR_OBJ = ./obj
DIR_BIN = ./bin
DIR_ARC = ./archive
DIR_PLT = ./data

OBJ_C = $(wildcard ${DIR_OBJ}/*.cpp)
OBJ_O = $(patsubst %.cpp,${DIR_BIN}/%.o,$(notdir ${OBJ_C}))

TARGET = OmniRobotGoGo

CC = g++

DEBUG = -g -O0 -Wall
CFLAGS += $(DEBUG) 

ARC = ${DIR_ARC}/libads1256.a
LIB = -lwiringPi -lpthread -lpigpio -lpigpiod_if2 -lrt -lm 

${TARGET}:${OBJ_O}
	$(CC) $(CFLAGS) $(OBJ_O) -o $@  $(LIB) ${ARC}

${DIR_BIN}/%.o : $(DIR_OBJ)/%.cpp $(DIR_OBJ)/%.hpp
	$(CC) $(CFLAGS) -c  $< -o $@  $(LIB) ${ARC}
	
clean :
	rm $(DIR_BIN)/*.* 
	rm $(TARGET) 
