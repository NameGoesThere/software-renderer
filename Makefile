.PHONY: clean

software-renderer:
	gcc -Wall -o software-renderer *.c -lm -O3

clean:
	rm -f software-renderer