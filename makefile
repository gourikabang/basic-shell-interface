all: shell.c
	sudo apt-get update
	sudo apt-get install libreadline6 libreadline6-dev
	sudo apt-get install coreutils
	sudo apt-get update
	gcc shell.c -lreadline
clean:
	rm a.out