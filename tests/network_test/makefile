TARGET  := nettest
BLD_DIR := build/
SRC_DIR := ../../
SOURCES := $(shell find $(SRC_DIR) -name "*.cpp")
HEADERS := $(shell find $(SRC_DIR) -name "*.hpp")
OBJECTS := $(patsubst $(SRC_DIR)%.cpp,$(BLD_DIR)%.o,$(SOURCES))
CXXFLAGS := -std=c++14 -DBOOST_LOG_DYN_LINK -DBOOST_TEST_DYN_LINK
LDLIBS := -pthread -lboost_system -lboost_filesystem -lboost_unit_test_framework -lboost_log
INCLUDE := -I$(SRC_DIR)

all: $(TARGET)

$(TARGET): $(OBJECTS)	
	$(CXX) $(CXXFLAGS) -o $(BLD_DIR)$@ $^ $(LDLIBS)
	
$(OBJECTS): $(BLD_DIR)%.o: $(SRC_DIR)%.cpp $(HEADERS)	
	mkdir -vp $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -f $(BLD_DIR)$(TARGET)
