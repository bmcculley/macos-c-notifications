output:
	$(CC) -framework Foundation -o notify notify.c functions.c

clean:
	rm -f notify
