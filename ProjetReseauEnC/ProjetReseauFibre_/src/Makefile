CC = gcc
CFLAGS = -Wall -Wextra
LDLIBS = -lm

all: ChaineMain ReconstitueReseau

ChaineMain: chaine.c SVGwriter.c ChaineMain.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

ReconstitueReseau: chaine.c Reseau.c SVGwriter.c ReconstitueReseau.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f ChaineMain ReconstitueReseau *.o *.html *_modifie.cha

.PHONY: all clean
