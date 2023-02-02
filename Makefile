all: DeauthAttack
main.o : main.cpp
DeauthAttack : main.o -ltins
	$(LINK.cc) $^  $(LDLIBS) -o $@

clean:
	rm -f DeauthAttack *.o