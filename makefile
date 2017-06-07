leviosa: main.c sense.c move.c plan.c mapas.c
	gcc -o mapetix main.c sense.c move.c plan.c mapas.c -lwiringPi -lm
	chmod +x mapetix
