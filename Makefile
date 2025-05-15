PROGRAM = main
LIB = ./submodules/NRA_visionGL/lib/libvision_lib.a
BIN = ./.bin/$(PROGRAM)
INCLUDE = -I ./include -I ./submodules -I ./submodules/NRA_visionGL/vendor/glfw/include -I ./submodules/NRA_visionGL/vendor/glad/include -I ./submodules/NRA_visionGL/build/include -I ./submodules/NRA_visionGL/include
COMPILER = g++ -std=c++17
ARGS = 

# ImGui parameters
IMGUI_SRC = imgui.cpp imgui_draw.cpp imgui_widgets.cpp imgui_tables.cpp imgui_demo.cpp
IMGUI_BACKENDS = backends/imgui_impl_glfw.cpp backends/imgui_impl_opengl3.cpp
IMGUI_OBJS = $(addprefix ./lib/.o/, $(IMGUI_SRC:.cpp=.o) $(IMGUI_BACKENDS:.cpp=.o) )
IMGUI_LIB = ./lib/libimgui.a
IMGUI_INCLUDE = -I ./submodules/imgui

include ./colors.mak

$(LIB):
	cd ./submodules/NRA_visionGL && make build


./lib/.o/imgui.o: ./submodules/imgui/imgui.cpp
	$(CXX) -c $< -o $@ 
./lib/.o/imgui%.o: ./submodules/imgui/imgui%.cpp
	$(CXX) -c $< -o $@
./lib/.o/backends/imgui%.o: ./submodules/imgui/backends/imgui%.cpp
	$(CXX) -c $< -o $@ $(IMGUI_INCLUDE)
$(IMGUI_LIB): $(IMGUI_OBJS)
	ar rcs $(IMGUI_LIB) $^

./.bin:
	mkdir ./.bin

build: $(BIN)
$(BIN): ./.bin ./src/$(PROGRAM).cpp $(LIB) $(IMGUI_LIB)
	-rm $(BIN)
	@echo -e "${GREEN}Building '${PROGRAM}'${CYAN}"
	$(COMPILER) $(INCLUDE) $(IMGUI_INCLUDE) -o $(BIN) ./src/$(PROGRAM).cpp $(LIB) $(IMGUI_LIB) -lglfw -lGL
	@echo -e "${GREEN}Built '${PROGRAM}'${NOCOLOR}"

# Run the executable file
run: $(BIN)
	@echo -e "${GREEN}Running '${PROGRAM}'${NOCOLOR}"
	$(BIN) $(ARGS)
	@echo -e "${GREEN}Finished Running '${PROGRAM}'${NOCOLOR}"