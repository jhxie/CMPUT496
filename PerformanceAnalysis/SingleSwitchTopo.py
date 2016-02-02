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
    _buildOption = {"default": 1, "delay": 0, "loss": 0}

    def build(self, n=2):
        switch = self.addSwitch("s1")
        # by default mininet's host index starts from 1,
        # so we follow its convention
        for host_idx in range(n):
            # percentage of cpu usage for each host
            # is left as default (unspecified)
            host = self.addHost("h%s" % (host_idx + 1))
            # link is set to be 1Gbps with no delay/loss
            if 0 != SingleSwitchTopo.getBuildOption()["default"]:
                self.addLink(host, switch, bw=1000)
            elif 0 != SingleSwitchTopo.getBuildOption()["delay"]:
                delay_val = SingleSwitchTopo.getBuildOption()["delay"]
                self.addLink(host, switch, bw=1000, delay=str(delay_val)+"ms")
            elif 0 != SingleSwitchTopo.getBuildOption()["loss"]:
                loss_val = SingleSwitchTopo.getBuildOption()["loss"]
                self.addLink(host, switch, bw=1000, loss=loss_val)

    @classmethod
    def getBuildOption(topo):
        return topo._buildOption

    @classmethod
    def setBuildOption(topo, option, arg=1):
        validOptions = SingleSwitchTopo.getBuildOption().keys()
        if option not in validOptions:
            raise ValueError("option must be one of " +
                             " ".join(validOptions) +
                             "!")
        else:
            buildOption = SingleSwitchTopo.getBuildOption()
            for candidateOption in buildOption:
                if option != candidateOption:
                    buildOption[candidateOption] = 0
                else:
                    buildOption[candidateOption] = arg
