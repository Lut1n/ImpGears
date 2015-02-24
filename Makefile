DEBUG=no
CXX=g++

ifeq ($(DEBUG),yes)
	CXXFLAGS=-Wall -Werror -fpic -std=c++11 -fvisibility="hidden" -g
	MACROS=-DIMPGEARS_BUILD_DLL -DIMPGEARS_DEBUG
else
	CXXFLAGS=-Wall -Werror -fpic -std=c++11 -fvisibility="hidden"
	MACROS=-DIMPGEARS_BUILD_DLL
endif

LDFLAGS=-pthread -lpng -lGL -lGLU -lGLEW -lsfml-graphics -lsfml-window -lsfml-system

SRCDIR=src
INCDIR=includes
OBJDIR=obj
OUTPUT=libimpgears.so
SRC=$(shell find $(SRCDIR) -type f -name '*.cpp')
OBJ=$(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
MKDIR=mkdir -p


all: $(OUTPUT)

$(OUTPUT): $(OBJ)
	$(CXX) -shared $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(OBJDIR)
	$(MKDIR) "$(@D)"
	$(CXX) $(CXXFLAGS) $(MACROS) -I $(INCDIR) -c $< -o $@

$(OBJDIR):
	$(MKDIR) $(OBJDIR)


.PHONY : clean mrproper

clean:
	rm -rf "$(OBJDIR)"

mrproper: clean
	rm -rf "$(OUTPUT)"
