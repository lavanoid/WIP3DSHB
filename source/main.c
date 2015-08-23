#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <3ds.h>
#include "brew_bgr.h"
#include "jsmn.h"
#define BUF_SIZE 1024

u32 actionOld = 0;
char HOST[] = "192.168.3.1"; // Host.

// Yeah I know this code probably looks really crappy. That's because I'm a total noob.

void http_download(httpcContext *context) {
    Result ret=0;
    u32 statuscode=0;
    //u32 size=0;
    u32 contentsize=0;
    u8 *buf;

    ret = httpcBeginRequest(context);
    if(ret!=0) {
        printf("[ERROR 01] Could not contact the host :(\n");
        return;
    }

    ret = httpcGetResponseStatusCode(context, &statuscode, 0);
    if(ret!=0) {
        printf("[ERROR 02] Could not contact the host :(\n");
        return;
    }
    
    if (statuscode!=200) {
        printf("[SERVER - ERROR] Status code NOT = 200\n");
        return;
    } else {
        printf("[SERVER] Status code = 200 :D\n");
    }

    ret=httpcGetDownloadSizeState(context, NULL, &contentsize);
    if(ret!=0) {
        printf("[ERROR 03] Could not contact the host :(\n");
        return;
    }


    buf = (u8*)malloc(contentsize);
    if(buf==NULL)return;
    memset(buf, 0, contentsize);
    ret = httpcDownloadData(context, buf, contentsize, NULL);
}

void ping(char* url) {
    httpcContext context;
    Result ret = 0;
    ret = httpcOpenContext(&context, url , 0);
    printf("[PING] %s\n", url);
    if(ret==0) http_download(&context);
    httpcCloseContext(&context);
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

int main() {
    gfxInitDefault();
    gfxSet3D(false);
    httpcInit();
    gfxSetDoubleBuffering(GFX_BOTTOM, false);
    u8* fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
    memcpy(fb, brew_bgr, brew_bgr_size);
    consoleInit(GFX_TOP,NULL);
    
    
    
    printf("####\\\\\\ JSON TEST ///####\n\n");
    /// Test stuff.
    /////////////////////////////////////////
    
    u8* file_buffer;
    FILE *file = fopen("jsbr-ford-mustang.json","rb");
    if (file == NULL) printf("Error.\n");
    fseek(file,0,SEEK_END);
    off_t size = ftell(file);
    fseek(file,0,SEEK_SET);
    file_buffer=malloc(size);
    if(!file_buffer) printf("Error.\n");
    off_t bytesRead = fread(file_buffer,1,size,file);
    fclose(file);
    if(size!=bytesRead) printf("Error.\n");
    int i;
    int r;
    jsmn_parser p;
    jsmntok_t t[128];
    jsmn_init(&p);
    r = jsmn_parse(&p, file_buffer, size, t, sizeof(t)/sizeof(t[0]));
    
    if (r < 0) {
        printf("Failed to parse JSON: %d\n", r);
        return 1;
    }
    if (r < 1 || t[0].type != JSMN_OBJECT) {
        printf("Object expected\n");
        return 1;
    }
    for (i = 1; i < r; i++) {
        if (jsoneq(file_buffer, &t[i], "name") == 0) {
            printf("- Name: %.*s\n", t[i+1].end-t[i+1].start,
            file_buffer + t[i+1].start);
            i++;
        } else if (jsoneq(file_buffer, &t[i], "description") == 0) {
            printf("- Desc: %.*s\n", t[i+1].end-t[i+1].start,
            file_buffer + t[i+1].start);
            i++;
        } else if (jsoneq(file_buffer, &t[i], "frequency") == 0) {
            printf("- Freq: %.*s\n", t[i+1].end-t[i+1].start,
            file_buffer + t[i+1].start);
            i++;
        } else if (jsoneq(file_buffer, &t[i], "synchronization_burst_us") == 0) {
            printf("- Sync burst: %.*s\n", t[i+1].end-t[i+1].start,
            file_buffer + t[i+1].start);
            i++;
        } else if (jsoneq(file_buffer, &t[i], "synchronization_spacing_us") == 0) {
            printf("- Sync spacing: %.*s\n", t[i+1].end-t[i+1].start,
            file_buffer + t[i+1].start);
            i++;
        } else if (jsoneq(file_buffer, &t[i], "total_synchronizations") == 0) {
            printf("- Total sync: %.*s\n", t[i+1].end-t[i+1].start,
            file_buffer + t[i+1].start);
            i++;
        } else if (jsoneq(file_buffer, &t[i], "signal_burst_us") == 0) {
            printf("- Signal burst: %.*s\n", t[i+1].end-t[i+1].start,
            file_buffer + t[i+1].start);
            i++;
        } else if (jsoneq(file_buffer, &t[i], "signal_spacing_us") == 0) {
            printf("- Signal spacing: %.*s\n", t[i+1].end-t[i+1].start,
            file_buffer + t[i+1].start);
            i++;
        }
        else if (jsoneq(file_buffer, &t[i], "host") == 0) {
            printf("- HOST IP: %.*s\n", t[i+1].end-t[i+1].start, file_buffer + t[i+1].start);
            
            // This part causes it to force close/crash :/
            char* test;
            sprintf(test, "%s", t[i+1].end-t[i+1].start, file_buffer + t[i+1].start);
            printf("%s\n", test);
            
            i++;
        }
    }
    

    ////////////////////////////////////////
    
    printf("[STAT] Enabled.\n[STAT] Host: %s\n", HOST);
    char buffer [BUF_SIZE];
    snprintf( buffer, BUF_SIZE, "http://%s/command/enable/", HOST );
    ping( buffer );

    // Main loop
    while (aptMainLoop()) {
        gspWaitForVBlank();
        hidScanInput();
        u32 kDown = hidKeysDown();
        //u32 kUp = hidKeysUp();
        u32 kHeld = hidKeysHeld();
        u32 kHeldOld = 0;
        u32 action = 0;
        
        if (hidKeysHeld() == 0) {
            if (action != actionOld) {
                consoleClear(); printf("[STAT] Stopped.\n");
                char buffer [BUF_SIZE];
                snprintf( buffer, BUF_SIZE, "http://%s/command/stop/", HOST );
                ping( buffer );
            }
        } else if (kHeld != kHeldOld) {
            if (kHeld & KEY_UP) {
                if (kHeld & KEY_X) {
                    action = 1;
                        if (action != actionOld) {
                            consoleClear(); printf("[STAT] Moving forward...\n");
                            char buffer [BUF_SIZE];
                            snprintf( buffer, BUF_SIZE, "http://%s/command/forward/", HOST );
                            ping( buffer );
                        }
                } else if (kHeld & KEY_B) {
                    action = 2;
                    if (action != actionOld) {
                        consoleClear(); printf("[STAT] Turning right...\n");
                        char buffer [BUF_SIZE];
                        snprintf( buffer, BUF_SIZE, "http://%s/command/right/", HOST );
                        ping( buffer );
                    }
                } else {
                    action = 3;
                    if (action != actionOld) {
                        consoleClear(); printf("[STAT] Left wheel moving forward...\n");
                        char buffer [BUF_SIZE];
                        snprintf( buffer, BUF_SIZE, "http://%s/command/left_forward/", HOST );
                        ping( buffer );
                    }
                }
            } else if (kHeld & KEY_DOWN) {
                if (kHeld & KEY_B) {
                    action = 4;
                    if (action != actionOld) {
                        consoleClear(); printf("[STAT] Moving backwards...\n");
                        char buffer [BUF_SIZE];
                        snprintf( buffer, BUF_SIZE, "http://%s/command/backward/", HOST );
                        ping( buffer );
                    }
                } else if (kHeld & KEY_X) {
                    action = 5;
                    if (action != actionOld) {
                        consoleClear(); printf("[STAT] Turning left...\n");
                        char buffer [BUF_SIZE];
                        snprintf( buffer, BUF_SIZE, "http://%s/command/left/", HOST );
                        ping( buffer );
                    }
                } else {
                    action = 6;
                    if (action != actionOld) {
                        consoleClear(); printf("[STAT] Left wheel moving backwards...\n");
                        char buffer [BUF_SIZE];
                        snprintf( buffer, BUF_SIZE, "http://%s/command/left_back/", HOST );
                        ping( buffer );
                    }
                }
            } else if (kHeld & KEY_X) {
                if (kHeld & KEY_UP) {
                    action = 1;
                    if (action != actionOld) {
                        consoleClear(); printf("[STAT] Moving forward...\n");
                        char buffer [BUF_SIZE];
                        snprintf( buffer, BUF_SIZE, "http://%s/command/forward/", HOST );
                        ping( buffer );
                    }
                } else if (kHeld & KEY_DOWN) {
                    action = 5;
                    if (action != actionOld) {
                        consoleClear(); printf("[STAT] Turning left...\n");
                        char buffer [BUF_SIZE];
                        snprintf( buffer, BUF_SIZE, "http://%s/command/left/", HOST );
                        ping( buffer );
                    }
                } else {
                    action = 7;
                    if (action != actionOld) {
                        consoleClear(); printf("[STAT] Right wheel moving forward...\n");
                        char buffer [BUF_SIZE];
                        snprintf( buffer, BUF_SIZE, "http://%s/command/right_forward/", HOST );
                        ping( buffer );
                    }
                }
            } else if (kHeld & KEY_B) {
                if (kHeld & KEY_DOWN) {
                    action = 4;
                    if (action != actionOld) {
                        consoleClear(); printf("[STAT] Moving backwards...\n");
                        char buffer [BUF_SIZE];
                        snprintf( buffer, BUF_SIZE, "http://%s/command/backward/", HOST );
                        ping( buffer );
                    }
                } else if (kHeld & KEY_UP) {
                    action = 2;
                    if (action != actionOld) {
                        consoleClear(); printf("[STAT] Turning right...\n");
                        char buffer [BUF_SIZE];
                        snprintf( buffer, BUF_SIZE, "http://%s/command/right/", HOST );
                        ping( buffer );
                    }
                } else {
                    action = 8;
                    if (action != actionOld) {
                        consoleClear(); printf("[STAT] Right wheel moving backwards...\n");
                        char buffer [BUF_SIZE];
                        snprintf( buffer, BUF_SIZE, "http://%s/command/right_back/", HOST );
                        ping( buffer );
                    }
                }
            }
        }        
        kHeldOld = kHeld;
        actionOld = action;
        if(kDown & KEY_Y) {
            consoleClear(); printf("[STAT] SHUTTING DOWN SHUTTING DOWN SHUTTING DOWN SHUTTING DOWN \nSHUTTING DOWN SHUTTING DOWN SHUTTING DOWN SHUTTING DOWN ");
            char buffer [BUF_SIZE];
            snprintf( buffer, BUF_SIZE, "http://%s/command/shutdown/", HOST );
            ping( buffer );
            break;
        }
        if(kDown & KEY_SELECT) {
            consoleClear(); printf("[STAT] REBOOTING REBOOTING REBOOTING REBOOTING REBOOTING REBOOTING\nREBOOTING REBOOTING REBOOTING REBOOTING REBOOTING REBOOTING\nREBOOTING REBOOTING REBOOTING REBOOTING REBOOTING REBOOTING\n");
            char buffer [BUF_SIZE];
            snprintf( buffer, BUF_SIZE, "http://%s/command/reboot/", HOST );
            ping( buffer );
            break;
        }
        if(kDown & KEY_START) {
            consoleClear(); printf("[STAT] EXITING EXITING EXITING EXITING \nEXITING EXITING EXITING EXITING \nEXITING EXITING EXITING EXITING \nEXITING EXITING EXITING EXITING \nEXITING EXITING EXITING EXITING \n");
            char buffer [BUF_SIZE];
            snprintf( buffer, BUF_SIZE, "http://%s/command/disable/", HOST );
            ping( buffer );
            break;
        }

        // Flush and swap framebuffers
        gfxFlushBuffers();
        gfxSwapBuffers();
    }
    // Exit services
    httpcExit();
    gfxExit();
    return 0;
}
