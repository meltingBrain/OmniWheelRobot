#DIR_FONTS = ./Fonts
DIR_OBJ = ./obj
DIR_BIN = ./bin

OBJ_C = $(wildcard ${DIR_OBJ}/*.cpp)
OBJ_O = $(patsubst %.cpp,${DIR_BIN}/%.o,$(notdir ${OBJ_C}))

TARGET = libads1256
#BIN_TARGET = ${DIR_BIN}/${TARGET}

CC = g++

DEBUG = -g -O3 -Wall
CFLAGS += $(DEBUG) 

AR     = ar
ARFLAGS = crsv 

LIB = -lwiringPi -lpthread -lm

${TARGET}.a:${OBJ_O} ${OBJ_C}
	#$(CC) $(CFLAGS) $(OBJ_O) -o $@  $(LIB)
	$(AR) $(ARFLAGS) $@ $(OBJ_O)
	$(AR) t $@ 
	@echo "ADS1256 Archive Generated"

${DIR_BIN}/%.o : $(DIR_OBJ)/%.cpp
	$(CC) $(CFLAGS) -c  $< -o $@  $(LIB)
	
clean :
	rm $(DIR_BIN)/*.* 
	rm $(TARGET).a 
