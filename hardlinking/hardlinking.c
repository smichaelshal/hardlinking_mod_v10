#include "hardlinking.h"
#include <linux/namei.h>
#include <linux/fsnotify.h>

int vfs_unlink_dir(struct user_namespace *mnt_userns, struct inode *dir,
	       struct dentry *dentry, struct inode **delegated_inode)
{
	struct inode *target = dentry->d_inode;
	int error;
	
	// error = may_delete(mnt_userns, dir, dentry, 0); // ???
	// if (error)
	// 	return error;

	if (!dir->i_op->unlink)
		return -EPERM;

	inode_lock(target);
	// if (is_local_mountpoint(dentry))
	if (0)
		error = -EBUSY;
	else {
		// error = security_inode_unlink(dir, dentry);
		if (!error) {
			error = try_break_deleg(target, delegated_inode);
			if (error)
				goto out;
			error = dir->i_op->unlink(dir, dentry);
			if (!error) {
				dont_mount(dentry);
				// detach_mounts(dentry);
				fsnotify_unlink(dir, dentry);
			}
		}
	}
out:
	inode_unlock(target);

	if (!error && !(dentry->d_flags & DCACHE_NFSFS_RENAMED)) {
		fsnotify_link_count(target);
		d_delete(dentry);
	}

	return error;
}

int unlink_dir(const char *name){
	struct dentry *dentry, *parent_dentry;
	struct path path; // parent_path
	struct inode *inode = NULL;
	struct user_namespace *mnt_userns;
	struct inode *delegated_inode = NULL;
	int error;

	error = kern_path(name, LOOKUP_DIRECTORY, &path);
	if (error)
		return error;

	inode = dentry->d_inode;
	parent_dentry = path.dentry->d_parent;

	mnt_userns = mnt_user_ns(path.mnt); // parent ???

	// lock mutex parent dir
	inode_lock_nested(parent_dentry->d_inode, I_MUTEX_PARENT);

	error = vfs_unlink_dir(mnt_userns, parent_dentry->d_inode, path.dentry, &delegated_inode);

	// dput(dentry);

	inode_unlock(parent_dentry->d_inode);

	// if (inode)
	// 	iput(inode);
		
	// mnt_drop_write(path.mnt);

	path_put(&path);
	return error;
}

// int unlink_dir2(const char *name)
// {
// 	struct path parent;
// 	struct dentry *dentry;
// 	int err = 0;

// 	dentry = kern_path_locked(name, &parent);
// 	// if (IS_ERR(dentry))
// 	// 	return PTR_ERR(dentry);
// 	// if (d_really_is_positive(dentry)) {
// 	// 	printk("tests: call unlink_dir\n");
// 	// 	// if (d_inode(dentry)->i_private == &thread)
// 	// 	// 	printk("tests: call unlink_dir\n");
// 	// 	// else
// 	// 	// 	err = -EPERM;
// 	// } else {
// 	// 	err = -ENOENT;
// 	// }
// 	dput(dentry);
// 	inode_unlock(d_inode(parent.dentry));
// 	path_put(&parent);
// 	return err;
// }


int vfs_link_dir(struct dentry *old_dentry, struct user_namespace *mnt_userns,
	     struct inode *dir, struct dentry *new_dentry,
	     struct inode **delegated_inode)
{
	struct inode *inode = old_dentry->d_inode;
	unsigned max_links = dir->i_sb->s_max_links;
	int error;
	printk("start vfs_link_dir\n");

	if (!inode){
		printk("error_v1: %d\n", -ENOENT);
		return -ENOENT;
	}

	error = may_create(mnt_userns, dir, new_dentry); // ???
	if (error)
		return error;

	if (dir->i_sb != inode->i_sb){
		printk("error_v2: %d\n", -EXDEV);
		return -EXDEV;
	}

	/*
	 * A link to an append-only or immutable file cannot be created.
	 */
	if (IS_APPEND(inode) || IS_IMMUTABLE(inode)){
		printk("error_v3: %d\n", -EPERM);
		return -EPERM;
	}
	
	if (HAS_UNMAPPED_ID(mnt_userns, inode)){
		printk("error_v4: %d\n", -EPERM);
		return -EPERM;
	}
	
	if (!dir->i_op->link){
		printk("error_v5: %d\n", -EPERM);
		return -EPERM;
	}
	
	// if (S_ISDIR(inode->i_mode))
	// 	return -EPERM;

	// error = security_inode_link(old_dentry, dir, new_dentry); // ???
	// if (error)
	// 	return error;

	inode_lock(inode);
	if (inode->i_nlink == 0 && !(inode->i_state & I_LINKABLE)){
		error =  -ENOENT;
		printk("error_v6: %d\n", error);
	}
	else if (max_links && inode->i_nlink >= max_links){
		error = -EMLINK;
		printk("error_v7: %d\n", error);
	}
	else {
		error = try_break_deleg(inode, delegated_inode);
		if (!error){
			printk("error_v8: %d\n", error);
			error = dir->i_op->link(old_dentry, dir, new_dentry);
			printk("error_v8_2: %d\n", error);
		}
	}

	if (!error && (inode->i_state & I_LINKABLE)) {
		spin_lock(&inode->i_lock);
		inode->i_state &= ~I_LINKABLE;
		spin_unlock(&inode->i_lock);
		printk("error_v9: %d\n", error);
	}
	inode_unlock(inode);
	if (!error)
		fsnotify_link(dir, inode, new_dentry); // ???
	return error;
}

int link_dir(const char *oldname, const char *newname)
{
	struct dentry *new_dentry;
	struct path old_path, new_path;
	struct user_namespace *mnt_userns;
	int error;
	
	printk("start: link_dir\n");
	// printk("current_uid().val: %d\n", current_uid().val);
	// current_uid().val
	// kern_path_locked ???

	error = kern_path(oldname, 0, &old_path);
	if (error)
	{
		printk("error_v10: %d\n", error);
		return error;
	}

	new_dentry = kern_path_create(AT_FDCWD, newname, &new_path, LOOKUP_DIRECTORY);
	error = PTR_ERR(new_dentry);
	if (IS_ERR(new_dentry)){
		printk("error_v11: %d\n", error);
		goto out;
	}

	error = -EXDEV;
	if (old_path.mnt != new_path.mnt)
	{
		printk("error_v12: %d\n", error);
		goto out_dput;
	}
	mnt_userns = mnt_user_ns(new_path.mnt);
	// error = may_linkat(mnt_userns, &old_path);
	// if (unlikely(error))
	// 	goto out_dput;
	error = security_path_link(old_path.dentry, &new_path, new_dentry); // ???
	if (error){
		printk("error_v13: %d\n", error);
		goto out_dput;
	}
	error = vfs_link_dir(old_path.dentry, mnt_userns, new_path.dentry->d_inode,
			 new_dentry, NULL);
	printk("error_v14: %d\n", error);
out_dput:
	done_path_create(&new_path, new_dentry);
out:
	path_put(&old_path);
	printk("error_v15: %d\n", error);
	return error;
}