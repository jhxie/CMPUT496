#!/usr/bin/env python

"""
This python script is based on the example given in
https://github.com/mininet/mininet/wiki/Introduction-to-Mininet
"""
from mininet.topo import Topo
from mininet.net import Mininet
from mininet.link import TCLink
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel


class SingleSwitchTopo(Topo):
    """
    Single switch connected to n hosts.
    """
    def build(self, n=2):
        switch = self.addSwitch("s1")
        # by default mininet's host index starts from 1,
        # so we follow its convention
        for host_idx in range(n):
            # percentage of cpu usage for each host
            # is left as default (unspecified)
            host = self.addHost("h%s" % (host_idx + 1))
            # link is set to be 1Gbps with no loss
            self.addLink(host, switch, bw=1000)


def perfTest():
    """
    Create network and run performance test  with 1 switch and 2 hosts.
    """
    topo = SingleSwitchTopo(n=2)
    net = Mininet(topo, link=TCLink)
    net.start()
    print("Dumping host connections")
    dumpNodeConnections(net.hosts)
    print("Testing network connectivity")
    net.pingAll()

    # Host is an alias for Node
    h1, h2 = net.getNodeByName("h1", "h2")
    print("Testing bandwidth between h1 ({}) and h2 ({})"
          .format(h1.IP(), h2.IP()))

    # The built-in Mininet.iperf is only suitable for very simple performance
    # tests since it only supports very limited amount of customization
    # We choose to run 'iperf' as a command instead in order to pass
    # custom command line flags to it
    # net.iperf(hosts=(h1, h2), l4Type="TCP")
    h1.cmd("iperf -f m -s > /tmp/iperf_server_report &")

    # File to be transferred up to 2 ^ 10
    for mb in range(1, 11):
        print("File Size [{}]".format(str(2 ** mb)))
        h2.cmd("dd if=/dev/zero of={}M bs=1M count={}"
               .format(str(2 ** mb), str(2 ** mb)))
        h2.cmd("iperf -f m -c {} -F {}M  > /dev/null 2>&1"
               .format(h1.IP(), str(2 ** mb)))

    h1.cmd("kill %")
    h2.cmd("kill %")
    net.stop()

    print("Final iperf Report from Server Side")
    with open("/tmp/iperf_server_report") as report:
        for line in report:
            print(line)

if __name__ == "__main__":
    # Tell mininet to print useful information
    setLogLevel("info")
    perfTest()
