CFLAGS=-Llib 
LFLAGS=-L../zht/lib -lzht
LFLAGS+=-lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotobuf-c
PROTOBUF_HOME=/usr/local/include/google/protobuf
CFLAGS+=-I$(PROTOBUF_HOME)

all : spade_query
	make libprovenance.a
	make spade_query

libprovenance.a : provenance.o
	ar -rcs libprovenance.a provenance.o

provenance.o : provenance.cpp provenance.h
	g++ -Wall -fPIC -std=c++0x $(CPPFLAGS) $(CFLAGS) provenance.cpp -c -o provenance.o $(LFLAGS)
	
#spade_log.o : spade_log.cpp spade_log.h
#	g++ -fPIC -Wall $(CFLAGS) spade_log.cpp -c -o spade_log.o

spade_query :
	g++ -Wall -fPIC -std=c++0x $(CPPFLAGS) $(CFLAGS) query.cpp -o spade_query $(LFLAGS)

clean:
	rm *.o
	rm *.a
	rm spade_query
#	rm provenance
