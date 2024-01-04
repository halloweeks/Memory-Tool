# Android Memory Tool

This is a C-based memory tool for Android that allows you to read and write memory in a specified process. It leverages the `process_vm_readv` and `process_vm_writev` system calls for memory operations.

## Features

- **read_memory:** Read memory from a specified process.
- **write_memory:** Write data to a specified memory address in a process.
- **get_pid:** Get the process ID based on the process name.
- **get_module_address:** Get the base address of a specific module in the process's memory.

## Usage

1. **Include the tool:**
   ```c
   #include "MemoryTool.h"
   ```

2. **Read Memory:**
   ```c
   ssize_t read_result = read_memory(pid, address, buffer, size);
   if (read_result == -1) {
       printf("Error: Failed to read memory.\n");
       return 1;
   }
   ```

3. **Write Memory:**
   ```c
   ssize_t write_result = write_memory(pid, address, data, size);
   if (write_result == -1) {
       printf("Error: Failed to write memory.\n");
       return 1;
   }
   ```

4. **Get Process ID:**
   ```c
   pid_t process_id = get_pid("target_process_name");
   if (process_id == -1) {
       printf("Error: Process ID not found.\n");
       return 1;
   }
   ```

5. **Get Module Address:**
   ```c
   unsigned long long module_address = get_module_address(process_id, "target_module_name");
   if (module_address == 0) {
       printf("Error: Module address not found.\n");
       return 1;
   }
   ```

## Example Code Snippet

To use this tool, create a file named `main.c` and include the `MemoryTool.h` header:

```c
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
    float new_value = 240.0;
    off_t offset = 0x3EF90E4;

    ssize_t write_result = write_memory(pid, base_address + offset, &new_value, sizeof(float));

    if (write_result == -1) {
        printf("Error: Memory modification failed.\n");
        return 1;
    }

    printf("Memory modification successful. PUBG Mobile view changed to iPad view!\n");

    return 0;
}
```

## How to Build

Compile the tool using your preferred compiler, for example:
```bash
cc main.c -o main -Wall
```

## License

This project is licensed under the [MIT License](LICENSE).

## Contribution

Feel free to contribute by opening issues or creating pull requests.
