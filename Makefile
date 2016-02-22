EXE = peak
CXX = gcc

ifdef SystemRoot # Windows
	DEP = win32.c
	LIB = -lpsapi
	RM = del /q
else
	DEP = unix.c
	RM = rm -f
endif

$(EXE) : peak.c $(DEP)
	$(CXX) -o $(EXE) $< $(LIB)

clean :
	$(RM) $(EXE)
