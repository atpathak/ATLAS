
ROOTINC = $(shell root-config --cflags)
ROOTLIB = $(shell root-config --libs)

OBJ = AtlasLabels.o AtlasStyle.o AtlasUtils.o PileUp.o

ATLASOBJ = AtlasExample.o $(OBJS)
BASICOBJ = BasicExample.o $(OBJS)
PILEUPOBJ = PileUp.o $(OBJS)

all : atlastest basictest pileuptest

.SUFFIXES: .C .o

.C.o :
	g++ $(ROOTINC) -c $<

atlastest : $(OBJ)
	g++  -fPIC -o $@ $(OBJ) $(ROOTLIB)

basictest : $(OBJ)
	g++ -fPIC -o $@ $(OBJ) $(ROOTLIB)

pileuptest : $(OBJ)
	g++ -fPIC -o $@ $(OBJ) $(ROOTLIB)

archive : clean
	cd .. ; tar -czf atlasstyle-00-04-00.tgz atlasstyle-00-04-00		  

clean : 
	rm -rf *.o *~
