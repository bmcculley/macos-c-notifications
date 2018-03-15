MKDIR_P = mkdir -p
BUILD_DIR = build

all: output

static:
	test -d $(BUILD_DIR) || $(MKDIR_P) $(BUILD_DIR)
	test -d $(BUILD_DIR)/static || $(MKDIR_P) $(BUILD_DIR)/static
	$(CC) -c -o $(BUILD_DIR)/static/libCUserNotification.o src/libCUserNotification/libCUserNotification.c
	ar rcs $(BUILD_DIR)/static/libCUserNotification.a $(BUILD_DIR)/static/libCUserNotification.o
	rm $(BUILD_DIR)/static/libCUserNotification.o

shared:
	test -d $(BUILD_DIR) || $(MKDIR_P) $(BUILD_DIR)
	test -d $(BUILD_DIR)/shared || $(MKDIR_P) $(BUILD_DIR)/shared
	$(CC) -c -fPIC -o $(BUILD_DIR)/shared/libCUserNotification.o src/libCUserNotification/libCUserNotification.c
	$(CC) -framework Foundation -shared $(BUILD_DIR)/shared/libCUserNotification.o -o $(BUILD_DIR)/shared/libCUserNotification.so
	rm $(BUILD_DIR)/shared/libCUserNotification.o

notify:
	test -d $(BUILD_DIR) || $(MAKE) static
	test -d $(BUILD_DIR)/static || $(MAKE) static
	test -d $(BUILD_DIR)/notifier || $(MKDIR_P) $(BUILD_DIR)/notifier
	$(CC) -c src/notifier.c -o $(BUILD_DIR)/notifier/notify.o
	$(CC) -framework Foundation $(BUILD_DIR)/notifier/notify.o $(BUILD_DIR)/static/libCUserNotification.a -o $(BUILD_DIR)/notifier/notify
	rm -f $(BUILD_DIR)/notifier/notify.o

test:
	cd CFTest && $(MAKE) lib
	test -d $(BUILD_DIR)/tests || $(MKDIR_P) $(BUILD_DIR)/tests
	$(CC) -framework Foundation -o $(BUILD_DIR)/tests/libCUserNotification_test tests/libCUserNotification_test.c $(BUILD_DIR)/static/libCUserNotification.a CFTest/lib/libCFTest.a 
	rm -rf CFTest/lib
	./$(BUILD_DIR)/tests/libCUserNotification_test

output: static shared notify test