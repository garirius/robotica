leviosa: main.c sense.c move.c plan.c mapas.c
	gcc -o mapas main.c sense.c move.c plan.c mapas.c -lwiringPi -lm
	chmod +x mapas
