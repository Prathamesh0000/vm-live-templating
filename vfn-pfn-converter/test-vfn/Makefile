
test: test.o hashset.o hashset_itr.o
	$(CC) $(CFLAGS) test.o hashset.o hashset_itr.o -o $@

test.o: test.c
	$(CC) $(CFLAGS) -c test.c -o $@

hashset.o: hashset.c
	$(CC) $(CFLAGS) -c hashset.c -o $@
	
hashset_itr.o: hashset_itr.c
	$(CC) $(CFLAGS) -c hashset_itr.c -o $@

clean:
	rm -f *.o test

.PHONY: clean