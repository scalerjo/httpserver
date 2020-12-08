TARGET = ./Example/ExampleServer.cpp
CXXFLAGS = -O3 -Wall -Werror -pedantic-errors -fmessage-length=0 -lboost_thread -lpthread
OUTPUT = Server

main:
	g++ $(TARGET) -o $(OUTPUT) $(CXXFLAGS)

clean:
	rm $(OUTPUT)
