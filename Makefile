# thanks u/lovelacedeconstruct for posting most of what this is on reddit
CC=gcc
EXT=c

OPT=
DBG=
WARNINGS=-Wall -Wextra -Wsign-conversion -Wconversion
DEPFLAGS=-MP -MD
# DEF=-DTRACY_ENABLE

INCS=$(foreach DIR,$(INC_DIRS),-I$(DIR))
LIBS=$(foreach DIR,$(LIB_DIRS),-L$(DIR))

LIBS+=-lGL -lm -lpthread -ldl -lrt -lX11 -lraylib

CFLAGS=$(DBG) $(OPT) $(INCS) $(LIBS) $(WARNINGS) $(DEPFLAGS) $(DEF) -ffast-math -fopenmp

# maybe modify this to use shell and automatically find the libraries?
INC_DIRS=. ./external/include/ ./external/raylib-5.0_linux_i386/include/ ./include/
LIB_DIRS=. ./external/lib ./external/raylib-5.0_linux_i386/lib/
BUILD_DIR=build
CODE_DIRS=. src 
VPATH=$(CODE_DIRS)

SRC=$(foreach DIR,$(CODE_DIRS),$(wildcard $(DIR)/*.$(EXT)))
OBJ=$(addprefix $(BUILD_DIR)/,$(notdir $(SRC:.$(EXT)=.o)))
DEP=$(addprefix $(BUILD_DIR)/,$(notdir $(SRC:.$(EXT)=.d)))

PROJ=Main
EXEC=$(PROJ)

all: $(BUILD_DIR)/$(EXEC)
	@echo "========================================="   
	@echo "              BUILD SUCCESS              "
	@echo "========================================="

release: OPT += -O2 
release: all

debug: DBG += -g -gdwarf-2
debug: OPT += -O0
debug: all

$(BUILD_DIR)/%.o: %.$(EXT) | $(BUILD_DIR)
	$(CC) -c  $< -o $@ $(CFLAGS)
$(BUILD_DIR)/$(EXEC): $(OBJ)
	$(CC)  $^ -o $@ $(CFLAGS)

$(BUILD_DIR):
	mkdir $@
	# cp ./external/lib/*.dll ./build/
	# Only copy DLLs if they exist
	-[ -n "$(wildcard ./external/lib/*.dll)" ] && cp ./external/lib/*.dll ./build/
	$(info SRC_DIRS : $(CODE_DIRS))
	$(info INC_DIRS : $(INC_DIRS))
	$(info INCS     : $(INCS))
	$(info SRC_FILES: $(SRC))
	$(info OBJ_FILES: $(OBJ))   
	@echo "========================================="

clean:
	rm -fR $(BUILD_DIR)

profile:
	start Tracy;start ./$(BUILD_DIR)/$(EXEC);

-include $(DEP)

.PHONY: all clean profile
