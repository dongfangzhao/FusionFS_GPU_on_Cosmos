fusionfs : fusionfs.o util.o log.o
	gcc -g -o fusionfs fusionfs.o util.o log.o -L/usr/local/lib64 -L./ffsnet -lffsnet_bridger -L./zht/lib -lzht -I./provenance -L./provenance -lprovenance -L/usr/local/cuda-5.0/lib64 -lcuda -lcudart -I/usr/local/cuda-5.0/include -L./gibraltar/lib -lgibraltar -lprotobuf -lprotobuf-c -lpthread `pkg-config fuse --libs` -lstdc++ -L/lib64 -lm

fusionfs.o : fusionfs.c log.h params.h util.h
	gcc -g -Wall -c fusionfs.c -L./ffsnet -lffsnet_bridger -I./provenance -L./provenance -lprovenance -I./gibraltar/inc -L./gibraltar/lib -lgibraltar -L/usr/local/cuda-5.0/lib64 -lcuda -lcudart -I/usr/local/cuda-5.0/include -lpthread `pkg-config fuse --cflags`
	
util.o : util.c log.h params.h
	gcc -g -Wall -c util.c -L./zht/lib -lzht -lstdc++ -lprotobuf -lprotobuf-c -lpthread `pkg-config fuse --cflags` 

log.o : log.c log.h params.h
	gcc -g -Wall -c log.c `pkg-config fuse --cflags`

clean:
	rm -f fusionfs *.o 

dist:
	rm -rf fuse-tutorial/
	mkdir fuse-tutorial/
	cp ../*.html fuse-tutorial/
	mkdir fuse-tutorial/example/
	mkdir fuse-tutorial/example/mountdir/
	mkdir fuse-tutorial/example/rootdir/
	echo "a bogus file" > fuse-tutorial/example/rootdir/bogus.txt
	mkdir fuse-tutorial/src
	cp Makefile *.c *.h fuse-tutorial/src/
	tar cvzf ../../fuse-tutorial.tgz fuse-tutorial/
	rm -rf fuse-tutorial/
