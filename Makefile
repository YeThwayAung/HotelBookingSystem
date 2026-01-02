CXX = g++
CXXFLAGS = -std=c++11 -Wall
OBJS = main.o Room.o DeluxeRoom.o Suite.o Admin.o HotelBookingSystem.o
TARGET = hotel

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cpp HotelBookingSystem.h
	$(CXX) $(CXXFLAGS) -c main.cpp

Room.o: Room.cpp Room.h
	$(CXX) $(CXXFLAGS) -c Room.cpp

DeluxeRoom.o: DeluxeRoom.cpp DeluxeRoom.h Room.h
	$(CXX) $(CXXFLAGS) -c DeluxeRoom.cpp

Suite.o: Suite.cpp Suite.h Room.h
	$(CXX) $(CXXFLAGS) -c Suite.cpp

Admin.o: Admin.cpp Admin.h
	$(CXX) $(CXXFLAGS) -c Admin.cpp

HotelBookingSystem.o: HotelBookingSystem.cpp HotelBookingSystem.h Room.h DeluxeRoom.h Suite.h Admin.h
	$(CXX) $(CXXFLAGS) -c HotelBookingSystem.cpp

clean:
	rm -f $(OBJS) $(TARGET)