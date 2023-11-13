#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/uio.h>

#ifndef process_vm_readv
	#include <sys/syscall.h>
	#include <asm/unistd.h>
	
	ssize_t process_vm_readv(pid_t pid, const struct iovec *local_iov, unsigned long liovcnt, const struct iovec *remote_iov, unsigned long riovcnt, unsigned long flags) {
		return syscall(__NR_process_vm_readv, pid, local_iov, liovcnt, remote_iov, riovcnt, flags);
	}
#endif

#ifndef process_vm_writev
	#include <sys/syscall.h>
	#include <asm/unistd.h>
	
	ssize_t process_vm_writev(pid_t pid, const struct iovec *local_iov, unsigned long liovcnt, const struct iovec *remote_iov, unsigned long riovcnt, unsigned long flags) {
		return syscall(__NR_process_vm_writev, pid, local_iov, liovcnt, remote_iov, riovcnt, flags);
	}
#endif

int GetProcessId(const char *PackageName) {
	DIR *dir = NULL;
	struct dirent *ptr = NULL;
	FILE *fp = NULL;
	char filepath[256];
	char filetext[128];
	dir = opendir("/proc");
	if (NULL != dir) {
		while ((ptr = readdir(dir)) != NULL) {
			if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0)) {
				continue;
			}
			if (ptr->d_type != DT_DIR) {
				continue;
			}
			sprintf(filepath, "/proc/%s/cmdline", ptr->d_name);
			fp = fopen(filepath, "r");
			if (NULL != fp) {
				fgets(filetext, sizeof(filetext), fp);
				if (strcmp(filetext, PackageName) == 0) {
					break;
				}
				fclose(fp);
			}
		}
	}
	if (readdir(dir) == NULL) {
		return 0;
	}
	closedir(dir);
	return atoi(ptr->d_name);
}

unsigned long long GetModuleBase(int ProcessId, const char *ModuleName) {
	
	long addr = 0;
	char *pch;
	char filename[32];
	char line[1024];
	snprintf(filename, sizeof(filename), "/proc/%d/maps", ProcessId);
	FILE *fp = fopen(filename, "r");
	
	unsigned long long start;
	unsigned long long end;
	char permission[5];
	char is_readable;
	char is_writable;
	// char flag;
	
	if (fp != NULL) {
		while (fgets(line, sizeof(line), fp)) {
			if (strstr(line, ModuleName)) {
				if (sscanf(line, "%llx-%llx %4s", &start, &end, permission) == 3) {
					printf("permission: %s\n", permission);
					/*
					if (
					if (is_writable == 'w') {
						addr = start;
						break;
					}*/
				} else {
					continue;
				}
			}
			
			/*
			if (strstr(line, ModuleName)) {
				pch = strtok(line, "-");
				addr = strtoul(pch, NULL, 16);
				break;
			}*/
		}
		fclose(fp);
	}
	return addr;
}

ssize_t ProcessRead(pid_t pid, uintptr_t address, void *value, size_t size) {
	struct iovec local[1];
	struct iovec remote[1];
	local[0].iov_base = value;
	local[0].iov_len = size;
	remote[0].iov_base = (void*)address;
	remote[0].iov_len = size;
	return process_vm_readv(pid, local, 1, remote, 1, 0);
}

ssize_t ProcessWrite(pid_t pid, uintptr_t address, const void *value, size_t size) {
	struct iovec local[1];
	struct iovec remote[1];
	local[0].iov_base = (void *)value;
	local[0].iov_len = size;
	remote[0].iov_base = (void *)address;
	remote[0].iov_len = size;
	return process_vm_writev(pid, local, 1, remote, 1, 0);
}