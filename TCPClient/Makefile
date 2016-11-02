##
## @file Makefile
## Distributed Systems - simple_message-client Makefile
## 
## @author Christian Mödlhammer
## @date 2016-01-12
##
## @version 0.1
##


##
## ------------------------------------------------------------- variables --
##

CC=gcc
CFLAGS=-Wall -Werror -Wextra -Wstrict-prototypes -pedantic -fno-common -O3 -g -std=gnu11
LDFLAGS=-lsimple_message_client_commandline_handling
CP=cp
CD=cd
MV=mv
RM=rm
GREP=grep
DOXYGEN=doxygen 

OBJECTS=simple_message_client.o ##simple_message_client_commandline_handling.o

EXCLUDE_PATTERN=footrulewidth

##
## ----------------------------------------------------------------- rules --
##

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

##
## --------------------------------------------------------------- targets --
##

.PHONY: all
all: simple_message_client

simple_message_client: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ 

.PHONY: clean
clean:
	for f in $(OBJECTS); do if [ -e $$f ]; then rm $$f; fi done

distclean: clean
	@if [ -e *.doc ]; then $(RM) -r doc; fi
	@if [ -e simple_message_client ]; then $(RM) simple_message_client; fi


##
## --------------------------------------------------------------- documentation --
##

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
	$(MV) ./doxygen/latex/refman.pdf simple_message_client_refman.pdf

##
## ---------------------------------------------------------- dependencies --
##

##
## =================================================================== eof ==
##
