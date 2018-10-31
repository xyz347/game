ifeq ($(OS),Windows_NT)
TAR=tss.exe
else
TAR=tss
endif
ALLFILE=$(wildcard *.c)
$(TAR):$(ALLFILE)
	gcc -O2 $^ -o $@

clean:
	-rm $(TAR)
