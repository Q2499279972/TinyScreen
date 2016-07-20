TARGETS = ../screen.exe  
CXXFLAGS = -pipe \
           -O2 -static\
           -fthread-jumps \
           -fexpensive-optimizations \
           -fcheck-new \
           -Wall -W -pedantic -Wno-long-long \
           -Wunknown-pragmas -Wundef -Wconversion -Wno-missing-braces \
           -Wuninitialized -Wpointer-arith -Wno-multichar \
           -mwindows \

objs:=main.o ReadLuaConfig.o window.o

all:$(objs)  
	g++ $(CXXFLAGS) $(objs) ./liblua.a -o $(TARGETS) -lstdc++ -lopengl32 -lglu32
	strip -g $(TARGETS)
.cpp.o: 
	g++ -c $(CXXFLAGS) $< -o $@  
  