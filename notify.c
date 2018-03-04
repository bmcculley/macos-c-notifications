#include <CoreFoundation/CoreFoundation.h>
#include <objc/objc-runtime.h>
#include <string.h>

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

void send_notification(char * title, char * subtitle, char * infotext,
        char * soundname)
{
    id pool = (id)objc_getClass("NSAutoreleasePool");
    
    pool = objc_msgSend(pool,
                        sel_registerName("alloc"),
                        sel_registerName("init"));
    
    Class bundle = objc_getClass("NSBundle");

    /** TODO:
     * this converting char arrays to cfstring could probably be moved 
     * to a function, something to look into...
     */
    char *bytes;

    //Block swizzle 
    class_replaceMethod(bundle,
                        sel_registerName("bundleIdentifier"),
                        method_getImplementation((Method)^{return CFSTR("com.apple.finder");}),
                        NULL);
    
    id notifCenter = objc_msgSend((id)objc_getClass("NSUserNotificationCenter"),
                                  sel_registerName("defaultUserNotificationCenter"));
    
    id notif = objc_msgSend((id)objc_getClass("NSUserNotification"),
                            sel_registerName("alloc"),
                            sel_registerName("init"));
    
    if ( title != NULL) {
        CFStringRef strTitle;
        bytes = CFAllocatorAllocate(CFAllocatorGetDefault(), 6, 0);
        strcpy(bytes, title);
        strTitle = CFStringCreateWithCStringNoCopy(NULL, bytes,
            kCFStringEncodingMacRoman, NULL);
        objc_msgSend(notif, sel_registerName("setTitle:"), strTitle);
        CFRelease(strTitle);
    }
    if ( subtitle != NULL ) {
        CFStringRef strSubtitle;
        bytes = CFAllocatorAllocate(CFAllocatorGetDefault(), 6, 0);
        strcpy(bytes, subtitle);
        strSubtitle = CFStringCreateWithCStringNoCopy(NULL, bytes,
            kCFStringEncodingMacRoman, NULL);
        objc_msgSend(notif, sel_registerName("setSubtitle:"), strSubtitle);
        CFRelease(strSubtitle);
    }
    if ( infotext != NULL ) {
        CFStringRef strInfoText;
        bytes = CFAllocatorAllocate(CFAllocatorGetDefault(), 6, 0);
        strcpy(bytes, infotext);
        strInfoText = CFStringCreateWithCStringNoCopy(NULL, bytes,
            kCFStringEncodingMacRoman, NULL);
        objc_msgSend(notif, sel_registerName("setInformativeText:"), strInfoText);
        CFRelease(strInfoText);
    }
    if ( soundname != NULL ) {
        CFStringRef strSoundName;
        bytes = CFAllocatorAllocate(CFAllocatorGetDefault(), 6, 0);
        strcpy(bytes, soundname);
        strSoundName = CFStringCreateWithCStringNoCopy(NULL, bytes,
            kCFStringEncodingMacRoman, NULL);
        objc_msgSend(notif, sel_registerName("setSoundName:"), strSoundName);
        CFRelease(strSoundName);
    }
    objc_msgSend(notifCenter, sel_registerName("deliverNotification:"), notif);
    
    sleep(1);
    objc_msgSend(pool, sel_registerName("release"));

}

int main(int argc, char** argv) 
{
    char * userTitle = NULL;
    char * userSubtitle = NULL;
    char * userInformativeText = NULL;
    char * userSoundName = NULL;

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if ( !strcmp(argv[i], "-title") ) {
                i++;
                userTitle = strdup(argv[i]);           
            }
            else if ( !strcmp(argv[i], "-subtitle") ) {
                i++;
                userSubtitle = strdup(argv[i]);           
            }
            else if ( !strcmp(argv[i], "-msg") ) {
                i++;
                userInformativeText = strdup(argv[i]);            
            }
            else if ( !strcmp(argv[i], "-sound") ) {
                i++;
                userSoundName = strdup(argv[i]);            
            }
            else if ( !strcmp(argv[i], "-help") ) {
                help_msg(argv[0]);            
            }
            else {
                printf("Oops, I didn't understand that. %s\n", argv[i]);
            }
        }
    }
    send_notification(userTitle, userSubtitle, userInformativeText, userSoundName);
    return 0;
}
