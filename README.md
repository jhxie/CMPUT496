# CMPUT 496
Shared Repository of Course Material for CMPUT 496.

## [I] Network Performance Analysis using Mininet
Please remember to have the **mininet** installed first, to plot the performance
graph, **matplotlib** is needed as well.  
If you are on a Debian-based linux machine (ubuntu for example), type the
following to install the dependency:
```bash
sudo apt-get install python-matplotlib
```
By default the report is generated under */tmp/* directory and named
**iperf\_server_report**, feel free to move it else where since the next
time the script is run it would be over-written.

To generate the report, type
```bash
sudo python performance_analysis/SingleSwitchTopo.py
```

To plot the performance graph (remember to have matplotlib installed), type
```bash
sudo python performance_analysis/plotPerfResult.py
```
For now the data is **hard-coded** within the *plotPerfResult.py* script, but
Jiahui plans to add parsing functionality of generated report to it as soon as
possible.
