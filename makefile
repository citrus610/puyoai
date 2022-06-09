CXX = g++

ifeq ($(BUILD), debug)
CXXFLAGS += -fdiagnostics-color=always -DUNICODE -std=c++20 -Wall -Og -g
else
CXXFLAGS += -DUNICODE -DNDEBUG -std=c++20 -O3 -flto -s -march=native
endif

ifeq ($(USE_PEXT), true)
CXXFLAGS += -DUSE_PEXT
endif

STATIC_LIB = -luser32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -ldwmapi -lstdc++fs -lsetupapi -lhid -static

.PHONY: all client tuner clean makedir

all: client tuner

client: makedir
	@echo Building Lemon Tea client...
	@$(CXX) $(CXXFLAGS) "bot\*.cpp" "client\*.cpp" $(STATIC_LIB) -o "bin\client\client.exe"
	@echo Finished building Lemon Tea client!

tuner: makedir
	@echo Building tuner...
	@$(CXX) $(CXXFLAGS) "bot\*.cpp" "tuner\*.cpp" $(STATIC_LIB) -o "bin\tuner\tuner.exe"
	@echo Finished building tuner!

clean: makedir
	@echo Cleaning the bin directory
	@rmdir /s /q bin
	@make makedir

makedir:
	@IF NOT exist bin ( mkdir bin )
	@IF NOT exist bin\client ( mkdir bin\client )
	@IF NOT exist bin\tuner ( mkdir bin\tuner )
	@IF NOT exist bin\tuner\data ( mkdir bin\tuner\data )

.DEFAULT_GOAL := client