# *****************************************************
# Variables

CXX = g++
DRIVER_DIR = ../../Driver/Coachbot/
#BENCHMARK_DIR = ../../Benchmark/simple_test/
BENCHMARK_DIR = ./
# For mac
CXXFLAGS = -std=c++11 -Wall -g -I $(DRIVER_DIR) -framework Python
# For linux
#CXXFLAGS = -std=c++11 -Wall -g -I $(DRIVER_DIR) -I/usr/include/python2.7/ -lpython2.7
SNFLAGS = 
BIN = ./bin
SNDIR = ../../SwarmNet/src/
SODIR = ../../SwarmOS/src/
SNOBJ = util.o publisher.o packet.o subscriber.o channel.o swarmnet.o
SOOBJ = swarmos.o
COACHOBJ = user_program.o
SHAREDOBJ = coach_driver.so

# ****************************************************
# Targets

user_program: $(SNOBJ) $(SOOBJ) $(COACHOBJ)
	$(CXX) $(SNFLAGS) $(CXXFLAGS) -I $(BENCHMARK_DIR) -c $(DRIVER_DIR)/coach_driver.cpp -o $(BIN)/coach_driver.so -shared
	$(CXX) $(SNFLAGS) $(CXXFLAGS) $(patsubst %, $(BIN)/%, $^) -o $@

#coach_driver.so: $(DRIVER_DIR)/coach_driver.cpp
#	$(CXX) $(SNFLAGS) $(CXXFLAGS) -I $(BENCHMARK_DIR) -c $< -o $(BIN)/$@ -shared

%.o: ./%.cpp
	$(CXX) $(SNFLAGS) $(CXXFLAGS) -I $(BENCHMARK_DIR) -c $< -o $(BIN)/$@

%.o: $(SNDIR)/%.cpp
	$(CXX) $(SNFLAGS) $(CXXFLAGS) -c $< -o $(BIN)/$@

%.o: $(SODIR)/%.cpp
	$(CXX) $(SNFLAGS) $(CXXFLAGS) -c $< -o $(BIN)/$@

clean:
	rm -rf ./bin/* simulation
