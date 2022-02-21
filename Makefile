all: a2 c64 c128 sdcc-c64
# sdcc-c64

a2:
	mkdir -p build-a2
	cd build-a2; make -f ../Makefile.a2

c64:
	mkdir -p build-c64
	cd build-c64; make -f ../Makefile.c64

c128:
	mkdir -p build-c128
	cd build-c128; make -f ../Makefile.c128

sdcc-c64:
	mkdir -p build-sdcc64
	cd build-sdcc64; make -f ../Makefile.sdcc64

clean:
	rm -rf build-a2 build-c64 build-c128 build-sdcc64 gmt_bin.tar.gz

bin: all
	tar cvfz gmt_bin.tar.gz build-a2/*.2mg build-a2/*.po build-a2/gmt.system \
		build-c64/*.bin build-c64/*.crt build-c64/gmt64 \
		build-c128/*.bin build-c128/gmt128

test: all
	x64 +sound -cart16 build-c64/gmt64.bin -georam -georamsize 512
	x64 +sound -cart16 build-sdcc64/gmt64.bin -georam -georamsize 512
	x128 +sound -extfrom build-c128/gmt128.bin -extfunc 1 -georam -georamsize 512
	mame apple2gs -window -sound none -ui_active -skip_gameinfo -ramsize 2M -flop3 build-a2/gmt.2mg

cart: all
	cat build-c64/gmt64.bin build-c64/gmt64.bin build-c64/gram1-64.bin build-c64/gram4-64.bin \
		build-c128/gmt128.bin build-c128/gmt128.bin build-c128/gram1-128.bin build-c128/gram4-128.bin \
		> cart.bin

.PRECIOUS: *.rel *.ihx
