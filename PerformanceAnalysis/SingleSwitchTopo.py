#!/usr/bin/env python

"""
This python script is based on the example given in
https://github.com/mininet/mininet/wiki/Introduction-to-Mininet
"""
from mininet.topo import Topo


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
