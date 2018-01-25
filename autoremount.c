#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/inotify.h>
#include <limits.h>
#include <mntent.h>
#include <sys/mount.h>
#include <syslog.h>
#include <libgen.h>
#include <string.h>
#include "daemonize.h"

#include "configure.h"
#include "remount.h"

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

int main(int argc, char *argv[]) {

  int inotifyFd, wd;
  char buf[BUF_LEN] __attribute__ ((aligned(8)));
  ssize_t numRead;
  char *p;
  struct inotify_event *event;
  int rc;
  int wc = 0;
  fs_opts *fsOpts = NULL;
  mount_opts *mountOpts = NULL;
  mount_opts *mO = NULL;

  daemonize(basename(argv[0]), "root", LOG_DAEMON, &daemonize_signal_handler);
  configure(&fsOpts, &mountOpts);

  inotifyFd = inotify_init();
  if (inotifyFd == -1) {
    syslog(LOG_ERR, "Error initializing inotify.");
    daemon_clean_up();
  }

  mO = mountOpts;
  while (mO) {
    syslog(LOG_INFO, "Dir: %s Peer: %s", mO->dir, mO->peerDir);
    if (strcmp(mO->peerDir, "*")) {
      wc++;
      syslog(LOG_INFO, "Adding watch for %s.", mO->peerDir);
      wd = inotify_add_watch(inotifyFd, mO->peerDir, IN_DELETE|IN_UNMOUNT|IN_ISDIR);
      if (wd == -1) {
        syslog(LOG_ERR, "Error adding inotify watch for %s.", mO->peerDir);
        daemon_clean_up();
      }
      mO->wd = wd;
    }
    mO = mO->next;
  }

  for (;;) {
    numRead = read(inotifyFd, buf, BUF_LEN);
    if (numRead == 0) {
      syslog(LOG_ERR, "Read from inotify fd returned 0.");
      daemon_clean_up();
    }

    if (numRead == -1) {
      syslog(LOG_ERR, "Inotify read error.");
      daemon_clean_up();
    }

    /* Process all of the events in buffer returned by read() */

    for (p = buf; p < buf + numRead; ) {
      event = (struct inotify_event *) p;
      mO = mountOpts;
      while (mO) {
        if (event->wd == mO->wd) {
          if (event->mask & IN_UNMOUNT) {
            wc--;
            if (!wc) {
              syslog(LOG_INFO, "Last watch removed.");
              daemon_clean_up();
            }
          } else if (event->mask & IN_DELETE) {
            syslog(LOG_INFO, "Toggling write status: %s/%s", mO->dir, event->name);
            rc = remount(event->name, mO->dir);
          }
          break;
        }
        mO = mO->next;
      }
      p += sizeof(struct inotify_event) + event->len;
    }
  }
  daemon_clean_up();
}
