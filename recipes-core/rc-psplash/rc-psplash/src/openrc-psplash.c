#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <einfo.h>
#include <rc.h>


#define PSPLASH_BIN "/bin/psplash"
#define PSPLASH_WRITE_BIN "/bin/psplash-write"
#define PSPLASH_FIFO "/psplash_fifo"

#ifdef DEBUG
#define DBG(...) einfo(__VA_ARGS__)
#else
#define DBG(...)
#endif


static int psplash_start()
{
    int ret = fork();
    if (ret < 0) {
        eerror("Failed to start %s: %s\n", PSPLASH_BIN, strerror(errno));
        return ret;
    } else if (ret == 0) {
        execl(PSPLASH_BIN, "psplash", NULL);
        exit(0);
    }
    DBG("Pid of %s: %d", PSPLASH_BIN, ret);

    return 0;
}

static void psplash_write_msg(const char *msg, const char *name)
{
    char buff[128];
    int len = 0;

    if (name) {
        len = strlen(name);
    }

    if (strlen(msg) + strlen(buff) + len >= 128) {
        eerror("Message is to long\n");
        return;
    }

    if (name) {
        sprintf(buff, PSPLASH_WRITE_BIN " \"MSG %s %s\"", msg, name);
    } else {
        sprintf(buff, PSPLASH_WRITE_BIN " \"MSG %s\"", msg);
    }

    system(buff);
}

static void psplash_quit()
{
    char buff[128];

    sprintf(buff, PSPLASH_WRITE_BIN " QUIT");
    system(buff);
}

static void psplash_set_progress(int progress)
{
    char buff[128];

    if (progress < 0 || progress > 100) {
        return;
    }

    sprintf(buff, PSPLASH_WRITE_BIN " \"PROGRESS %d\"", progress);
    system(buff);
}

#define	TAILQ_FIRST(head)		((head)->tqh_first)
#define	TAILQ_END(head)			(NULL)
#define	TAILQ_NEXT(elm, field)		((elm)->field.tqe_next)
static int rc_stringlist_len(RC_STRINGLIST *list)
{
    RC_STRING *s1, *s2;
    int len = 0;

    if (!list) {
        return 0;
    }

    s1 = TAILQ_FIRST(list);
    while (s1) {
        s2 = TAILQ_NEXT(s1, entries);
        s1 = s2;

        len++;
    }

    return len;
}

static int rc_services_in_runlevel_count(const char *name)
{
    RC_STRINGLIST *list = rc_services_in_runlevel(name);
    int len = rc_stringlist_len(list);
    rc_stringlist_free(list);

    return len;
}

int rc_plugin_hook(RC_HOOK hook, const char *name)
{
    int ret = 0;
    char* runlevel = rc_runlevel_get();
    const char* defaultlevel = getenv("RC_DEFAULTLEVEL");

    /* Don't do anything if we're not booting or shutting down. */
    if(!(rc_runlevel_starting() || rc_runlevel_stopping())) {
        switch(hook) {
        case RC_HOOK_RUNLEVEL_STOP_IN:
        case RC_HOOK_RUNLEVEL_STOP_OUT:
        case RC_HOOK_RUNLEVEL_START_IN:
        case RC_HOOK_RUNLEVEL_START_OUT:
            /* Switching runlevels, so we're booting or shutting down.*/
            break;
        default:
            DBG("Not booting or shutting down");
            goto exit;
        }
    }

    switch(hook) {
    case RC_HOOK_RUNLEVEL_STOP_IN:
        if(strcmp(name, RC_LEVEL_SHUTDOWN) == 0) {
            DBG("Shutting down");
            if (psplash_start() != 0) {
                ret = -1;
            }
            psplash_write_msg("Shutting down...", NULL);
        }

        break;
    case RC_HOOK_RUNLEVEL_START_IN:
        if(strcmp(name, RC_LEVEL_SYSINIT) == 0) {
            DBG("Booting");
            if (psplash_start() != 0) {
                ret = -1;
            }
            psplash_write_msg("Booting...", NULL);
        }

        break;
    case RC_HOOK_RUNLEVEL_START_OUT:
        if(strcmp(name, defaultlevel) == 0) {
            psplash_quit();
        }
        break;
    case RC_HOOK_SERVICE_STOP_IN:
        if(strcmp(name, "mount-ro") == 0 &&
                strcmp(runlevel, RC_LEVEL_SHUTDOWN) == 0) {
            psplash_quit();
        }
        break;
    case RC_HOOK_SERVICE_STOP_NOW:
        psplash_write_msg("Stopping service", name);
        // update_progress(runlevel, false);
        break;
    case RC_HOOK_SERVICE_START_NOW:
        psplash_write_msg("Starting service", name);
        // update_progress(runlevel, true);
        break;
    default:
        break;
    }

exit:
    free(runlevel);
    return ret;
}
