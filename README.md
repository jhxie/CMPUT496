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

## Prototype
### Dependencies
Please make sure you have **build-essential** and **cmake** packages installed.
if you are on a Debian-based machine (ubuntu for example), to install all the
build dependencies, issue:
```bash
sudo apt-get install build-essential cmake cmake-extras extra-cmake-modules
```
After all the above build dependencies are installed, make sure your gcc's
version is at least 5.1, then use the following commands to build the binary:
```bash
cd Prototype
mkdir build
cmake -Bbuild -H.
make -j5 -C build
```
where the *5* in the make invocation stands for the total number of CPUs (or
CPU cores) plus one; the final compiled binary will reside in build/src/.

### Documentation
Since this is a prototype program (at least for now), lots of documentation are
left in-source without using appropriate doxygen formatting directives; full
doxygen formatted documentation would be provided after this project goes out
of prototype stage and the codebase becomes relatively stable.

To generate the tentative documentation, install **doxygen** first:
```bash
sudo apt-get install doxygen
```
then perform the following to generate the documentation:
```bash
cd Prototype
doxygen Doxyfile
```
### Overview
For now this prototype measures per-packet response time by using a *timestamp*
program sending timestamps to each other and records the response time (one
way RTT) on the receiver side; refer to its [README.md](./Prototype/README.md)
for details.
