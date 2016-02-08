# CMPUT 496
Shared Repository of Course Material for CMPUT 496.

## Network Performance Analysis using Mininet
Please remember to have the **mininet** installed first, to plot the performance
and network topology graph , **graphviz** and **matplotlib** are needed as well.  
If you are on a Debian-based linux machine (ubuntu for example), type the
following to install the dependency
(note it is tested on ubuntu 14.04/15.10 only):
```bash
sudo apt-get install graphviz mininet python-matplotlib
```
By default all 3 reports are generated under */tmp/* directory and are named
**IperfClientFileSizeReport**, **IperfClientLatencyReport**, and
**IperfClientLossReport**, which would be overwritten by previous runs if the
*number of runs* command flag (see below for details) is set to more than 1.
(remember to have matplotlib installed)

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

To export the generated result to a binary data to be imported later ("-f"):
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
