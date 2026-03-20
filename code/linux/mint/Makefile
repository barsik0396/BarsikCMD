CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS  = -lcurl
TARGET   = barsikcmd
SRCS     = main.cpp shell.cpp commands.cpp \
           commands/meow.cpp commands/update.cpp

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET)