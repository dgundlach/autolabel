#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <libgen.h>
#include <stdio.h>
#include <string.h>
#include <mntent.h>
#include <blkid/blkid.h>

#include "configure.h"
#include "remount.h"

// aaa

int main(int argc, char **argv, char **env) {

  char *label;
  char *byLabelPath;
  char *devPath;
  const char *fsType;
  char *mountFSType;
  fs_opts *fsOpts = NULL;
  mount_opts *mountOpts = NULL;
  fs_opts *fO = NULL;
  mount_opts *mO = NULL;
  char wd[PATH_MAX];
  char *ro = "";
  char *options;
  char *defaultOpts;
  struct stat st;

  if (argc != 2) exit(1);
  label = argv[1];
  getcwd(wd, PATH_MAX);

  configure(&fsOpts, &mountOpts);

  /*
   * Get the corresponding device for the label.
   */

  byLabelPath = malloc(sizeof(DEV_DISK_BY_LABEL) + 2 + NAME_MAX);
  sprintf(byLabelPath, DEV_DISK_BY_LABEL"%s", label);
  if (!(devPath = realpath(byLabelPath, NULL))) {
    devPath = malloc(NAME_MAX + 6);
    sprintf(devPath, "/dev/%s", label);
  }
  free(byLabelPath);

  if (stat(devPath, &st) == -1) {
    exit(1);
  }

  /*
   * Check if the mount path is a paired one, and if the drive is already mounted read-only.
   * If it's already mounted, remount it read-write.
   */

  mO = mountOpts;
  while (mO) {
    if (!strcmp(wd, mO->dir) && strcmp(mO->peerDir, "*")) {
      ro = ",ro";
      break;
    }
    if (!strcmp(wd, mO->peerDir)) {
      remount(label, mO->dir);
      break;
    }
    mO = mO->next;
  }

  /*
   * Get the mount options. The priority is the entry, the default entries, then defaults.
   */

  if (!(fsType = blkid_get_tag_value(NULL, "TYPE", devPath))) {
    exit(1);
  }

    /*
   * Default options and mount type.
   */

  options = MOUNT_DEFAULTS;
  mountFSType = (char *)fsType;

  /*
   * Mount dir entry.
   */

  if (mO) {
    options = mO->options;
  }

  /*
   * Now get the options from the default entry.  The options will only be changed if
   * options still equals "defaults".  If the entry doesn't exist, just use "defaults".
   */

  fO = fsOpts;
  if (fO && !strcmp(fO->fsType, "*")) {
    defaultOpts = fO->options;
    fO = fO->next;
  } else {
    defaultOpts = MOUNT_DEFAULTS;
  }

  /*
   * Set the mount type and the options if they are still set to "defaults".
   */

  while (fO) {
    if (!strcmp(fO->fsType, fsType)) {
      mountFSType = fO->mountType;
      if (!strcmp(options, MOUNT_DEFAULTS)) {
        options = fO->options;
      }
      break;
    }
    fO = fO->next;
  }

  /*
   * If the options are still set to "defaults", use the options from the default entry.
   */

  if (!strcmp(options, MOUNT_DEFAULTS)) {
    options = defaultOpts;
  }

  /*
   * Send all of this to automount.  Then clean up.
   */

  printf("-fstype=%s,%s%s :%s\n", mountFSType, options, ro, devPath);

  free(devPath);
  free((void *)fsType);
  exit(0);
}
