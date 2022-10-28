ifeq ($(COMP), clang)
CXX = clang++
else
CXX = g++
endif

ifeq ($(PROF), true)
CXXPROF += -pg -no-pie
else
CXXPROF += -s
endif

ifeq ($(BUILD), debug)
CXXFLAGS += -fdiagnostics-color=always -DUNICODE -std=c++20 -Wall -Og -g -march=native
else
CXXFLAGS += -DUNICODE -DNDEBUG -std=c++20 -O3 -flto $(CXXPROF) -march=native
endif

ifeq ($(USE_PEXT), true)
CXXFLAGS += -DUSE_PEXT
endif

STATIC_LIB = -luser32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -ldwmapi -lstdc++fs -lsetupapi -lhid -static

.PHONY: all cli tuner clean makedir

all: cli tuner

cli: makedir
	@$(CXX) $(CXXFLAGS) "core\*.cpp" "ai\*.cpp" "cli\*.cpp" $(STATIC_LIB) -o "bin\cli\cli.exe"

tuner: makedir
	@$(CXX) $(CXXFLAGS) -DMAKE_TUNER "core\*.cpp" "ai\*.cpp" "tuner\*.cpp" $(STATIC_LIB) -o "bin\tuner\tuner.exe"

ppc: makedir
	@$(CXX) $(CXXFLAGS) "core\*.cpp" "ai\*.cpp" "ppc\*.cpp" $(STATIC_LIB) -o "bin\ppc\ppc.exe"

clean: makedir
	@echo Cleaning the bin directory
	@rmdir /s /q bin
	@make makedir

makedir:
	@IF NOT exist bin ( mkdir bin )
	@IF NOT exist bin\cli ( mkdir bin\cli )
	@IF NOT exist bin\ppc ( mkdir bin\ppc )
	@IF NOT exist bin\tuner ( mkdir bin\tuner )
	@IF NOT exist bin\tuner\data ( mkdir bin\tuner\data )

.DEFAULT_GOAL := cli