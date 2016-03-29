# CMPUT 496
Shared Repository of Course Material for CMPUT 496.

## Network Performance Analysis using Mininet
### Dependencies
Please remember to have the **mininet** installed first, to plot the performance
and network topology graph , **graphviz** and **matplotlib** are needed as well.
If you are on a Debian-based linux machine (ubuntu for example), type the
following to install the dependency
(note it is tested on ubuntu 14.04/15.10 only):
```bash
sudo apt-get install graphviz mininet python-matplotlib
```
### Overview
By default all 3 reports are generated under */tmp/* directory and are named
**IperfClientFileSizeReport**, **IperfClientLatencyReport**, and
**IperfClientLossReport**, which would be overwritten by previous runs if the
*number of runs* command flag (see below for details) is set to more than 1.

The *perfTest.py* program has 2 main "operating modes"; one of them **MUST** be
sepcified in order to do anything useful.

To specify the *number of runs* and generate a new dataset to be plotted
(remember to have matplotlib installed), type:
```bash
sudo python PerformanceAnalysis/perfTest.py -r [RUNS]
```
where **[RUNS]** stands for the number of runs needed
(please see issue #2 for pitfall).

To import dataset from an existing file (which is exported by *-o* flag) and
plot the result (remember to have matplotlib installed), type:
```bash
sudo python PerformanceAnalysis/perfTest.py -f [FILE]
```

The *perfTest.py* program also supports 2 optional flags that are very useful,
which are discussed below.
**WARNING**
The following flags **MUST** be used along with **ONE** of the operating modes!

To export the generated result to a binary data to be imported later
(*-f* flag):
```bash
sudo python PerformanceAnalysis/perfTest.py -o [FILE]
```

To print the actual dataset obtained from testrun:
```bash
sudo python PerformanceAnalysis/perfTest.py -p
```

To render the network topology graph (remember to have graphviz installed):
```bash
vimdot PerformanceAnalysis/SingleSwitchTopo.dot
```

## Timestamp
### Dependencies
Please make sure you have **build-essential**, **sshpass**,
**python-paramiko**, **python-scp**, and **cmake** packages installed (sshpass
is required for automatically supply password among virtual hosts set up by
mininet).

If you are on a Debian-based machine (ubuntu for example), to install all the
dependencies, issue:
```bash
sudo apt-get install build-essential cmake cmake-extras extra-cmake-modules sshpass python-paramiko python-scp
```
After all the above build dependencies are installed, make sure your gcc's
version is at least 5.1 and cmake's version is above 3 if you are using the VM
image (Ubuntu 14.04 LTS) provided by mininet:
```bash
sudo apt-get install software-properties-common
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install g++-5
sudo add-apt-repository ppa:george-edison55/cmake-3.x
sudo apt-get update
sudo apt-get install cmake
```
then use the following commands to build the binary:
```bash
cd Timestamp
mkdir build
cmake -Bbuild -H.
make -j5 -C build
sudo make -C build install
```
where the *5* in the make invocation stands for the total number of CPUs (or
CPU threads) plus one; the final compiled binary will reside in *build/src/* in
addition to */usr/bin/*.

**NOTE** if you are using the VM image provided by mininet (Ubuntu 14.04 LTS),
use the following cmake command rather the the one shown above; otherwise the
new version of gcc will not be used:
```bash
cmake -DCMAKE_C_COMPILER=/usr/bin/gcc-5 -DCMAKE_CXX_COMPILER=/usr/bin/g++-5 -Bbuild -H.
```

### Documentation
Since this is a prototype program (at least for now), lots of documentation are
left in-source without using appropriate doxygen formatting directives; full
doxygen formatted documentation may be provided after this project goes out
of prototype stage and the codebase becomes relatively stable.

To generate the tentative documentation, install **doxygen** first:
```bash
sudo apt-get install doxygen
```
then perform the following to generate the documentation:
```bash
cd Timestamp
doxygen Doxyfile
```
by default the generated **html** and **latex** will reside in *doc* folder,
browse the **html** version by opening *doc/html/index.html*.
### Overview
For now this prototype measures per-packet response time by using a *timestamp*
program sending timestamps to each other and records the response time (one
way RTT) on the receiver side; refer to its [README.md](./Timestamp/README.md)
for details.
