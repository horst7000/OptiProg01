CXXFLAGS = -g -Iinclude/

myProg: main.o
	$(CXX) $(CXXFLAGS) -o $@ $^
	
main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $<