#include <stdio.h>
#include <dirent.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <map>
#include <string>
#include <unistd.h>
#include <stdlib.h>

typedef std::map<DIR *, error_t> dirpToError_t;
static dirpToError_t dirpToError;

typedef std::map<int, int> fdToError_t;
static fdToError_t fdToError;

struct dirent *readdir(DIR *dirp) {
	dirpToError_t::const_iterator found = dirpToError.find(dirp);
	if (found != dirpToError.end()) {
		errno = found->second;
		return NULL;
	}

	typedef struct dirent *func_t(DIR *dirp);
	func_t *func = (func_t *)dlsym(RTLD_NEXT, "readdir");
	return (*func)(dirp);
}

DIR *opendir(const char *name) {
	typedef DIR *func_t(const char *name);
	func_t *func = (func_t *)dlsym(RTLD_NEXT, "opendir");
	DIR *result = (*func)(name);

	char *fullPath = realpath(name, NULL);
	if (fullPath != NULL) {
		char *envValue = getenv("BROKEN");
		if (envValue != NULL) {
			if (strcmp(fullPath, envValue) == 0) {
				dirpToError[result] = EIO;
			}
		}
		free(fullPath);
	}

	return result;
}

int closedir(DIR *dirp) {
	dirpToError.erase(dirp);

	typedef int func_t(DIR *dirp);
	func_t *func = (func_t *)dlsym(RTLD_NEXT, "closedir");
	return (*func)(dirp);
}

DIR *fdopendir(int fd) {
	typedef DIR *func_t(int fd);
	func_t *func = (func_t *)dlsym(RTLD_NEXT, "fdopendir");
	DIR *result = (*func)(fd);

	fdToError_t::const_iterator found = fdToError.find(fd);
	if (found != fdToError.end()) {
		dirpToError[result] = found->second;
	}

	return result;
}

extern "C" int open(const char *pathname, int flags, mode_t mode) {
	typedef int func_t(const char *pathname, int flags, mode_t mode);
	func_t *func = (func_t *)dlsym(RTLD_NEXT, "open");
	int result = (*func)(pathname, flags, mode);

	char *fullPath = realpath(pathname, NULL);
	if (fullPath != NULL) {
		char *envValue = getenv("BROKEN");
		if (envValue != NULL) {
			if (strcmp(fullPath, envValue) == 0) {
				fdToError[result] = EIO;
			}
		}
		free(fullPath);
	}

	return result;
}

int close(int fd) {
	fdToError.erase(fd);

	typedef int func_t(int fd);
	func_t *func = (func_t *)dlsym(RTLD_NEXT, "close");
	return (*func)(fd);
}

