all: ext2_ls ext2_cp ext2_mkdir ext2_ln ext2_rm readimage

# ext2_helper : ext2_helper.o ext2.h 
# 	gcc -Wall -g -o ext2_helper $^

ext2_ls : ext2_ls.o ext2.h ext2_helper.o
	gcc -Wall -g -o ext2_ls $^

ext2_cp : ext2_cp.o ext2.h ext2_helper.o
	gcc -Wall -g -o ext2_cp $^

ext2_mkdir : ext2_mkdir.o ext2.h ext2_helper.o
	gcc -Wall -g -o ext2_mkdir $^

ext2_ln : ext2_ln.o ext2.h ext2_helper.o
	gcc -Wall -g -o ext2_ln $^

ext2_rm : ext2_rm.o ext2.h ext2_helper.o
	gcc -Wall -g -o ext2_rm $^

readimage : readimage.o ext2.h

# %: %.o ext2.h readimage.o
# 	gcc -Wall -g -o % ${%} $^

%.o : %.c ext2.h
	gcc -Wall -g -c $<

clean : 
	rm -f *.o all *~