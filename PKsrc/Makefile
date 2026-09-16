#CC      = gcc
CC      = g++
PROF    =
NOCRYPT =  
#C_FLAGS =  -Wall $(PROF) $(NOCRYPT) -ggdb
C_FLAGS =  -Wall -Wno-narrowing -Wno-write-strings $(PROF) $(NOCRYPT) -ggdb -DCPPBUILD
L_FLAGS =  $(PROF) -lm -lcrypt

OUT_FILE = PKexecutable

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o archer.o \
	  bfs.o cmod.o color.o comm.o const.o db.o \
	  fight.o grenade.o handler.o high.o interp.o last.o magic.o \
	  obj_fun.o pkmud.o \
	  random.o room_fun.o save.o shoot.o skills.o social.o  \
	  special.o strconst.o update.o wear_fun.o \
          twitterAnnounce.o CharIterator.o ObjIterator.o

#	  special.o strconst.o update.o wear_fun.o twitterPort.o

merc: $(O_FILES)
	rm -f $(OUT_FILE)
	$(CC) -o $(OUT_FILE) $(O_FILES) $(L_FLAGS)
	chmod 4755 $(OUT_FILE)

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<

.cpp.o: merc.h
	$(CC) -c $(C_FLAGS) $<

clean:
	rm -f *.o PKexecutable
