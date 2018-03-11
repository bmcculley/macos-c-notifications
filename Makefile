MKDIR_P = mkdir -p
BUILD_DIR = build

all: output clean

static:
	test -d $(BUILD_DIR) || $(MKDIR_P) $(BUILD_DIR)
	test -d $(BUILD_DIR)/static || $(MKDIR_P) $(BUILD_DIR)/static
	$(CC) -c -o $(BUILD_DIR)/static/libCUserNotification.o libCUserNotification.c
	ar rcs $(BUILD_DIR)/static/libCUserNotification.a $(BUILD_DIR)/static/libCUserNotification.o
	$(CC) -c notifier.c -o $(BUILD_DIR)/notify.o
	$(CC) -framework Foundation $(BUILD_DIR)/notify.o $(BUILD_DIR)/static/libCUserNotification.a -o notify

shared:
	test -d $(BUILD_DIR) || $(MKDIR_P) $(BUILD_DIR)
	test -d $(BUILD_DIR)/shared || $(MKDIR_P) $(BUILD_DIR)/shared
	$(CC) -c -fPIC -o $(BUILD_DIR)/shared/libCUserNotification.o libCUserNotification.c
	$(CC) -framework Foundation -shared $(BUILD_DIR)/shared/libCUserNotification.o -o $(BUILD_DIR)/shared/libCUserNotification.so
	$(CC) -c notifier.c -o $(BUILD_DIR)/notify.o
	$(CC) $(BUILD_DIR)/shared/libCUserNotification.so $(BUILD_DIR)/notify.o -o notify

output:
	test -d $(BUILD_DIR) || $(MKDIR_P) $(BUILD_DIR)
	test -d $(BUILD_DIR)/static || $(MKDIR_P) $(BUILD_DIR)/static
	$(CC) -c -o $(BUILD_DIR)/static/libCUserNotification.o libCUserNotification.c
	ar rcs $(BUILD_DIR)/static/libCUserNotification.a $(BUILD_DIR)/static/libCUserNotification.o
	$(CC) -c notifier.c -o $(BUILD_DIR)/notify.o
	$(CC) -framework Foundation $(BUILD_DIR)/notify.o $(BUILD_DIR)/static/libCUserNotification.a -o notify

clean:
	rm -rf $(BUILD_DIR)
