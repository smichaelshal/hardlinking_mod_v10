#ifndef NAMEI_UTILS_H_
#define NAMEI_UTILS_H_

#include <linux/namei.h>

inline int may_create(struct user_namespace *mnt_userns,
			     struct inode *dir, struct dentry *child);

int may_delete(struct user_namespace *mnt_userns, struct inode *dir,
		      struct dentry *victim, bool isdir);

#endif /* NAMEI_UTILS_H_ */