#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <CoreFoundation/CoreFoundation.h>
#include <objc/objc-runtime.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void help_msg(char * progname);

CFStringRef c_cfstr(char * str);

void objc_swizzle(Class class, char *sel, Method method);

void set_bundle_id();

id get_default_user_notif_center();

id init_notification();

void set_title(id *notif, char * title);

void set_subtitle(id *notif, char * subtitle);

void set_info_text(id *notif, char * info_text);

void set_sound_name(id *notif, char * sound_name);

void post_notification(id *notif);

#endif