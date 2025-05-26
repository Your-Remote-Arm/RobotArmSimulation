PROGRAM = arm_sim
PLATFORM = arch

# ImGui parameters
IMGUI_SRC = imgui.cpp imgui_draw.cpp imgui_widgets.cpp imgui_tables.cpp imgui_demo.cpp
IMGUI_BACKENDS = backends/imgui_impl_glfw.cpp backends/imgui_impl_opengl3.cpp
IMGUI_OBJS = $(addprefix ./lib/.o/, $(IMGUI_SRC:.cpp=.o) $(IMGUI_BACKENDS:.cpp=.o) )
IMGUI_LIB = ./lib/libimgui_$(PLATFORM).a
IMGUI_INCLUDE = -I ./submodules/imgui

# Other submodule parameters
GLFW = ./submodules/NRA_visionGL/vendor/glfw
GLFW_LIB = ./submodules/glfw-build/src/libglfw3.a
GLFW_BUILD = 
NRA_LIB = ./submodules/NRA_visionGL/lib/libvision_lib_$(PLATFORM).a

LIB = $(NRA_LIB) $(IMGUI_LIB)
BIN_LOCATION = ./.bin/$(PROGRAM)_bin
BIN = $(BIN_LOCATION)/$(PROGRAM)
INCLUDE = -I ./include -I ./submodules -I ./submodules/NRA_visionGL/vendor/glfw/include -I ./submodules/NRA_visionGL/vendor/glad/include -I ./submodules/NRA_visionGL/build/include -I ./submodules/NRA_visionGL/include -I ./submodules/NRA_visionGL/vendor/glm
COMPILER = g++ -std=c++17
ARGS = 
SRC = src/arm_segment.cpp src/arm_segment_manager.cpp

ifneq ($(PLATFORM), win)
	LIB += -lglfw -lGL -lzmq
else
	LIB += ./submodules/glfw-build/src/libglfw3.a -lgdi32
	GLFW_BUILD = $(GLFW_LIB)
endif

include ./colors.mak

init:
	-mkdir lib
	-mkdir lib/.o
	-mkdir lib/.o/backends
	-git submodule init
	-git submodule update
	-cd submodules/NRA_visionGL && make init && make build

$(NRA_LIB):
	cd ./submodules/NRA_visionGL && make build
$(GLFW_LIB):
	-mkdir ./submodules/glfw-build
	cd $(GLFW) && cmake -S ./ -B ../../../glfw-build
	cmake -DGLFW_LIBRARY_TYPE=STATIC ./submodules/glfw-build
	cmake --build ./submodules/glfw-build


./lib/.o/imgui.o: ./submodules/imgui/imgui.cpp
	$(CXX) -c $< -o $@ 
./lib/.o/imgui%.o: ./submodules/imgui/imgui%.cpp
	$(CXX) -c $< -o $@
./lib/.o/backends/imgui%.o: ./submodules/imgui/backends/imgui%.cpp
	$(CXX) -c $< -o $@ $(IMGUI_INCLUDE) $(INCLUDE)
$(IMGUI_LIB): $(IMGUI_OBJS)
	ar rcs $(IMGUI_LIB) $^

./.bin:
	-@mkdir ./.bin
$(BIN_LOCATION): ./.bin
	-@mkdir $(BIN_LOCATION)

build: $(BIN)
$(BIN): $(BIN_LOCATION) ./main/$(PROGRAM).cpp $(SRC) $(NRA_LIB) $(IMGUI_LIB) $(GLFW_BUILD)
	-rm $(BIN)
	@echo -e "${GREEN}Building '${PROGRAM}'${CYAN}"
	$(COMPILER) $(INCLUDE) $(IMGUI_INCLUDE) -o $(BIN) ./main/$(PROGRAM).cpp $(SRC) $(LIB)
	@echo -e "${GREEN}Built '${PROGRAM}'${NOCOLOR}"

# Run the executable file
run: $(BIN)
	@echo -e "${GREEN}Running '${PROGRAM}'${NOCOLOR}"
	cd $(BIN_LOCATION) && ./$(PROGRAM) $(ARGS)
	@echo -e "${GREEN}Finished Running '${PROGRAM}'${NOCOLOR}"