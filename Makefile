# ---------------------------------------------------------------------------
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2 as
#  published by the Free Software Foundation.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#  As a special exception, you may use this file as part of a free software
#  library without restriction.  Specifically, if other files instantiate
#  templates or use macros or inline functions from this file, or you compile
#  this file and link it with other files to produce an executable, this
#  file does not by itself cause the resulting executable to be covered by
#  the GNU General Public License.  This exception does not however
#  invalidate any other reasons why the executable file might be covered by
#  the GNU General Public License.
#
# ---------------------------------------------------------------------------

# compilatore da usare
CC		=  gcc
# aggiungo alcuni flags di compilazione
CCFLAGS	        += -std=c99 -Wall
# gli includes sono nella directory includes
INCDIR          = ./lib/sc_handler
# cerco gli include oltre che in INCDIR anche nella dir. corrente
INCLUDES	= -I. -I $(INCDIR)
# librerie da linkare a tutti i files (es. -lm per le librerie matematiche)
LDFLAGS 	=
# altre librerie  (es. -L<path> -lmylib)
LIBS1		= -L ./lib/sc_handler -lsc_handler
# flags di ottimizzazione
OPTFLAGS	= # -O3
# flags di debugging
DBGFLAGS        = -g
FLAGS           = $(DBGFLAGS) $(OPTFLAGS)
ARFLAGS         = rvs

TARGETS		= bin/main

.PHONY: all clean cleanall
.SUFFIXES: .c .h

lib/sc_handler/%.o: lib/sc_handler/%.c
	$(CC) $(CCFLAGS) $(INCLUDES) $(FLAGS) -c -fPIC -o $@ $<

# root target
all		: $(TARGETS)

# NOTA: $^ prende tutte le occorrenze della dependency list
#       $< prende solo la prima entry della dependency list
bin/main: src/main.o lib/sc_handler/sc_handler.so
	$(CC) $(CFLAGS) $(INCLUDES) $(FLAGS) -o $@ $< -Wl,-rpath=${PWD}/lib/sc_handler  $(LDFLAGS) $(LIBS1)

lib/sc_handler/sc_handler.so: ./lib/sc_handler/sc_handler.o $(INCDIR)/sc_handler.h
	$(CC) -shared -o $@ $<

clean		:
	-rm -f $(TARGETS)
cleanall	: clean
	-rm -f lib/sc_handler/*.o lib/sc_handler/*.so *~ bin/*


