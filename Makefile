.PHONY: clean

software-renderer:
	gcc -Wall -o software-renderer *.c -lm -lX11 -O3

clean:
	rm -f software-renderer