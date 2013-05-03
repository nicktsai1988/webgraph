OBJECTS = bitvector.o creategraph.o utils.o Compress.o graph.o HuffmanTree.o main.cpp edmonds.o Decompress.o DecodeTree.o
all: $(OBJECTS)
	g++ -o webgraph $(OBJECTS)
clean:
	rm -rf *.o

