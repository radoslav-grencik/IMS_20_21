# author Radoslav Grenčík, xgrenc00@stud.fit.vutbr.cz

LOGIN = xgrenc00
BIN = xgrenc00
DOC = documentation

CC = g++
CFLAGS = -std=c++11 -Wall -Wextra -Werror -pedantic
LDLIBS = -lsimlib

HEADER = $(BIN).hpp
SRC = $(BIN).cpp
PROJECT_FILES = $(SRC) $(HEADER) Makefile $(DOC).pdf

$(BIN): $(SRC) $(HEADER)
	$(CC) $(SRC) -o $(BIN) $(CFLAGS) $(LDLIBS)

debug: $(SRC) $(HEADER)
	$(CC) $(SRC) -o $(BIN) $(CFLAGS) -g -DDEBUG=1 $(LDLIBS)

upload:
	scp $(SRC) $(HEADER) Makefile eva:~/IMS

zip:
	zip -r 06_$(LOGIN).zip $(PROJECT_FILES)

clean: docclean
	rm -f *.o *.out $(BIN)

# DOCUMENTATION
doc: $(DOC).tex $(DOC).bib pdfclean
	pdflatex $(DOC).tex
	bibtex $(DOC)
	pdflatex $(DOC).tex
	pdflatex $(DOC).tex
	make docclean

pdfclean:
	rm -f $(DOC).pdf

docclean:
	rm -f $(DOC).aux $(DOC).bbl $(DOC).blg $(DOC).log $(DOC).out $(DOC).toc $(DOC).synctex.gz

# ALL
all: clean $(BIN) upload doc zip
