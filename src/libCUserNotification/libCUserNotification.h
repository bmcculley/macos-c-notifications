#ifndef LIBCUSERNOTIFICATION_H_INCLUDED
#define LIBCUSERNOTIFICATION_H_INCLUDED
#include <CoreFoundation/CoreFoundation.h>
#include <objc/objc-runtime.h>

typedef struct S_nsusernotification {
	id notification;
} usernotification_t;

CFStringRef c_cfstr(char * str);
id init_notification();
usernotification_t * new_usernotification();
void set_title(usernotification_t *notif, char * title);
void set_subtitle(usernotification_t *notif, char * subtitle);
void set_info_text(usernotification_t *notif, char * info_text);
void set_sound_name(usernotification_t *notif, char * sound_name);
void post_notification(usernotification_t *notif);

#endif