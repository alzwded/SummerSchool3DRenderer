CPP = g++
CFLAGS = -g -c
LD = g++
LDFLAGS = -lglut -lGLU -lGL -lm

renderer.out: main.o drawing.o sample.bmp
	$(LD) -o renderer.out main.o drawing.o $(LDFLAGS)

main.o: main.cxx drawing.hxx core.hxx
	$(CPP) -o main.o $(CFLAGS) main.cxx

drawing.o: drawing.cxx drawing.hxx core.hxx bitmap.h
	$(CPP) -o drawing.o $(CFLAGS) drawing.cxx

clean:
	rm -f *.o renderer.out
