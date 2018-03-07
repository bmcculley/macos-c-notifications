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

CFStringRef arg_to_cfstr(char **argv, uintptr_t *i, uintptr_t argc) {
    char *c_str = calloc(sizeof(char), 150);
    while(*i != (argc-1) && argv[++*i][0] != '-'){
        strcat(c_str, argv[*i]);
        strcat(c_str, " ");
    }
    --*i;
    CFStringRef ret_str = c_cfstr(c_str);
    free(c_str);    
    return ret_str;  
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

id init_notification_center() {
    return objc_msgSend((id)objc_getClass("NSUserNotificationCenter"),
                                  sel_registerName("defaultUserNotificationCenter"));
}

id init_notification() {
    return objc_msgSend((id)objc_getClass("NSUserNotification"),
                            sel_registerName("alloc"),
                            sel_registerName("init"));
}

void send_notification(CFStringRef title, CFStringRef subtitle, CFStringRef info_text,
        CFStringRef sound_name)
{
    id pool = (id)objc_getClass("NSAutoreleasePool");
    
    pool = objc_msgSend(pool,
                        sel_registerName("alloc"),
                        sel_registerName("init"));
    
    set_bundle_id();
    id notifCenter = init_notification_center();
    id notif = init_notification();
    
    if (title != NULL) {
        objc_msgSend(notif, sel_registerName("setTitle:"), title);
        CFRelease(title);
    }
    if (subtitle != NULL) {
        objc_msgSend(notif, sel_registerName("setSubtitle:"), subtitle);
        CFRelease(subtitle);
    }
    if (info_text != NULL) {
        objc_msgSend(notif, sel_registerName("setInformativeText:"), info_text);
        CFRelease(info_text);
    }
    if (sound_name != NULL) {
        objc_msgSend(notif, sel_registerName("setSoundName:"), sound_name);
        CFRelease(sound_name);
    }

    objc_msgSend(notifCenter, sel_registerName("deliverNotification:"), notif);
    
    sleep(1);
    objc_msgSend(pool, sel_registerName("release"));

}


int main(int argc, char** argv) {
    CFStringRef title = NULL;
    CFStringRef subtitle = NULL;
    CFStringRef info_text = NULL;
    CFStringRef sound_name = NULL;

    // check for piped data
    if ( !isatty(STDIN_FILENO) ) {
        char buf[BUFSIZ];
        char msg[BUFSIZ];
        while (fgets(buf, sizeof buf, stdin)) {
            strcat(msg, buf);
        }
        if (msg[strlen(msg)-1] == '\n') {
            title = c_cfstr("Pipe");
            info_text = c_cfstr(msg);
        }
    }

    if (argc > 1) {
        for (uintptr_t i = 1; i < argc; i++) {
            if ( !strcmp(argv[i], "-title") ) {
                title = arg_to_cfstr(argv, &i, argc); 
            }
            else if ( !strcmp(argv[i], "-subtitle") ) {
                subtitle = arg_to_cfstr(argv, &i, argc);          
            }
            else if ( !strcmp(argv[i], "-msg") ) {
                info_text = arg_to_cfstr(argv, &i, argc);
            }
            else if ( !strcmp(argv[i], "-sound") ) {
                i++;
                sound_name = c_cfstr(argv[i]);            
            }
            else if ( !strcmp(argv[i], "-help") ) {
                help_msg(argv[0]);            
            }
        }
    }

    if(title) {
        send_notification(title, subtitle, info_text, sound_name);
    } else {
        puts("You must specify a title or the notification will not post\n");
    }
    return 0;
}
