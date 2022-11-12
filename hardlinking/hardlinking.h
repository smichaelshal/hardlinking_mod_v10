#ifndef HARDLINKING_H_
#define HARDLINKING_H_

#include "namei_utils.h"

int link_dir(const char *oldname, const char *newname);
int unlink_dir(const char *name);

#endif /* HARDLINKING_H_ */