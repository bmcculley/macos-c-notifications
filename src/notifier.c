#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../src/libCUserNotification/libCUserNotification.h"

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

void concat_args(char *str, char **argv, uintptr_t *i, uintptr_t argc) {
    while(*i != (argc-1) && argv[++*i][0] != '-'){
        strcat(str, argv[*i]);
        strcat(str, " ");
    }
    --*i; 
}

_Bool send_notification(char * title, char * subtitle, char * info_text, char * sound_name) {
    
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