output:
	$(CC) -framework Foundation -o notify notify.c

clean:
	rm -f notify
