# Decide about the main compiler
CC := g++

# Set the directories
DIR := src
TARGET := plot.exe

# Set flags
CFLAGS := -I./include `root-config --cflags`
LIBS := `root-config --libs`
MISCFLAGS := -fdiagnostics-color=always
DEBUGFLAGS := -O0 -g2

SRC := $(shell find $(DIR) -type f -name *.cc)
SRC += util/plot.cc
OBJ := $(SRC:.cc=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	@echo "   Linking..."
	@echo "   $(CC) $^ -o $(TARGET) $(LIBS)"; $(CC) $^ -o $(TARGET) $(LIBS)

%.o: %.cc
	@echo "   $(CC) $(CFLAGS) $(MISCFLAGS) -c -o $@ $<"; $(CC) $(CFLAGS) $(MISCFLAGS) -c -o $@ $<

clean:
	@echo "   Cleaning...";
	@echo "   find $(DIR) -type f -name "*.o" -exec rm -f {} \;"; find $(DIR) -type f -name "*.o" -exec rm -f {} \;
	@echo "   rm -f $(TARGET)"; rm -f $(TARGET)
