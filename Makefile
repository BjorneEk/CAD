TARGET=cad

# Files
SOURCE_DIR:=src
SOURCE_FILES:=$(wildcard $(SOURCE_DIR)/*.c $(SOURCE_DIR)/*/*.c)
HEADER_FILES:=$(wildcard $(SOURCE_DIR)/*.h $(SOURCE_DIR)/*/*.h)
ASSEMB_FILES:=$(wildcard $(SOURCE_DIR)/*.S $(SOURCE_DIR)/*/*.S)

OBJ:=${SOURCE_FILES:.c=.o}
OBJ+=${ASSEMB_FILES:.S=.o}

# Compiler
CC:=gcc
ARCH:=arm64
FONT_INCLUDE:=-I/opt/homebrew/opt/freetype/include/freetype2 -I/opt/homebrew/opt/libpng/include/libpng16
GL_INCLUDE:=-framework Cocoa -framework OpenGL -framework IOKit
CFLAGS:=-arch $(ARCH) -I$(SOURCE_DIR) $(GL_INCLUDE) $(FONT_INCLUDE) -F /Library/Frameworks -g
LIBS:= -lglfw3 -framework UL -L/opt/homebrew/opt/freetype/lib -lfreetype

# Assembler
ASM:=gcc
ASMFLAGS:=-arch arm64

# Preprocessor
PP:=gcc
PPFLAGS:=-E

# Rules

%.o: %.S Makefile
	$(PP) $< -o tmp.tmp $(PPFLAGS)
	sed /^#/d tmp.tmp > preprocessed.tmp.s
	$(RM) tmp.tmp
	$(ASM) -c -o $@ preprocessed.tmp.s $(ASMFLAGS)
	$(RM) preprocessed.tmp.s

%.o: %.c $(HEADER_FILES) Makefile
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

run: $(TARGET)
	./$(TARGET)

install: $(TARGET)
	echo "export PATH=\"\$$PATH:$(PWD)\"" >> ~/.zshrc

clean:
	$(RM)	$(OBJ)
	$(RM)	$(TARGET)