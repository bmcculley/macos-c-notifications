all: output clean
output:
	$(CC) -c -o libCUserNotification.o libCUserNotification.c  
	ar rcs libCUserNotification.a libCUserNotification.o
	$(CC) -framework Foundation -o notify notifier.c libCUserNotification.a

clean:
	rm -f libCUserNotification.o
