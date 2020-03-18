all: ext2_ls ext2_cp ext2_mkdir ext2_ln ext2_rm
#	ext2_ls ext2_cp ext2_mkdir ext2_ln ext2_rm

ext2_ls : ext2.h readimage.o ext2_ls.o
	gcc -Wall -g -o ext2_ls $^

ext2_cp : ext2.h readimage.o ext2_cp.o
	gcc -Wall -g -o ext2_cp $^

ext2_mkdir : ext2.h readimage.o ext2_mkdir.o
	gcc -Wall -g -o ext2_mkdir $^

ext2_ln : ext2.h readimage.o ext2_ln.o
	gcc -Wall -g -o ext2_ln $^

ext2_rm : ext2.h readimage.o ext2_rm.o
	gcc -Wall -g -o ext2_rm $^

# %: %.o ext2.h readimage.o
# 	gcc -Wall -g -o % ${%} $^

%.o : %.c ext2.h
	gcc -Wall -g -c $<

clean : 
	rm -f *.o ext2_ls ext2_cp ext2_mkdir ext2_ln ext2_rm *~