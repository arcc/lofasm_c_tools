CFLAGS = -O3 -D_GNU_SOURCE \
	-Wall -Wno-unused-function -Wno-unused-but-set-variable
LDLIBS = -lz -lm
HEADERS = markdown_parser.h charvector.h lofasmIO.h
OBJS = lofasmIO.o markdown_peg.o markdown_parser.o charvector.o
PROGS = md2man lfslice lfcat lftest bxresample lftype lfplot2d lfstats
CC = gcc
LIBS = liblofasmio.a(lofasmIO.o)

# The basic targets need only standard (implicit) rules.
all: $(PROGS)$(LIBS)
$(PROGS): $(HEADERS) $(OBJS)
$(OBJS): $(HEADERS)

# A more elaborate rule for the package to generate its own documentation.
man: README.md formats.md $(HEADERS) $(PROGS) $(OBJS:.o=.c)
	mkdir -p doc
	cd doc; ../md2man -m ../README.md
	cd doc; ../md2man -m ../formats.md
	cd doc; ../md2man -b "/* <BEGIN>" -e "<END> */" -m ../*.c ../*.h
	echo '/usr/bin/cat $$3' > man; (PATH=""; for file in $(PROGS); do $$file -H > doc/$$file.1; done); rm man

check: $(PROGS)
	./lfcat -v3 -p nan testdata/20160619_000326_AA.bbx.gz testdata/20160619_000828_AA.bbx.gz testplot.bbx.gz
	./lfslice -v3 -f 0+88e6 testplot.bbx.gz | ./lfplot2d -v3 -x 704 -y 600 -c hot -l 1e3 -s 1e-3 -p - testplot.eps
	rm testplot.bbx.gz
	@echo -e "\x1b[32;1mSUCCESS:\x1b[0m output in testplot.eps; view with gv(1)"

clean:
	rm -f $(OBJS) $(PROGS)
	rm -rf doc
	rm -f testplot.eps
