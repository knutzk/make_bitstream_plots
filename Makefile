# Decide about the main compiler
CC := g++

# Set the directories
DIR := core
TARGET := plot

# Set flags
CFLAGS := -I./ `root-config --cflags`
LIBS := `root-config --libs`
MISCFLAGS := -fdiagnostics-color=always
DEBUGFLAGS := -O0 -g2

SRC := $(shell find $(COREDIR) -type f -name *.cc)
OBJ := $(CORESRC:.cc=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	@echo "   Linking..."
	@mkdir -p bin
	@echo "   $(CC) $^ -o $(TARGET) $(LIBS)"; $(CC) $^ -o $(TARGET) $(LIBS)

$(DIR)/%.o: $(DIR)/%.cc
	@echo "   $(CC) $(CFLAGS) -c -o $@ $<"; $(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "   Cleaning...";
	@echo "   find $(DIR) -type f -name "*.o" -exec rm {} \;"; find $(DIR) -type f -name "*.o" -exec rm {} \;
	@echo "   rm $(TARGET)"; rm $(TARGET)
