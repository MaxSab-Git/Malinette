# Malinette
### A Unit Test program using "*.mali" files (refered as malifiles) as source.
- Mainly made for Linux, but should compile fine with most POSIX compliant systems including, in theory, MacOS.</br></br>
- This program has mainly been tested on the WSL version of Linux Ubuntu and on Windows, other systems may not be supported as claimed.</br></br>
- The Windows port is quite dirty, mostly the WinMakefile because of my lack of practice on Windows CMD, but it should works fine.</br></br>

# How to compile
## Linux (and probaly most POSIX systems)
- Ensure that you have a working C++17 compiler.</br></br>
- Set your compiler of choice by setting COMPILER in Makefile (default "c++" alias clang++).</br></br>
- Run this command in the root of the project:</br></br>
  ```bash
  make
  ```
- Then it should create a folder "bin/" with "Malinette" inside (and a folder "srcs/obj/" with all *.o files).</br></br>

## Windows
- Ensure that you have a working C++17 compiler.</br></br>
- Set your compiler of choice by setting COMPILER in WinMakefile (default "clang++").</br></br>
- Run this command in the root of the project:</br></br>
  ```bash
  make -f WinMakefile
  ```
- Then it should create a folder "bin/" with "Malinette.exe" inside (and a lot of *.o file in the "srcs/" folder).</br></br>

# Launch Malinette
- After compilation, you can easily test Malinette with:</br></br>
  ```bash
  make launch
  ```
  After launch, it should generate a "trace.txt" file in the root folder containing the result of all tests done by Malinette in the "tests/" folder using the file "tests/subjects.mali" added with some debug information.</br></br>
- If you see "Final result: KO: error: 127" in "trace.txt" it's likely because "tests/subjects.mali" or wants the use "clang" but don't find it, you can remedy this by changing the compiler used by these files with an another C compiler or by installing "clang" from <a href="https://llvm.org/">the official LLVM website</a> and adding it in your PATH.</br></br>
- To use Malinette, launch the following command:</br></br>
  ```bash
  ./Malinette [Options...] <Malifile>
  ```
  Where Malifile is your "*.mali" file, currently, only one input file is supported, subsequent files will be ignored.</br></br>
- The launch directory of Malinette will NOT affect any path referenced by a malifile.</br></br>

## Supported options
### Debug
These are options who can be useful to debug malifiles errors.</br>
All debug options are formatted like so: -D[info].</br>
Currently supported options are:</br>
- -DTypes: will display all tokens type before outputting any test result (ex: testName, command...).</br>
- -DValues: will display all tokens values before outputting any test result (ex: Test Subject 00, clang...).</br>

# Disclaimers
- Like say above, WinMakefile has been made with ducts and tapes, it's not my priority to learn how Windows CMD works for the moment.</br>
Until then, it will stay like this unless I find someone to make it better.</br></br>

- Even if Malinette is designed to be cross-platform, malifiles are not, some can works in a specific OS or computer and not in an other one, due to some commands being unusable or some program missing from the system or PATH of the end user.</br></br>
  
- Speaking of incompatibilities... Malinette treating each commands as a program to execute, it can do command like "echo" no problem on Linux since it IS a program, what the same command on Windows IS NOT, That's right, you can't put system commands like "echo" on Windows, and don't try "cmd /c", IT WILL NOT WORK, because Malinette do some preprocessing to improve cross-compatibilty of malifiles under the hood that will not permit this kind of things ! Add that with some weird quirks and you have a recipe for failure.</br>
For these reasons, I strongly recommend using functions like ":launch(echo):" instead, at leasts, it works for everyone. More functions can be added if necessary.</br></br>
  
- Malifile errors can be somewhat unclear, some efforts has been made to make them more useful but it's still not enough.</br>
This system may improve over time.</br>
