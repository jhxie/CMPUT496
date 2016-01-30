#!/usr/bin/env python

from __future__ import print_function  # use python3 print function instead

import matplotlib.pyplot as plt

from mininet.net import Mininet
from mininet.link import TCLink
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel

from SingleSwitchTopo import SingleSwitchTopo


def perfTest():
    """
    Create network and run performance test  with 1 switch and 2 hosts.
    """
    topo = SingleSwitchTopo(n=2)
    net = Mininet(topo, link=TCLink)
    net.start()
    print("!! Dumping host connections !!")
    dumpNodeConnections(net.hosts)
    print("!! Testing network connectivity !!")
    net.pingAll()

    # Host is an alias for Node
    h1, h2 = net.getNodeByName("h1", "h2")
    print("!! Testing bandwidth between h1 ({}) and h2 ({}) !!"
          .format(h1.IP(), h2.IP()))

    # The built-in Mininet.iperf is only suitable for very simple performance
    # tests since it only supports very limited amount of customization
    # We choose to run 'iperf' as a command instead in order to pass
    # custom command line flags to it
    # net.iperf(hosts=(h1, h2), l4Type="TCP")
    h1.cmd("iperf -f m -s > /tmp/IperfServerReport &")

    # File to be transferred up to 2 ^ 10
    for mb in range(1, 11):
        print("File Size -------- [{}MB]".format(str(2 ** mb)))
        h2.cmd("dd if=/dev/zero of={}M bs=1M count={}"
               .format(str(2 ** mb), str(2 ** mb)))
        h2.cmd("iperf -f m -c {} -F {}M  > /dev/null 2>&1"
               .format(h1.IP(), str(2 ** mb)))
        h2.cmd("rm -f {}M".format(str(2 ** mb)))

    h1.cmd("kill %")
    h2.cmd("kill %")
    net.stop()

    print("!! Final iperf Report from Server Side !!")
    with open("/tmp/IperfServerReport") as report:
        for line in report:
            print(line)


def perfPlotResult():
    bandwidthList = []
    with open("/tmp/IperfServerReport", "r") as report:
        for line in report:
            # The built-in function all() described from official python doc,
            # "
            # Return True if all elements of the iterable are true (or if the
            # iterable is empty)
            # "
            # As a result, the following if conditional is only executed when
            # both keywords are in the line since the generator expression
            # returns an iterable of True/False values
            if all(keyword in line for keyword in ("MBytes", "Mbits/sec")):
                bandwidthList.append(int(line.split(" ")[-2]))

    plt.plot([2 ** i for i in range(1, 11)], bandwidthList, "ro")
    plt.axis([0, 2 ** 10, 0, 2 ** 10])
    plt.xlabel("File Size (MB)")
    plt.ylabel("Bandwidth (Mbps)")
    plt.show()

if __name__ == "__main__":
    import sys
    # Tell mininet to print useful information
    setLogLevel("info")
    perfTestFlags = ["-t", "--test"]
    perfPlotResultFlags = ["-p", "--plot"]
    perfAllFlags = ["-a", "--all"]

    if any(cmdFlag in sys.argv for cmdFlag in perfTestFlags):
        perfTest()
    elif any(cmdFlag in sys.argv for cmdFlag in perfPlotResultFlags):
        perfPlotResult()
    elif any(cmdFlag in sys.argv for cmdFlag in perfAllFlags):
        perfTest()
        perfPlotResult()
    else:
        print("[Usage]")
        for testOptions in perfTestFlags:
            print(testOptions + "\t", end="")
        print("Perform Network Performance Test Only Using Iperf/Mininet")
        for plotOptions in perfPlotResultFlags:
            print(plotOptions + "\t", end="")
        print("Plot Network Performance Graph Only Using Matplotlib")
        for allOptions in perfAllFlags:
            print(allOptions + "\t", end="")
        print("Perform the Above 2 Actions in Sequence")
