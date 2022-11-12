#include <linux/namei.h>
#include <linux/fsnotify.h>
#include <linux/audit.h>


inline int may_create(struct user_namespace *mnt_userns,
			     struct inode *dir, struct dentry *child)
{
	audit_inode_child(dir, child, AUDIT_TYPE_CHILD_CREATE);
	if (child->d_inode)
		return -EEXIST;
	if (IS_DEADDIR(dir))
		return -ENOENT;
	if (!fsuidgid_has_mapping(dir->i_sb, mnt_userns))
		return -EOVERFLOW;

	return inode_permission(mnt_userns, dir, MAY_WRITE | MAY_EXEC);
}

int may_delete(struct user_namespace *mnt_userns, struct inode *dir,
		      struct dentry *victim, bool isdir)
{
	struct inode *inode = d_backing_inode(victim);
	int error;

	if (d_is_negative(victim))
		return -ENOENT;
	BUG_ON(!inode);

	BUG_ON(victim->d_parent->d_inode != dir);

	/* Inode writeback is not safe when the uid or gid are invalid. */
	if (!uid_valid(i_uid_into_mnt(mnt_userns, inode)) ||
	    !gid_valid(i_gid_into_mnt(mnt_userns, inode)))
		return -EOVERFLOW;

	audit_inode_child(dir, victim, AUDIT_TYPE_CHILD_DELETE);

	error = inode_permission(mnt_userns, dir, MAY_WRITE | MAY_EXEC);
	if (error)
		return error;
	if (IS_APPEND(dir))
		return -EPERM;

	if (check_sticky(mnt_userns, dir, inode) || IS_APPEND(inode) ||
	    IS_IMMUTABLE(inode) || IS_SWAPFILE(inode) ||
	    HAS_UNMAPPED_ID(mnt_userns, inode))
		return -EPERM;
	if (isdir) {
		if (!d_is_dir(victim))
			return -ENOTDIR;
		if (IS_ROOT(victim))
			return -EBUSY;
	} else if (d_is_dir(victim))
		return -EISDIR;
	if (IS_DEADDIR(dir))
		return -ENOENT;
	if (victim->d_flags & DCACHE_NFSFS_RENAMED)
		return -EBUSY;
	return 0;
}

// int sysctl_protected_hardlinks __read_mostly = 0;

// bool safe_hardlink_source(struct user_namespace *mnt_userns,
// 				 struct inode *inode)
// {
// 	umode_t mode = inode->i_mode;

// 	/* Special files should not get pinned to the filesystem. */
// 	if (!S_ISREG(mode))
// 		return false;

// 	/* Setuid files should not get pinned to the filesystem. */
// 	if (mode & S_ISUID)
// 		return false;

// 	/* Executable setgid files should not get pinned to the filesystem. */
// 	if ((mode & (S_ISGID | S_IXGRP)) == (S_ISGID | S_IXGRP))
// 		return false;

// 	/* Hardlinking to unreadable or unwritable sources is dangerous. */
// 	if (inode_permission(mnt_userns, inode, MAY_READ | MAY_WRITE))
// 		return false;

// 	return true;
// }

// int may_linkat(struct user_namespace *mnt_userns, struct path *link)
// {
// 	struct inode *inode = link->dentry->d_inode;

// 	/* Inode writeback is not safe when the uid or gid are invalid. */
// 	if (!uid_valid(i_uid_into_mnt(mnt_userns, inode)) ||
// 	    !gid_valid(i_gid_into_mnt(mnt_userns, inode)))
// 		return -EOVERFLOW;

// 	if (!sysctl_protected_hardlinks)
// 		return 0;

// 	/* Source inode owner (or CAP_FOWNER) can hardlink all they like,
// 	 * otherwise, it must be a safe source.
// 	 */
// 	if (safe_hardlink_source(mnt_userns, inode) ||
// 	    inode_owner_or_capable(mnt_userns, inode))
// 		return 0;

// 	// audit_log_path_denied(AUDIT_ANOM_LINK, "linkat"); // ???
// 	return -EPERM;
// }

