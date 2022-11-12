#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "../hardlinking_ioctl.h"

#define DEVICE_PATH	"/dev/ioctl_hardlinking_dev"

int link_dir(int fd, const char *src_path, const char *dst_path){
    struct ioctl_link *link_dir_paths = (struct ioctl_link*)malloc(sizeof(struct ioctl_link));
    memcpy(link_dir_paths->src_path, src_path, strlen(src_path));
    memcpy(link_dir_paths->dst_path, dst_path, strlen(dst_path));
    return ioctl(fd, IOCTL_LINK, link_dir_paths);
}

int unlink_dir(int fd, const char *path){
    struct ioctl_unlink *unlink_dir_path = (struct ioctl_unlink*)malloc(sizeof(struct ioctl_unlink));
    memcpy(unlink_dir_path->path, path, strlen(path));
    return ioctl(fd, IOCTL_UNLINK, unlink_dir_path);
}


int main(int argc, char **argv)
{
    int fd, err;
    fd = open(DEVICE_PATH, O_RDONLY);
   
    if (fd < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}

    if(argc == 2){
        err = unlink_dir(fd, argv[1]);
    }
    else if(argc == 3){
        err = link_dir(fd, argv[1], argv[2]);
    }else{
        printf("error args\n");
        return -1;
    }

    close(fd);
    return err;
}

// ./hardlinking_user /root/td1 /root/td2