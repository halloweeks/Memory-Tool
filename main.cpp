#include <iostream>
#include "MemoryTools.h"

int main(int argc, char **argv) {
	if (argc != 3) {
        printf("Usage: %s <PID>\n", argv[0]);
        return 1;
    }
    
    pid_t target_pid = (pid_t)atoi(argv[1]);
    // uintptr_t address = (uintptr_t)strtoull(argv[2], NULL, 0);
    
    uintptr_t offset = 162079265520;
    
    uint8_t value[11];
    size_t value_size = 11;
    
    memcpy(value, "data inject", value_size);
    
    unsigned long long Base = GetModuleBase(target_pid, "test", "rw");
    
	if (ProcessWrite(target_pid, Base + offset, value, value_size) > 0) {
		printf("written '%s'\n", (char*)value);
	} else {
		printf("Failed to write memory process\n");
	}
	
	return 0;
}

