#!/usr/bin/env python

from __future__ import print_function  # use python3 print function instead

import matplotlib.pyplot as plt
import os
import sys

from mininet.net import Mininet
from mininet.link import TCLink
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel

from SingleSwitchTopo import SingleSwitchTopo


# Quoted verbatim from the mininet/net.py under 'iperf' method:
# "
# note: send() is buffered, so client rate can be much higher than
# the actual transmission rate; on an unloaded system, server
# rate should be much closer to the actual receive rate
# "
reportNames = ["IperfServerFileSizeReport",
               "IperfServerLatencyReport",
               "IperfServerLossReport"]


def perfTestFileSize(rangeMin=1, rangeMax=11, useDD=False):
    """
    Create network and run performance test using file size as a parameter
    (i.e. no message loss or latency) with 1 switch and 2 hosts topology.
    The 3 optional parameters stand for the lower and upper bound exponent
    of file size with 2 as the base, along with the option to use the data
    description program to read random data from 'urandom' device, write to
    a file, then pass the file to 'iperf' client.
    Example
    perfTestFileSize(1,11) would lead to file size range from 2MB up to 1024MB.
    """
    for argument in (rangeMin, rangeMax):
        if not isinstance(argument, int) or 0 > argument:
            raise ValueError("argument must be non-negative integers")

    print("!! Performing Network Performance Test Using File Size !!")
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
    h1.cmd("iperf -f m -s > /tmp/IperfServerFileSizeReport &")

    # File to be transferred up to 2 ^ 10
    for mb in range(rangeMin, rangeMax):
        print("File Size -------- [{} MB]".format(str(2 ** mb)))
        # To avoid "accidental" compression by some of the application layer
        # protocols, 'urandom' is used here rather than the 'zero' device
        # to achieve better 'fidelity'.
        # Note that the 'fullblock' argument for iflag parameter is required
        # when dd is invoked using 'urandom' device; otherwise dd may not
        # read enough data to write the output file of proper size
        if False == useDD:
            h2.cmd("iperf -f m -c {} -n {}M  > /dev/null 2>&1"
                   .format(h1.IP(), str(2 ** mb)))
        else:
            h2.cmd("dd if=/dev/urandom of={}M bs={}M count=1 iflag=fullblock"
                   .format(str(2 ** mb), str(2 ** mb)))
            h2.cmd("iperf -f m -c {} -F {}M  > /dev/null 2>&1"
                   .format(h1.IP(), str(2 ** mb)))
            h2.cmd("rm -f {}M".format(str(2 ** mb)))

    h1.cmd("kill %")
    h2.cmd("kill %")
    net.stop()
    print("!! Final iperf Report on File Size from Server Side !!")


def perfTestLatency(rangeMin=1, rangeMax=182, rangeStep=20):
    """
    Create network and run performance test using latency as a parameter
    (i.e. no message loss or variation in file size, holding time constant,
    which is set to be 10 seconds by default for iperf version 2) with 1
    switch and 2 hosts topology.
    Here the parameters stands for the latency range in milisecond.
    """
    for argument in (rangeMin, rangeMax, rangeStep):
        if not isinstance(argument, int) or 0 > argument:
            raise ValueError("argument must be non-negative integers")

    print("!! Performing Network Performance Test Using Latency !!")
    # From mininet's documentation as well as in-source comment it is not clear
    # how to set the RTT directly, so delay is used here.
    # In other words, the actual RTT = 2 * delay in a single switch network
    # since ONLY THE FIRST LINK in a single switch network is set to 'delay'.
    # Also, how to change the delay of established link is unknown to the
    # author so network topology is set up and tear down repeatedly
    # which may not be an elegant or efficient way of implementing it
    # The actual sequence is like [1, 21, 41, ... 141, 161, 181] where n = 10
    for delay in range(rangeMin, rangeMax, rangeStep):
        SingleSwitchTopo.setBuildOption("delay", delay)
        topo = SingleSwitchTopo(n=2)
        net = Mininet(topo, link=TCLink)
        net.start()
        print("!! Dumping host connections !!")
        dumpNodeConnections(net.hosts)
        print("!! Testing network connectivity !!")
        net.pingAll()
        h1, h2 = net.getNodeByName("h1", "h2")
        print("!! Testing bandwidth between h1 ({}) and h2 ({}) !!"
              .format(h1.IP(), h2.IP()))
        # since both hosts would be tear down at each iteration
        # we have to be careful and remember to use 'append' shell redirection
        h1.cmd("iperf -f m -s >> /tmp/IperfServerLatencyReport &")
        print("Latency -------- [{} ms]".format(str(delay)))
        h2.cmd("iperf -f m -c {} > /dev/null 2>&1".format(h1.IP()))
        h2.cmd("kill %")
        h1.cmd("kill %")
        net.stop()

    print("!! Final iperf Report on Latency from Server Side !!")


def perfTestLoss(rangeMin=0, rangeMax=5):
    """
    Create network and run performance test using message loss as a parameter
    (i.e. no variation in file size or latency, fix transfer time period as 10
    seconds, as mentioned previously) with 1 switch and 2 hosts topology.
    The argument stands for the PERCENTAGE for data loss on the FIRST LINK.
    """
    for argument in (rangeMin, rangeMax):
        if not isinstance(argument, int) or 0 > argument:
            raise ValueError("argument must be non-negative integers")

    print("!! Performing Network Performance Test Using Message Loss Rate !!")
    # in mininet the loss rate can only be expressed in decimal,
    # no fractions are allowed; i.e. 0.1% data loss rate is not possible
    # here the loss rate range between [1, 10]
    for loss_rate in range(rangeMin, rangeMax):
        SingleSwitchTopo.setBuildOption("loss", loss_rate)
        topo = SingleSwitchTopo(n=2)
        net = Mininet(topo, link=TCLink)
        net.start()
        print("!! Dumping host connections !!")
        dumpNodeConnections(net.hosts)
        print("!! Testing network connectivity !!")
        net.pingAll()
        h1, h2 = net.getNodeByName("h1", "h2")
        print("!! Testing bandwidth between h1 ({}) and h2 ({}) !!"
              .format(h1.IP(), h2.IP()))
        h1.cmd("iperf -f m -s >> /tmp/IperfServerLossReport &")
        print("Loss Rate -------- [{} %]".format(str(loss_rate)))
        h2.cmd("iperf -f m -c {} > /dev/null 2>&1".format(h1.IP()))
        h2.cmd("kill %")
        h1.cmd("kill %")
        net.stop()

    print("!! Final iperf Report on Loss Rate from Server Side !!")


def perfTestAllMetrics():
    """
    Clean all the past generated iperf reports and run all tests.
    Remember to execute the script with proper permission to make the 'rm'
    command actually work.
    """
    os.system("cd /tmp/ && rm -f " + " ".join(reportNames))
    perfTestFileSize()
    perfTestLatency()
    perfTestLoss()


def perfParseResult():
    fileSizeBandwidthList = []
    latencyBandwidthList = []
    lossBandwidthList = []
    for reportName in reportNames:
        with open("/tmp/" + reportName, "r") as report:
            for line in report:
                # The built-in function all() described from official doc
                # "
                # Return True if all elements of the iterable are true
                # (or if the iterable is empty)
                # "
                # As a result, the following if conditional is only executed
                # when both keywords are in the line since the generator
                # expression returns an iterable of True/False values
                if all(keyword in line for keyword in ("MBytes", "Mbits/sec")):
                    measuredBandwidth = float(line.split(" ")[-2])
                    if "IperfServerFileSizeReport" == reportName:
                        fileSizeBandwidthList.append(measuredBandwidth)
                    elif "IperfServerLatencyReport" == reportName:
                        latencyBandwidthList.append(measuredBandwidth)
                    elif "IperfServerLossReport" == reportName:
                        lossBandwidthList.append(measuredBandwidth)
    return fileSizeBandwidthList, latencyBandwidthList, lossBandwidthList


def perfPlotResult():
    """
    Plot the performance graph using the report generated by
    'perfTestFileSize' 'perfTestLatency' 'perfTestLoss'.
    """

    fileSizeBandwidth, latencyBandwidth, lossBandwidth = perfParseResult()

    _, axisArray = plt.subplots(3, sharey=True)

    axisArray[0].plot([2 ** i for i in range(1, 11)], fileSizeBandwidth, "ro-")
    axisArray[0].axis([0, 2 ** 10, 0, 2 ** 10])
    axisArray[0].set_title("Performance Analysis Using Iperf/Mininet")
    axisArray[0].set_xlabel("File Size (MB)")
    axisArray[0].set_ylabel("Bandwidth (Mbps)")
    axisArray[0].grid(True)

    axisArray[1].plot([i for i in range(1, 182, 20)], latencyBandwidth, "bs-")
    axisArray[1].axis([0, 162, 0, 2 ** 10])
    axisArray[1].set_xlabel("Latency (Miliseconds)")
    axisArray[1].set_ylabel("Bandwidth (Mbps)")
    axisArray[1].grid(True)

    axisArray[2].plot([i for i in range(0, 5)], lossBandwidth, "g^-")
    axisArray[2].axis([0, 11, 0, 2 ** 10])
    axisArray[2].set_xlabel("Loss Rate (%)")
    axisArray[2].set_ylabel("Bandwidth (Mbps)")
    axisArray[2].grid(True)

    plt.show()

if __name__ == "__main__":
    # Tell mininet to print useful information
    setLogLevel("info")
    perfTestFlags = ["-t", "--test"]
    perfPlotResultFlags = ["-p", "--plot"]
    perfAllFlags = ["-a", "--all"]

    if any(cmdFlag in sys.argv for cmdFlag in perfTestFlags):
        perfTestAllMetrics()
    elif any(cmdFlag in sys.argv for cmdFlag in perfPlotResultFlags):
        perfPlotResult()
    elif any(cmdFlag in sys.argv for cmdFlag in perfAllFlags):
        perfTestAllMetrics()
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
