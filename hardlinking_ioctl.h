#include <asm/ioctl.h>
#include <linux/limits.h>

#define IOCTL_BASE 'm' + 0x58
#define IOCTL_LINK_NR 0x67
#define IOCTL_UNLINK_NR 0x68


struct ioctl_link {
    char src_path[PATH_MAX];
    char dst_path[PATH_MAX];
};

struct ioctl_unlink {
    char path[PATH_MAX];
};

#define IOCTL_LINK _IOW(IOCTL_BASE, IOCTL_LINK_NR, struct ioctl_link)
#define IOCTL_UNLINK _IOW(IOCTL_BASE, IOCTL_UNLINK_NR, struct ioctl_unlink)
