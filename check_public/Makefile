#Dinica Mihnea-Gabriel 323CA

TARGETS=beamdrone curatare curse fortificatii
build:	$(TARGETS)

beamdrone: beamdrone.cpp
	g++ beamdrone.cpp -o beamdrone

curatare: curatare.cpp
	g++ curatare.cpp -o curatare

curse: curse.cpp
	g++ curse.cpp -o curse

fortificatii: fortificatii.cpp
	g++ fortificatii.cpp -o fortificatii

run-p1:
	./curatare

run-p2:
	./fortificatii
	
run-p3:
	./beamdrone

run-p4:
	./curse

clean:
	rm -f *.class $(TARGETS) *.o
