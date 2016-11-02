##
## @file Makefile
## Betriebssysteme Myfind Makefile
## 
##
## @author Christian Mödlhammer
## @date 2015/03/06
##
## @version 1.0
##


##
## ------------------------------------------------------------- variables --
##

CC=gcc
CFLAGS=-DDEBUG -Wall -Werror -pedantic -Wextra -Wstrict-prototypes -fno-common -g -O3
CP=cp
CD=cd
MV=mv
RM=rm
GREP=grep
DOXYGEN=doxygen

OBJECTS=myfind.o eval.o hal.o che.o moe.o

EXCLUDE_PATTERN=footrulewidth

##
## ----------------------------------------------------------------- rules --
##

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

##
## --------------------------------------------------------------- targets --
##

all: myfind

myfind: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	for f in $(OBJECTS); do if [ -e $$f ]; then rm $$f; fi done

distclean: clean
	@if [ -e *.doc ]; then $(RM) -r doc; fi
	@if [ -e myfind ]; then $(RM) myfind; fi

doc: html pdf

html:
	$(DOXYGEN) doxygen.dcf

pdf: html
	$(CD) ./doxygen/latex && \
	$(MV) refman.tex refman_save.tex && \
	$(GREP) -v $(EXCLUDE_PATTERN) refman_save.tex > refman.tex && \
	$(RM) refman_save.tex && \
	make && \
	$(MV) refman.pdf refman.save && \
	$(RM) *.tex *.aux *.sty *.log *.out *.ind *.idx \
	      *.ilg *.toc Makefile && \
	$(MV) refman.save refman.pdf
	$(CD) ..
	$(CD) ..
	$(MV) ./doxygen/latex/refman.pdf myfind_refman.pdf

##
## ---------------------------------------------------------- dependencies --
##

##
## =================================================================== eof ==
##
