DEBUG=yes
CXX=g++

ifeq ($(DEBUG),yes)
	CXXFLAGS=-Wall -Werror -fpic -std=c++11 -fvisibility="hidden" -g
	MACROS=-DIMPGEARS_BUILD_DLL -DIMPGEARS_DEBUG
else
	CXXFLAGS=-Wall -Werror -fpic -std=c++11 -fvisibility="hidden"
	MACROS=-DIMPGEARS_BUILD_DLL
endif

### mingw64
LDFLAGS=-lsfml-graphics -lsfml-window -lsfml-system -pthread -lpng  -lglew32 -lopengl32

### linux
#LDFLAGS=-lsfml-graphics -lsfml-window -lsfml-system -pthread -lpng  -pthread -lGLEW -lGL

SRCDIR=src/ImpGears
INCDIR=include/ImpGears
OBJDIR=obj
OUTPUT=libimpgears.so
SRC=$(shell find $(SRCDIR) -type f -name '*.cpp')
OBJ=$(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
MKDIR=mkdir -p


all: $(OUTPUT)

$(OUTPUT): $(OBJ)
	$(CXX) -g -o $@ -shared $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(OBJDIR)
	$(MKDIR) "$(@D)"
	$(CXX) $(CXXFLAGS) $(MACROS) -I $(INCDIR) -c $< -o $@

$(OBJDIR):
	$(MKDIR) $(OBJDIR)


.PHONY : clean mrproper

clean:
	rm -rf "$(OBJDIR)"

#mrproper: clean
#rm -rf "$(OUTPUT)"