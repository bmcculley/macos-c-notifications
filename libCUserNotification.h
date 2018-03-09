#ifndef LIBCUSERNOTIFICATION_H_INCLUDED
#define LIBCUSERNOTIFICATION_H_INCLUDED
#include <CoreFoundation/CoreFoundation.h>
#include <objc/objc-runtime.h>

id init_notification();
void set_title(id *notif, char * title);
void set_subtitle(id *notif, char * subtitle);
void set_info_text(id *notif, char * info_text);
void set_sound_name(id *notif, char * sound_name);
void post_notification(id *notif);

#endif