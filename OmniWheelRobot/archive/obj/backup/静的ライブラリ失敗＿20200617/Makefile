#DIR_FONTS = ./Fonts
LIB_FILE = lib_ads1256
DIR_SRC  = ./src
DIR_BIN  = ./bin 

OBJ_C = $(wildcard ${DIR_SRC}/*.cpp)
OBJ_O = $(patsubst %.cpp,${DIR_BIN}/%.o,$(notdir ${OBJ_C}))

TARGET = lib_ads1256

CXX    = g++
DEBUG  = -g -O0 -Wall
CFLAGS += $(DEBUG)

AR     = ar
ARFLAGS = crsv 

LIB = -lwiringPi -lpthread -lm

${LIB_FILE}.a: $(DIR_BIN)/%.o $(DIR_SRC)/%.cpp
	$(AR) $(ARFLAGS) $@ $(OBJ_O)
	$(AR) t $@ 
	@echo "Library SUCCESS"

${DIR_BIN}/%.o : $(DIR_SRC)/%.cpp
	$(CXX) $(CFLAGS) -c  $< -o $@  $(LIB)
	@echo "Object File Success"

clean :
	rm $(LIB_FILE).a
	rm $(DIR_BIN)/*.* 
	