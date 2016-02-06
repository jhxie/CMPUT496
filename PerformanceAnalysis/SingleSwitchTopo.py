#!/usr/bin/env python

"""
This python script is based on the example given in
https://github.com/mininet/mininet/wiki/Introduction-to-Mininet
"""

from mininet.topo import Topo


class SingleSwitchTopo(Topo):
    """
    Single switch connected to n hosts.
    Please see the caveat in the overriden 'build' function documentation.
    """
    _buildOption = {"default": 1, "delay": 0, "loss": 0}

    def build(self, n=2):
        """
        Overrides the default 'build' member function(or method) rather
        than '__init__' as recommended in the mininet documentation since
        version 2.2.0 to build a network with 2 hosts.
        Please DO NOT change the default parameter to other number of hosts!
        Otherwise you may encounter inconsistencies in testing the topology
        because only the FIRST LINK is set with appropriate attributes.
        """
        switch = self.addSwitch("s1")
        buildOption = SingleSwitchTopo.getBuildOption()
        # by default mininet's host index starts from 1,
        # so we follow its convention
        for host_idx in range(n):
            # percentage of cpu usage for each host
            # is left as default (unspecified)
            host = self.addHost("h%s" % (host_idx + 1))
            # link is set to be 1Gbps with no delay/loss
            if 0 != buildOption["default"]:
                self.addLink(host, switch, bw=1000)
                continue
            # -------------------------- CAVEAT -------------------------------
            # Only the FIRST LINK is set with the specifed attribute
            # for both 'delay' and 'loss', so this topology should NEVER be
            # used with MORE THAN 2 HOSTS!
            # Otherwise you MUST use the FIRST HOST to avoid inconsistencies
            # in various test cases.
            # -------------------------- CAVEAT -------------------------------
            elif 0 == host_idx and 0 != buildOption["delay"]:
                delay_val = buildOption["delay"]
                self.addLink(host, switch, bw=1000, delay=str(delay_val)+"ms")
                continue
            elif 0 == host_idx and 0 != buildOption["loss"]:
                loss_val = buildOption["loss"]
                self.addLink(host, switch, bw=1000, loss=loss_val)
                continue
            # remember to attach the 2nd host
            self.addLink(host, switch, bw=1000)

    @classmethod
    def getBuildOption(topo):
        """
        Returns the class-wide (pseudo-static) attribute build option.
        """
        return topo._buildOption

    @classmethod
    def setBuildOption(topo, option, arg=1):
        """
        Sets the class-wide (pseudo-static) attribute build option.
        The option itself is an embedded dictionary, and the valid
        keys are 'default', 'delay', and 'loss'.
        """
        # Why potentially over-complicates the codebase with all those
        # fancy classmethod decorators?
        # Well, turns out specifying options like 'bw', 'delay' in the
        # overriden 'build' member function directly run into all sorts of
        # problems that the author have no idea why and is not able to fix,
        # so here it is.
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
