# CMPUT 496
Shared Repository of Course Material for CMPUT 496.

## Network Performance Analysis using Mininet
Please remember to have the **mininet** installed first, to plot the performance
and network topology graph , **graphviz** and **matplotlib** are needed as well.  
If you are on a Debian-based linux machine (ubuntu for example), type the
following to install the dependency:
```bash
sudo apt-get install graphviz mininet python-matplotlib
```
By default all 3 reports are generated under */tmp/* directory and named
**IperfClientFileSizeReport**, **IperfClientLatencyReport**, and
**IperfClientLossReport**, feel free to move it elsewhere since the next
time the script is run it would be over-written.

To generate the performance test report only, type
```bash
sudo python PerformanceAnalysis/perfTest.py -t
```

To plot the performance graph only (remember to have matplotlib installed), type
```bash
sudo python PerformanceAnalysis/perfTest.py -p
```

To perform the above 2 actions in sequence, type
```bash
sudo python PerformanceAnalysis/perfTest.py -a
```

To render the network topology graph (remember to have graphviz installed), type
```bash
vimdot PerformanceAnalysis/SingleSwitchTopo.dot
```
