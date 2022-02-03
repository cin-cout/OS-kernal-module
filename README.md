# OS-kernal-module

Write a kernel module to get following basic information and build a proc file in ```/proc/my_info```

* Version: Linux version
* CPU:
  * processor, model name, physical id, core id, cpu cores, cache size, clflush size, cache_alignment, address sizes
* Memory:
  * Memtotal, Memfree, Buffers, Activate, Inactivate, Shmem, Dirty, Writeback, KernelStack, PageTables
* Time:
  * Uptime, Idletime

We also have an interface to show these info, which only get data from proc file build by self.

This assignment was done on linux 4.15.0

## Setup
1. Build and install module
```
make
```
2. Delete other file except source code
```
make clean
```
3. Compile module
```
make mod
```
4. Compile app
```
make app
```
5. Install module
```
make ins_mod
```
6. Remove module
```
make rm_mod
```
7. Delete app
```
make del_app
```
8. Delete module
```
make del_mod
```
## Usage
* Execute app
```
./app
```
```
Which information do you want?Which information do you want?
Version(v),CPU(c\),Memory(m),Time(t),All(a),Exit(e)?
v

Version: Linux version 4.15.0-72-generic
------------------------------------------------------------
Which information do you want?
Version(v),CPU(c\),Memory(m),Time(t),All(a),Exit(e)?
a


=============Version=============
Linux version 4.15.0-72-generic

=============CPU=============
processor		: 0
model name		: Intel(R) Core(TM) i7-4510U CPU @ 2.00GHz
physical id		: 0
core id			: 0
cpu cores		: 2
cache size		: 4096 KB
clflush size	: 64
cache_alignment		: 64
address sizes		: 39 bits physical, 48 bits virtual

processor		: 1
model name		: Intel(R) Core(TM) i7-4510U CPU @ 2.00GHz
physical id		: 0
core id			: 1
cpu cores		: 2
cache size		: 4096 KB
clflush size	: 64
cache_alignment		: 64
address sizes		: 39 bits physical, 48 bits virtual


=============Memory=============
MemTotal		:  1008948 kB
MemFree			:   335496 kB
Buffers			:    24048 kB
Active			:   332384 kB
Inactive		:   192888 kB
Shmem			:    15732 kB
Dirty			:      532 kB
Writeback		:        0 kB
KernelStack		:     7136 kB
PageTables		:    31112 kB

=============Time=============
Uptime			: 37597.24(s)
Idletime		: 68581.15(s)
------------------------------------------------------------
Which information do you want?
Version(v),CPU(c\),Memory(m),Time(t),All(a),Exit(e)?
e
```
