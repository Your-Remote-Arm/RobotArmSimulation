PROGRAM = main
LIB = ./submodules/NRA_visionGL/lib/libvision_lib.a
BIN = ./.bin/$(PROGRAM)
INCLUDE = -I ./include -I ./submodules -I ./submodules/NRA_visionGL/vendor/glfw/include -I ./submodules/NRA_visionGL/vendor/glad/include -I ./submodules/NRA_visionGL/build/include -I ./submodules/NRA_visionGL/include
COMPILER = g++ -std=c++17
ARGS = 

include ./colors.mak

$(LIB):
	cd ./submodules/NRA_visionGL && make build

./.bin:
	mkdir ./.bin

build: $(BIN)
$(BIN): ./.bin ./src/$(PROGRAM).cpp $(LIB)
	-rm $(BIN)
	@echo -e "${GREEN}Building '${PROGRAM}'${CYAN}"
	$(COMPILER) $(INCLUDE) -o $(BIN) ./src/$(PROGRAM).cpp $(LIB) -lglfw -lGL
	@echo -e "${GREEN}Built '${PROGRAM}'${NOCOLOR}"

# Run the executable file
run: $(BIN)
	@echo -e "${GREEN}Running '${PROGRAM}'${NOCOLOR}"
	$(BIN) $(ARGS)
	@echo -e "${GREEN}Finished Running '${PROGRAM}'${NOCOLOR}"