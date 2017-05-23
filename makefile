testplan3: main.c sense.c move.c plan.c 
	gcc -o testplan3 main.c sense.c move.c plan.c -lwiringPi -lm
	chmod +x testplan
