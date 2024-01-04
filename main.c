#include <stdio.h>
#include "MemoryTool.h"

int main() {
    // Get the process ID of PUBG Mobile
    pid_t pid = get_pid("com.tencent.ig");

    if (pid == -1) {
        printf("Error: PUBG Mobile process not found.\n");
        return 1;
    }

    // Get the base address of libUE4.so module
    unsigned long long base_address = get_module_address(pid, "libUE4.so");

    if (base_address == 0) {
        printf("Error: libUE4.so module not found in PUBG Mobile process.\n");
        return 1;
    }

    // Modify memory to change the normal view into iPad view
    float new_value = 1.8532798;
    off_t offset = 0x57A5CB6;

    ssize_t write_result = write_memory(pid, base_address + offset, &new_value, sizeof(float));

    if (write_result == -1) {
        printf("Error: Memory modification failed.\n");
        return 1;
    }

    printf("Memory modification successful. PUBG Mobile view changed to iPad view!\n");

    return 0;
}
