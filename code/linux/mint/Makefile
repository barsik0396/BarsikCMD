CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
TARGET   = barsikcmd
SRCS     = main.cpp shell.cpp commands.cpp

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)