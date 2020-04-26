obj-m +=	multiplier.o

all:
	make -C ../linux-4.20.17 M=$(PWD) modules

clean:
	make -C ../linux-4.20.17 M=$(PWD) clean
