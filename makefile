CXX = g++
CXXFLAGS = -Wall -std=c++98 -g

SRCS = main.cpp server.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = ircserv

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

fclean: clean

re: fclean all
