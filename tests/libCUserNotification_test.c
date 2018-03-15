#include <stdio.h>
#include <inttypes.h>
#include <CoreFoundation/CoreFoundation.h>
#include <objc/objc-runtime.h>

#include "../CFTest/CFTest.h"
#include "../src/libCUserNotification/libCUserNotification.h"



void cfstringref_test1() {
	char *cstr = "test";
	CFStringRef should_ret = CFSTR("test");
	CF_ASSERT(c_cfstr(cstr), should_ret);
	CFRelease(should_ret);
}

void cfstringref_test2(){
	char *cstr = "this is a test";
	CFStringRef should_ret = CFSTR("this is a test");
	CF_ASSERT(c_cfstr(cstr), should_ret);
	CFRelease(should_ret);
}

void init_notification_test() {
	 id notif = objc_msgSend((id)objc_getClass("NSUserNotification"),
                            sel_registerName("alloc"),
                            sel_registerName("init"));

	 NS_ASSERT(init_notification(), notif);

	 objc_msgSend(notif, sel_registerName("release"));
}

int main(void) {
	CF_TEST_SUITE * cfstring_suite = CF_new_suite("C string to CFStringRef Suite");
	CF_add_test(cfstring_suite, "Single word Test",  &cfstringref_test1);
	CF_add_test(cfstring_suite, "Multi-word Test",  &cfstringref_test2);
	RUN_SUITE(cfstring_suite);

	CF_TEST_SUITE * nsobj_suite = CF_new_suite("NSObjects Suite");
	CF_add_test(nsobj_suite, "NSUserNotification does create",  &init_notification_test);
	RUN_SUITE(nsobj_suite);
}