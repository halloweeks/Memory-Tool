#ifndef __ANDROID_MEMORY_TOOL__
#define __ANDROID_MEMORY_TOOL__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <dirent.h>

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

ssize_t read_memory(pid_t pid, uintptr_t address, void *value, size_t size) {
	struct iovec local[1];
	struct iovec remote[1];
	local[0].iov_base = value;
	local[0].iov_len = size;
	remote[0].iov_base = (void*)address;
	remote[0].iov_len = size;
	return process_vm_readv(pid, local, 1, remote, 1, 0);
}

ssize_t write_memory(pid_t pid, uintptr_t address, const void *value, size_t size) {
	struct iovec local[1];
	struct iovec remote[1];
	local[0].iov_base = (void *)value;
	local[0].iov_len = size;
	remote[0].iov_base = (void *)address;
	remote[0].iov_len = size;
	return process_vm_writev(pid, local, 1, remote, 1, 0);
}

pid_t get_pid(const char *process_name) {
	DIR *dir = opendir("/proc");
	struct dirent *entry = NULL;
	char cmdline_path[256];
	char cmdline[256];
	int fd;
	
	if (dir == NULL) {
		return -1;
	}
	
	while ((entry = readdir(dir)) != NULL) {
		if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0) || (entry->d_type != DT_DIR) || (strspn(entry->d_name, "0123456789") != strlen(entry->d_name))) {
			continue;
		}
		snprintf(cmdline_path, sizeof(cmdline_path), "/proc/%s/cmdline", entry->d_name);
		fd = open(cmdline_path, O_RDONLY);
		read(fd, cmdline, 256);
		close(fd);
		if (strncmp(cmdline, process_name, strlen(process_name)) == 0) {
			closedir(dir);
			return atoi(entry->d_name);
		}
	}
	closedir(dir);
	return -1;
}

unsigned long long get_module_address(pid_t process_id, const char *module_name) {
	char filename[256];
	char line[1024];
	FILE *fp = NULL;
	unsigned long long address;
	
	snprintf(filename, sizeof(filename), "/proc/%d/maps", process_id);
	
	if (!(fp = fopen(filename, "r"))) {
		return 0;
	}
	while (fgets(line, sizeof(line), fp)) {
		if (strstr(line, module_name)) {
			if (sscanf(line, "%llx", &address) == 1) {
				break;
			}
		}
	}
	fclose(fp);
	return address;
}

#endif