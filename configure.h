#define DEV_DISK_BY_LABEL   "/dev/disk/by-label/"
#define CONF_FILE           "/etc/altab"
#define FS_OPTS             "fsopts"
#define MOUNT_DEFAULTS      "defaults"

typedef struct fs_opts {
	char *fsType;
	char *mountType;
	char *options;
	void *next;
} fs_opts;

typedef struct mount_opts {
	char *dir;
	char *peerDir;
	char *fsType;
	char *options;
	int wd;
	void *next;
} mount_opts;

char *fixOptions(char *options);
mount_opts *addMountDef(mount_opts *mountOpts, char *dir, char *peerDir, char *fsType, char *options);
fs_opts *addFSDefault(fs_opts *fsOpts, char *fsType, char *mountType, char *options);
int configure(fs_opts **fsOpts, mount_opts **mountOpts);
