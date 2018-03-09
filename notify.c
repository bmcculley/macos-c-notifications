#include <CoreFoundation/CoreFoundation.h>
#include <objc/objc-runtime.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void help_msg(char * progname)
{
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
    "https://github.com/bmcculley/NS-C-UserNotification\n",
    progname );
}

CFStringRef c_cfstr(char * str) {

    return CFStringCreateWithCString(NULL, str, kCFStringEncodingMacRoman);
}

void concat_args(char *str, char **argv, uintptr_t *i, uintptr_t argc) {
    while(*i != (argc-1) && argv[++*i][0] != '-'){
        strcat(str, argv[*i]);
        strcat(str, " ");
    }
    --*i; 
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

_Bool send_notification(char * title, char * subtitle, char * info_text, char * sound_name) {
    id pool = (id)objc_getClass("NSAutoreleasePool");
    
    pool = objc_msgSend(pool,
                        sel_registerName("alloc"),
                        sel_registerName("init"));
    
    
    id notif = init_notification();
    
    set_title(&notif, title);
    
    if (subtitle) {
        set_subtitle(&notif, subtitle);
    }
    if (info_text) {
        set_info_text(&notif, info_text);
    }
    if (sound_name) {
        set_sound_name(&notif, sound_name);
    }

    post_notification(&notif);
    
    sleep(1);
    objc_msgSend(pool, sel_registerName("release"));

    return true;
}

int main(int argc, char** argv) {
    char * title = NULL;
    char * subtitle = NULL;
    char * info_text = NULL;
    char * sound_name = NULL;

    // check for piped data
    if ( !isatty(STDIN_FILENO) ) {
        char *buf = malloc(BUFSIZ);
        char *msg = malloc(BUFSIZ);
        title = calloc(sizeof(char), 5);
        info_text = calloc(sizeof(char), 350);
        while (fgets(buf, sizeof buf, stdin)) {
            strcat(msg, buf);
        }
        if (msg[strlen(msg)-1] == '\n') {
            strcat(title, "Pipe");
            memmove(info_text, msg, sizeof(*msg) * strlen(msg));
        }
        free(buf);
        free(msg);
    }

    if (argc > 1) {
        for (uintptr_t i = 1; i < argc; i++) {
            if ( !strcmp(argv[i], "-title") && !title) {
                title = calloc(sizeof(char), 100);
                concat_args(title, argv, &i, argc); 
            }
            else if ( !strcmp(argv[i], "-subtitle") ) {
                subtitle = calloc(sizeof(char), 150);
                concat_args(subtitle, argv, &i, argc);          
            }
            else if ( !strcmp(argv[i], "-msg") && !info_text) {
                info_text = calloc(sizeof(char), 350);
                concat_args(info_text, argv, &i, argc);
            }
            else if ( !strcmp(argv[i], "-sound") && i < (argc-1)) {
                sound_name = calloc(sizeof(char), 150);
                concat_args(sound_name, argv, &i, argc);  
            }
            else if ( !strcmp(argv[i], "-help") ) {
                help_msg(argv[0]);            
            }
        }
    }

    if(title) {
        if(!send_notification(title, subtitle, info_text, sound_name)) {
            puts("Something has gone terribly wrong.");
        }
        free(title);
    } else {
        puts("You must specify a title or the notification will not post\n");
    }

    if(subtitle) {
        free(subtitle);
    }

    if(info_text) {
        free(info_text);
    }

    if(sound_name) {
        free(sound_name);
    }

    return 0;
}