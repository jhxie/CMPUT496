#!/usr/bin/env python

import matplotlib.pyplot as plt


def plotPerfResult():
    plt.plot([2 ** i for i in range(1, 11)],
             [712, 699, 729, 801, 809, 823, 806, 833, 832, 830],
             "ro")
    plt.axis([0, 2 ** 10, 0, 2 ** 10])
    plt.xlabel("File Size (MB)")
    plt.ylabel("Bandwidth (Mbps)")
    plt.show()

if __name__ == "__main__":
    plotPerfResult()
