ext2 :  readimage.o ext2_ls.o ext2_cp.o ext2_cp.o ext2_mkdir.o ext2_ln.o
	gcc -Wall -g -o ext2 $^

%.o : %.c ext2.h
	gcc -Wall -g -c $<

clean : 
	rm -f *.o ext2 *~