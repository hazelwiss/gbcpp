BUILD_FOLDER:=build
EXECUTABLE:=gbcpp
CXX:=g++
CXXFLAGS:=-O0 -g -Wall -I"include" --std=gnu++20
CXXLIBS:=
SOURCE_FILES_SUFFIX:=-name "*.cpp"
HEADER_FILES:=$(shell find include -name "*.h")
SOURCE_FILES:=$(shell find src ${SOURCE_FILES_SUFFIX})
OBJECT_FILES:= $(addprefix $(BUILD_FOLDER)/,$(addsuffix .o,$(SOURCE_FILES)))

app: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECT_FILES) 
	@mkdir -p $(dir $@) 
	$(CXX) $(CXXFLAGS) $(CXXLIBS) $^ -o $@

$(OBJECT_FILES): $(SOURCE_FILES) $(HEADER_FILES)
	@mkdir -p $(dir $@) 
	$(CXX) $(CXXFLAGS) $(CXXLIBS) $(shell echo "$*" | cut -d "/" -f2-) -c -o $@

clean:
	rm -rf $(BUILD_FOLDER)/*

clean_all: clean
	rm $(EXECUTABLE)