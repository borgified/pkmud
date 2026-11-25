CC      = g++
PROF    =
NOCRYPT =  
C_FLAGS =  -Wall $(PROF) $(NOCRYPT) -ggdb
L_FLAGS =  $(PROF) -ltwitcurl

OUT_FILE = PKAnnounce

O_FILES = PKAnnounce.cpp

PKAnnounce: $(O_FILES)
	rm -f $(OUT_FILE)
	$(CC) $(L_FLAGS) -o $(OUT_FILE) $(O_FILES)
	chmod 4755 $(OUT_FILE)

.c.o: PKAnnounce.h
	$(CC) -c $(C_FLAGS) $<

clean:
	rm -f *.o PKAnnounce
