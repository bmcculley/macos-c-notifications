#include "functions.h"

void help_msg(char * progname) {
    printf("This is a command-line tool to send MacOS User Notifications.\n" \
    "\n" \
    "Usage: %s -title title -subtitle subtitle -msg message -sound default\n" \
    "\n" \
    "    -help            Display this help message.\n" \
    "    -title VALUE     The notification title.\n" \
    "    -subtitle VALUE  The notification subtitle.\n" \
    "    -msg VALUE       The notification message.\n" \
    "    -sound VALUE     The name of a sound to play when the notification \n" \
    "                     appears. The names are listed in Sound Preferences. \n" \
    "                     Use 'default' for the default notification sound.\n" \
    "\n" \
    "The source code for this can be found on github: \n" \
    "https://github.com/bmcculley/macos-c-notifications\n",
    progname );
}

CFStringRef c_cfstr(char * str) {
    return CFStringCreateWithCString(NULL, str, kCFStringEncodingMacRoman);
}

void objc_swizzle(Class class, char *sel, Method method) {
    class_replaceMethod(class,
                        sel_registerName(sel),
                        method_getImplementation(method),
                        NULL);
}

void set_bundle_id() {
    Method method = (Method)^{return CFSTR("com.apple.finder");};
    objc_swizzle(objc_getClass("NSBundle"), "bundleIdentifier", method);
}

id get_default_user_notif_center() {
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
    set_bundle_id();
    objc_msgSend(get_default_user_notif_center(), sel_registerName("deliverNotification:"), *notif);
}
