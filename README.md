# "Developing Modules in the Linux Kernel" labs

This repository contains a set of Linux kernel modules created for a laboratory course on Linux Kernel Development. These modules explore key concepts and functionalities of kernel programming, providing hands-on experience with Linux kernel internals.

## Modules Overview

### 1. Basic Module Initialization and Cleanup

- Demonstrates the simplest kernel module with `init_module` and `cleanup_module`.

### 2. Interfacing with Custom Functions

- Integrates custom functions via header files, showcasing modular development.

### 3. Random Number Array Sum Checker

- Generates random arrays in the kernel space, calculates their sums, and compares them using a helper function.

### 4. Dynamic Memory Management

- Allocates memory dynamically for arrays and structures, and safely deallocates them using helper functions.

### 5. Sysfs Interface

- Creates a custom sysfs interface for interacting with kernel objects and exposing variables to userspace.

### 6. Parameterized Modules

- Implements kernel module parameters, allowing configuration of module behavior via command-line arguments.

### 7. Character Device Driver

- Implements a basic character device driver for reading and writing data from/to userspace.

### 8. Advanced Character Device Driver with Sessions

- Extends the character device implementation to support session management for multiple users.

## Getting Started

### Prerequisites

- A Linux system with kernel headers installed.
- A working knowledge of Linux kernel module development.

### Compilation

Use the following command to compile a module:

```bash
make
```

### Loading and Unloading Modules

- insert a module into the kernel:

```bash
sudo insmod <module_name>.ko
```

- remove a module from the kernel:

```bash
sudo rmmod <module_name>
```

### Logs

Use `dmesg` to view kernel logs:

```bash
dmesg | tail
```

## License

This project is developed for educational purposes. No explicit license is applied.
