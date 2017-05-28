# Makefile for src/mod/bmotion.mod/
# $Id: Makefile,v 1.10 2003/04/01 05:33:41 wcc Exp $

srcdir = .

doofus:
	@echo ""
	@echo "Let's try this from the right directory..."
	@echo ""
	@cd ../../../ && make

static: ../bmotion.o

modules: ../../../bmotion.$(MOD_EXT)

../bmotion.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -DMAKING_MODS -c $(srcdir)/bmotion.c
	@rm -f ../bmotion.o
	mv bmotion.o ../

../../../bmotion.$(MOD_EXT): ../bmotion.o
	$(LD) -ldl -lgthread-2.0 -o ../../../bmotion.$(MOD_EXT) ../bmotion.o 
	$(STRIP) ../../../bmotion.$(MOD_EXT)

depend:
	$(CPP) $(CFLAGS) $(CPPFLAGS) -MM $(srcdir)/bmotion.c > .depend

clean:
	@rm -f .depend *.o *.$(MOD_EXT) *~
distclean: clean

