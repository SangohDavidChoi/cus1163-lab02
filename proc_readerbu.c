#include "proc_reader.h"

int list_process_directories(void) {
    //Open the /proc directory using opendir()
    DIR *dir = opendir("/proc");
    //Check if opendir() failed and print error message
    if (!dir) {
      perror("opendir failed!!");
      return -1;
    }

    //Declare a struct dirent pointer for directory entries
    struct dirent *entry;
    //Initialize process counter to 0
    int count = 0;
  
    //This is printing header that is given to me
    printf("Process directories in /proc:\n");
    printf("%-8s %-20s\n", "PID", "Type");
    printf("%-8s %-20s\n", "---", "----");

    //Read directory entries using readdir() in a loop
    while ((entry = readdir(dir)) != NULL ){
      //For each entry, check if the name is a number using is_number()
      if (is_number (entry -> d_name)) {
        //If it's a number, print it as a PID and increment counter
        printf ("%-8s %-20s\n", entry -> d_name, "process");
        count++;
      }
    }

    //Close the directory using closedir()
    closedir(dir);
    //Check if closedir() failed
    if (closedir(dir) == -1) {
      perror("closedir");
      return -1;
    }
    
    //Print the total count of process directories found
    printf ("found %d process directories!\n", count);
    return 0; // Replace with proper error handling
}

int read_process_info(const char* pid) {
    char filepath[256];

    //Create the path to /proc/[pid]/status using snprintf()
    snprintf(filepath, sizeof(filepath), "/proc/%s/status", pid);

    printf("\n--- Process Information for PID %s ---\n", pid);

    //Call read_file_with_syscalls() to read the status file
    //Check if the function succeeded
    if (read_file_with_syscalls(filepath) == -1) {
      fprintf(stderr, "Can't read %s\n", filepath);
      return -1;
    }

    //Create the path to /proc/[pid]/cmdline using snprintf()
    snprintf(filepath, sizeof(filepath), "/proc/%s/cmdline", pid);

    printf("\n--- Command Line ---\n");

    //Call read_file_with_syscalls() to read the cmdline file
    //Check if the function succeeded
    if (read_file_with_syscalls(filepath) == -1) {
      fprintf(stderr, "Can't read %s\n", filepath);
      return -1;
    }
    
    printf("\n"); // Add extra newline for readability

    return 0; // Replace with proper error handling
}

int show_system_info(void) {
    int line_count = 0;
    const int MAX_LINES = 10;

    printf("\n--- CPU Information (first %d lines) ---\n", MAX_LINES);

    //Open /proc/cpuinfo using fopen() with "r" mode
    //Check if fopen() failed
    FILE *file;
    file = fopen("/proc/cpuinfo", "r");
    if (!file) {
      perror("fopen failed");
      return -1;
    }

    //Declare a char array for reading lines
    char line[256];
    line_count = 0;
    //Read lines using fgets() in a loop, limit to MAX_LINES
    //Print each line
    while (line_count < MAX_LINES && fgets(line, sizeof(line), file) != NULL) {
      printf("%s", line);
      line_count++;
    }
    //Close the file using fclose()
    fclose(file);

    printf("\n--- Memory Information (first %d lines) ---\n", MAX_LINES);

    //Open /proc/meminfo using fopen() with "r" mode
    //Check if fopen() failed
    file = fopen("/proc/meminfo", "r");
    if (!file) {
      perror("fopen failed");
      return -1;
    }

    //Read lines using fgets() in a loop, limit to MAX_LINES
    //Print each line
    while (line_count < MAX_LINES && fgets(line, sizeof(line), file) != NULL) {
      printf("%s", line);
      line_count++;
    }
    //Close the file using fclose()
    fclose(file);

    return 0; // Replace with proper error handling
}

void compare_file_methods(void) {
    const char* test_file = "/proc/version";

    printf("Comparing file reading methods for: %s\n\n", test_file);

    printf("=== Method 1: Using System Calls ===\n");
    read_file_with_syscalls(test_file);

    printf("\n=== Method 2: Using Library Functions ===\n");
    read_file_with_library(test_file);

    printf("\nNOTE: Run this program with strace to see the difference!\n");
    printf("Example: strace -e trace=openat,read,write,close ./lab2\n");
}

int read_file_with_syscalls(const char* filename) {
    //Declare variables: file descriptor (int), buffer (char array), bytes_read (ssize_t)
    char buffer[1024];
    ssize_t bytes_read;
    
    //Open the file using open() with O_RDONLY flag
    int fd = open (filename, O_RDONLY);

    //Check if open() failed (fd == -1) and return -1
    if (fd == -1) {
      perror("open failed");
      return -1;
    }

    //Read the file in a loop using read()
    //Use sizeof(buffer) - 1 for buffer size to leave space for null terminator
    //Check if read() returns > 0 (data was read)
    //Null-terminate the buffer after each read
    //Print each chunk of data read
    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
      buffer[bytes_read] = '\0';
      printf("%s", buffer);
    }

    //Handle read() errors (return value -1)
    //If read() fails, close the file and return -1
    //Close the file using close()
    if (bytes_read == -1) {
      perror ("read error");
      close (fd);
      return -1;
    }
    
    //Check if close() failed
    if (close(fd) == -1) {
      perror("close error");
      return -1;
    }

    return 0; // Replace with proper error handling
}

int read_file_with_library(const char* filename) {
    //Declare variables: FILE pointer, buffer (char array)
    FILE *file;
    char line[256];

    //Open the file using fopen() with "r" mode
    file = fopen(filename, "r");

    //Check if fopen() failed and return -1
    if (!file) {
      perror ("file failed open");
      return -1;
    }

    //Read the file using fgets() in a loop
    //Continue until fgets() returns NULL
    //Print each line read
    while (fgets(line, sizeof(line), file) != NULL) {
      printf("%s", line);
    }

    //Close the file using fclose()
    fclose(file);
    //Check if fclose() failed
      if (file) {
      fclose(file);
    }

    return 0; // Replace with proper error handling
}

int is_number(const char* str) {
    //Handle empty strings - check if str is NULL or empty
    //Return 0 for empty strings
    if (!str || *str == '\0') {
      return 0;
    }
    
    //Check if the string contains only digits
    //Loop through each character using a while loop
    //Use isdigit() function to check each character
    //If any character is not a digit, return 0
    //Return 1 if all characters are digits
    while (*str) {
      if (!isdigit((unsigned char)*str)) {
        return 0;
        }
      return 1;
    }
    return 0; // Replace with actual implementation
}
