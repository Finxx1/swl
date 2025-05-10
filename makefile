# This makefile is for building the Windows version.
# Other platforms use GNUmakefile instead.

TARGET=swl
OBJECTS=\
	src\registry.o \
	src\win32_window.o
EXAMPLES=\
	examples/simple_window.exe

CFLAGS=/nologo /DINTERNAL /Iinclude
LDFLAGS=/nologo /NODEFAULTLIB /NOENTRY user32.lib kernel32.lib gdi32.lib
ARCH=X64

# You probably shouldn't edit below this line.

all: $(TARGET).lib $(TARGET).dll
clean:
	-del /Q $(TARGET).lib $(TARGET).dll $(OBJECTS) >NUL 2>NUL
examples: $(TARGET).dll $(EXAMPLES)
test: examples
	examples\simple_window

$(TARGET).lib: $(OBJECTS)
	lib /nologo /OUT:$(TARGET).lib $(OBJECTS)

$(TARGET).dll: $(OBJECTS) src\$(TARGET).def
	link /DEF:src\$(TARGET).def $(LDFLAGS) /DLL /OUT:$(TARGET).dll $(OBJECTS)
	lib /nologo /DEF:src\$(TARGET).def /OUT:$(TARGET)dll.lib /MACHINE:$(ARCH)

.c.o:
	$(CC) $(CFLAGS) /c /Fo: $@ $<

# Only used for examples.
.c.exe:
	$(CC) $(CFLAGS) /Fe: $@ /Fo: examples/ $< $(TARGET)dll.lib
