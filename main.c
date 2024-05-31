#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define MAX_BUFFER 1024
#define SYSFS_FILE_A_IN "/proc/proj4dmibar/rejA"
#define SYSFS_FILE_S_OUT "/proc/proj4dmibar/rejS"
#define SYSFS_FILE_W_OUT "/proc/proj4dmibar/rejW"

void print_header() {
    printf("             _                         __          \n");
    printf("   ___  ____(_)_ _  ___   ___ ___ ___ / /_____ ____\n");
    printf("  / _ \\/ __/ /  ' \\/ -_) (_-</ -_) -_)  '_/ -_) __/\n");
    printf(" / .__/_/ /_/_/_/_/\\__/ /___/\\__/\\__/_/\\_\\\\__/_/   \n");
    printf("/_/                                                \n");
    printf("This program tests the prime number calculation kernel module.\n");
    printf("It retrieves prime numbers based on user input or pre-defined tests.\n");
    printf("Calculations are performed using the GPIO emulator hardware module.\n\n");}

void write_to_file(const char *file_path, const char *data) {
    int fd = open(file_path, O_WRONLY);
    if (fd < 0) {
        printf("Open %s – error: %d\n", file_path, errno);
        exit(1);
    }

    lseek(fd, 0L, SEEK_SET);
    int n = write(fd, data, strlen(data));
    if (n != strlen(data)) {
        printf("Write to %s – error: %d\n", file_path, errno);
        close(fd);
        exit(2);
    }

    close(fd);
}

void read_from_file(const char *file_path, char *buffer) {
    int fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        printf("Open %s – error: %d\n", file_path, errno);
        exit(1);
    }

    lseek(fd, 0L, SEEK_SET);
    int n = read(fd, buffer, MAX_BUFFER);
    if (n > 0) {
        buffer[n] = '\0';
    } else {
        printf("Read from %s – error: %d\n", file_path, errno);
        buffer[0] = '\0';
    }

    close(fd);
}

int get_prime(int n) {
    char buffer[MAX_BUFFER];

    snprintf(buffer, MAX_BUFFER, "%d", n);
    write_to_file(SYSFS_FILE_A_IN, buffer);

    int status = 1;
    while (status != 2) {
        read_from_file(SYSFS_FILE_S_OUT, buffer);
        status = atoi(buffer);
        sleep(1);
    }

    read_from_file(SYSFS_FILE_W_OUT, buffer);
    return atoi(buffer);
}

void test_prime_number_module() {
    int result;

    printf("--------------------------------------------------\n");
    printf("------------ TEST 1: 5th prime number ------------\n");
    result = get_prime(5);
    if (result != 11) {
        printf("Test 1 failed: Expected 11, got %d\n", result);
    } else {
        printf("Test 1 passed: Correctly found 5th prime number %d\n", result);
    }
    printf("\n");

    printf("--------------------------------------------------\n");
    printf("------------ TEST 2: 1st prime number ------------\n");
    result = get_prime(1);
    if (result != 2) {
        printf("Test 2 failed: Expected 2, got %d\n", result);
    } else {
        printf("Test 2 passed: Correctly found 1st prime number %d\n", result);
    }
    printf("\n");

    printf("--------------------------------------------------\n");
    printf("----------- TEST 3: 10th prime number ------------\n");
    result = get_prime(10);
    if (result != 29) {
        printf("Test 3 failed: Expected 29, got %d\n", result);
    } else {
        printf("Test 3 passed: Correctly found 10th prime number %d\n", result);
    }
    printf("\n");

    printf("--------------------------------------------------\n");
    printf("---------- TEST 4: 1000th prime number -----------\n");
    result = get_prime(1000);
    if (result != 7919) {
        printf("Test 4 failed: Expected 7919, got %d\n", result);
    } else {
        printf("Test 4 passed: Correctly found 1000th prime number %d\n", result);
    }
    printf("--------------------------------------------------\n\n");
}

void user_input_loop() {
    char input[MAX_BUFFER];
    int number;

    while (1) {
        printf("Enter a number to find the corresponding prime or 'x' to exit: ");
        fgets(input, MAX_BUFFER, stdin);

        if (input[0] == 'x' || input[0] == 'X') {
            printf("Exiting.\n");
            break;
        }

        number = atoi(input);
        if (number < 1 || number > 1000) {
            printf("Invalid input. Please enter a number between 0 and 1000.\n");
            continue;
        }

        int result = get_prime(number);
        printf("The %d-th prime number is %d\n\n", number, result);
    }
}

int main(void) {
    print_header();
    test_prime_number_module();
    user_input_loop();
    return 0;
}
