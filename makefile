testplan: main.c
	gcc -o testplan main.c sense.c move.c plan.c -lwiringPi -lwiringPiDev -lpthread -lm
	chmod +x testplan