#include "libCUserNotification.h"

static CFStringRef c_cfstr(char * str) {
    return CFStringCreateWithCString(NULL, str, kCFStringEncodingMacRoman);
}

static void objc_swizzle(Class class, char *sel, Method method) {
    class_replaceMethod(class,
                        sel_registerName(sel),
                        method_getImplementation(method),
                        NULL);
}

static void set_bundle_id() {
    Method method = (Method)^{return CFSTR("com.apple.finder");};
    objc_swizzle(objc_getClass("NSBundle"), "bundleIdentifier", method);
}

static id get_default_user_notif_center() {
    return objc_msgSend((id)objc_getClass("NSUserNotificationCenter"),
                                  sel_registerName("defaultUserNotificationCenter"));
}

id init_notification() {
    return objc_msgSend((id)objc_getClass("NSUserNotification"),
                            sel_registerName("alloc"),
                            sel_registerName("init"));
}

void set_title(id *notif, char * title) {
    CFStringRef cf_title = c_cfstr(title);
    objc_msgSend(*notif, sel_registerName("setTitle:"), cf_title);
    CFRelease(cf_title);
}

void set_subtitle(id *notif, char * subtitle) {
    CFStringRef cf_sub = c_cfstr(subtitle);
    objc_msgSend(*notif, sel_registerName("setSubtitle:"), cf_sub);
    CFRelease(cf_sub);
}

void set_info_text(id *notif, char * info_text) {
    CFStringRef cf_info = c_cfstr(info_text);
    objc_msgSend(*notif, sel_registerName("setInformativeText:"), cf_info);
    CFRelease(cf_info);
}

void set_sound_name(id *notif, char * sound_name) {
    CFStringRef cf_sound = c_cfstr(sound_name);
    objc_msgSend(*notif, sel_registerName("setSoundName:"), cf_sound);
    CFRelease(cf_sound);
}

void post_notification(id *notif) {
    id pool = (id)objc_getClass("NSAutoreleasePool");
    
    pool = objc_msgSend(pool,
                        sel_registerName("alloc"),
                        sel_registerName("init"));

    set_bundle_id();
    objc_msgSend(get_default_user_notif_center(), sel_registerName("deliverNotification:"), *notif);

    sleep(1);
    objc_msgSend(pool, sel_registerName("release"));
}
