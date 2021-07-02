BUILD_FOLDER:=build
EXECUTABLE:=gbcpp
CXX:=g++
CXXFLAGS:=-O0 -g -Wall -I"include" -I"imgui" --std=gnu++20
SOURCE_FILES_SUFFIX:=-name "*.cpp"
HEADER_FILES:=$(shell find include -name "*.h")
SOURCE_FILES:=$(shell find src ${SOURCE_FILES_SUFFIX})
OBJECT_FILES:= $(addprefix $(BUILD_FOLDER)/,$(addsuffix .o,$(SOURCE_FILES)))

IMGUI_SOURCE_FILE:=$(shell find imgui -maxdepth 1 -name "*.cpp")
IMGUI_SOURCE_FILE+=imgui/backends/imgui_impl_glfw.cpp
IMGUI_SOURCE_FILE+=imgui/backends/imgui_impl_opengl3.cpp
IMGUI_CXXFLAGS:= -O3 -I"imgui"
IMGUI_OBJECT_FILES:= $(addprefix $(BUILD_FOLDER)/,$(addsuffix .o,$(IMGUI_SOURCE_FILE)))

CXXLIBS:=-lglfw -lGL -lGLEW

all: imgui app

app: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECT_FILES) $(IMGUI_OBJECT_FILES)
	@mkdir -p $(dir $@) 
	$(CXX) $(CXXFLAGS) $(CXXLIBS) $^ -o $@

$(OBJECT_FILES): $(SOURCE_FILES) $(HEADER_FILES)
	@mkdir -p $(dir $@) 
	$(CXX) $(CXXFLAGS) $(shell echo "$*" | cut -d "/" -f2-) -c -o $@

imgui: $(IMGUI_OBJECT_FILES)

$(IMGUI_OBJECT_FILES):
	@mkdir -p $(dir $@)
	$(CXX) $(IMGUI_CXXFLAGS) $(shell echo "$*" | cut -d "/" -f2-) -c -o $@

clean:
	rm -rf $(BUILD_FOLDER)/*

clean_all: clean
	rm $(EXECUTABLE)