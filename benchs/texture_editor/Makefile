

EXAMPLES_PATH = /home/Mathieu/imgui-master/examples/
IMGUI_PATH = /home/Mathieu/imgui-master/

EXE = textureGenerator
SOURCES = main.cpp
SOURCES += $(EXAMPLES_PATH)imgui_impl_glfw.cpp $(EXAMPLES_PATH)imgui_impl_opengl3.cpp
SOURCES += $(IMGUI_PATH)imgui.cpp $(IMGUI_PATH)imgui_demo.cpp $(IMGUI_PATH)imgui_draw.cpp $(IMGUI_PATH)imgui_widgets.cpp
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
UNAME_S := $(shell uname -s)

CXXFLAGS = -I$(EXAMPLES_PATH) -I$(IMGUI_PATH) -I/usr/include/ImpGears
CXXFLAGS += -g -Wall -Wformat
LIBS =

## Using OpenGL loader: glew
## (This assumes a system-wide installation)
CXXFLAGS += -lglew32 -DIMGUI_IMPL_OPENGL_LOADER_GLEW -std=c++17

ECHO_MESSAGE = "MinGW"
LIBS += -L /usr/lib -lglfw3 -lgdi32 -lopengl32 -limm32 -lstdc++fs -limpgears-core -limpgears-geometry -limpgears-graphics -limpgears-scenegraph -limpgears-renderer -limpgears-descriptors -limpgears-plugins

#CXXFLAGS += `pkg-config --cflags glfw3`
CFLAGS = $(CXXFLAGS)

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

%.o:%.cpp vegetal.h rock.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(EXAMPLES_PATH)%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_PATH)%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(EXAMPLES_PATH)libs/gl3w/GL/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o:$(EXAMPLES_PATH)libs/glad/src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(EXE) $(OBJS)
