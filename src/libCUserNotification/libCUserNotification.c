#include "libCUserNotification.h"

CFStringRef c_cfstr(char * str) {
    return CFStringCreateWithCString(NULL, str, kCFStringEncodingMacRoman);
}

static Ivar class_getIvarFromString(Class cl, char *ivar_str) {
    unsigned int count;
    Ivar *ivars = class_copyIvarList(cl, &count);
    for (int i = 0; i < count; i++) {
        Ivar ivar = ivars[i];
        if(strcmp(ivar_getName(ivar), ivar_str) == 0) {
            free(ivars);
            return ivar;
        }
    }
    free(ivars);
    return NULL;
}

static void objc_swizzle(Class class, char *sel, Method method) {
    class_replaceMethod(class,
                        sel_registerName(sel),
                        method_getImplementation(method),
                        NULL);
}

static void set_default_bundle_id() {
    Method method = (Method)^{return CFSTR("com.apple.finder");};
    objc_swizzle(objc_getClass("NSBundle"), "bundleIdentifier", method);
}

static void set_custom_bundle_id(id defCenter, CFStringRef bID) {
    Ivar bidIvar = class_getIvarFromString(object_getClass(defCenter), "_bundleIdentifier");
    object_setIvar(defCenter, bidIvar, (id)bID);
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

usernotification_t * new_usernotification() {
    usernotification_t *notif = malloc(sizeof(usernotification_t));
    notif->notification = init_notification();
    notif->bundleID = NULL;
    return notif;
}

void set_title(usernotification_t *notif, char * title) {
    CFStringRef cf_title = c_cfstr(title);
    objc_msgSend(notif->notification, sel_registerName("setTitle:"), cf_title);
    CFRelease(cf_title);
}

void set_subtitle(usernotification_t *notif, char * subtitle) {
    CFStringRef cf_sub = c_cfstr(subtitle);
    objc_msgSend(notif->notification, sel_registerName("setSubtitle:"), cf_sub);
    CFRelease(cf_sub);
}

void set_info_text(usernotification_t *notif, char * info_text) {
    CFStringRef cf_info = c_cfstr(info_text);
    objc_msgSend(notif->notification, sel_registerName("setInformativeText:"), cf_info);
    CFRelease(cf_info);
}

void set_bid(usernotification_t *notif, char * bid) {
    notif->bundleID = malloc(strlen(bid));
    strncpy(notif->bundleID, bid, strlen(bid));
}

void set_sound_name(usernotification_t *notif, char * sound_name) {
    CFStringRef cf_sound = c_cfstr(sound_name);
    objc_msgSend(notif->notification, sel_registerName("setSoundName:"), cf_sound);
    CFRelease(cf_sound);
}

void post_notification(usernotification_t *notif) {
    id pool = (id)objc_getClass("NSAutoreleasePool");
    pool = objc_msgSend(pool, sel_registerName("alloc"), sel_registerName("init"));

    set_default_bundle_id();
    id def_notif_center = get_default_user_notif_center();

    if(notif->bundleID) {
        CFStringRef cf_bid = c_cfstr(notif->bundleID);
        set_custom_bundle_id(def_notif_center, cf_bid);
        objc_msgSend(def_notif_center, sel_registerName("deliverNotification:"), notif->notification);
        sleep(1);
        CFRelease(cf_bid);
    } else {
        objc_msgSend(def_notif_center, sel_registerName("deliverNotification:"), notif->notification);
        sleep(1);
    }

    objc_msgSend(pool, sel_registerName("release"));
}
