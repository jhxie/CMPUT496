# Estimating Network Offered-Load Using Timestamp
All the 3 tests are done by running the [tsTest.py](../tsTest.py) script with
the following:
```bash
sudo python tsTest.py -b
```
after proper password is entered, the 3 generated reports named
* *tsLossTestResult.txt*
* *tsPadMsgSizeTestResult.txt*
* *tsRTTTestResult.txt*

will reside in this directory.
## Overview
The mininet topology setup is the same as the *Performance Analysis* one, which
is a **linear** topology with 1 switch and 2 hosts:
![topo](../../PerformanceAnalysis/doc/SingleSwitchTopo.png)
The whole experiment is done under the VM provided by mininet
(Ubuntu 14.04 LTS), and the following command:
```bash
uname -r
```
shows the kernel release version is
[3.13.0-24-generic](http://packages.ubuntu.com/trusty/kernel/linux-image-3.13.0-24-generic)
with the corresponding upstream debian and ubuntu patchset applied.

The version of mininet used is obtained by issuing:
```bash
mn --version
```
which is 2.2.1 in the VM.

Note all the tables listed in the following are random samples coming from the
full 1024 messages sent by the *ts* program.

A fundamental flaw in the [tsTest.py](../tsTest.py) test driver script is the
lack of averaging functions: the sample results listed here may turn out to be
outliers, so in the future the averaging functionality may be added to ensure
the data obtained is actually "trustworthy".

**NOTE**
* The unit for measuring time for all the tests below is millisecond.
* All the 3 plots are too large to fit into this page, so links are used
instead.
* The samples are used to give some intuition to the result obtained since it
is impractical to list all of the results here.
## Padding Message Size
Refer to the plot [tsTestPadMsgSize.png](plot/tsTestPadMsgSize.png).

2 Bytes

|Sequence Number|Arrival Time Delta|Normalized Arrival Time|
|:-------------:| ----------------:| ---------------------:|
| 83            | 128              | 128                   |
| 185           | 134              | 134                   |
| 236           | 137              | 137                   |
| 590           | 149              | 149                   |
| 605           | 149              | 149                   |
| 905           | 150              | 150                   |
| 940           | 150              | 150                   |
| 943           | 150              | 150                   |

32 Bytes

|Sequence Number|Arrival Time Delta|Normalized Arrival Time|
|:-------------:| ----------------:| ---------------------:|
| 83            | 136              | 136                   |
| 185           | 142              | 143                   |
| 236           | 145              | 145                   |
| 590           | 161              | 161                   |
| 605           | 161              | 161                   |
| 905           | 162              | 162                   |
| 940           | 162              | 162                   |
| 943           | 162              | 162                   |

512 Bytes

|Sequence Number|Arrival Time Delta|Normalized Arrival Time|
|:-------------:| ----------------:| ---------------------:|
| 83            | 179              | 179                   |
| 185           | 99               | 180                   |
| 236           | 99               | 181                   |
| 590           | 121              | 205                   |
| 605           | 125              | 209                   |
| 905           | 146              | 232                   |
| 940           | 149              | 235                   |
| 943           | 150              | 236                   |

8192 Bytes

|Sequence Number|Arrival Time Delta|Normalized Arrival Time|
|:-------------:| ----------------:| ---------------------:|
| 83            | 131              | 224                   |
| 185           | 212              | 309                   |
| 236           | 135              | 347                   |
| 590           | 142              | 627                   |
| 605           | 131              | 637                   |
| 905           | 1607             | 2528                  |
| 940           | 1937             | 2978                  |
| 943           | 1942             | 2983                  |

Padding message sizes for the *ts* program are set to be 2 bytes, 32 bytes,
512 bytes, and 8192 bytes respectively. For the cases where the padding bytes
are less than 8192, the time it takes for the timestamp to be received on the
other side seems to increase at a relatively slow pace (mostly resemble a curve
of logarithm) with the only exception that when the padding bytes is set to be
512, which has a significant drop when 100 or so timestamps are sent.

When padding size is set to be 8192 bytes, the arrival time increases to around
200 milliseconds when 200 timestamps are sent, and then the time drastically
decreased back to just above 110 miliseconds for some unknown reason.
When timestamps with sequence number in range 200 and 700 are sent, the arrival
time fluctuates around 100 milliseconds; once the "threshold" 700 is reached
(the 700th timestamp), the arrival time suddenly soars and increases
expotentially (note this testcase is tried a few times and all of them resemble
this similar pattern).

For the normalized arrival time, the situation is similar: when the padding
message size is less than 8192 bytes, the curve is almost flat, but once 8192
is set, the normalized arrival time monotonically increases and rises above
the 1000 miliseconds upper bound in the plot when 800 timestamps are about to
be sent.
## Loss Rate
Refer to the plot [tsTestLoss.png](plot/tsTestLoss.png).

0.1 %

|Sequence Number|Arrival Time Delta|Normalized Arrival Time|
|:-------------:| ----------------:| ---------------------:|
| 33            | 939              | 939                   |
| 79            | 940              | 940                   |
| 376           | 955              | 955                   |
| 405           | 955              | 955                   |
| 495           | 956              | 956                   |
| 681           | 959              | 959                   |
| 710           | 959              | 959                   |
| 957           | 973              | 973                   |

0.2 %

|Sequence Number|Arrival Time Delta|Normalized Arrival Time|
|:-------------:| ----------------:| ---------------------:|
| 33            | 923              | 923                   |
| 79            | 923              | 923                   |
| 376           | 927              | 927                   |
| 405           | 928              | 928                   |
| 495           | 929              | 929                   |
| 681           | 949              | 949                   |
| 710           | 950              | 950                   |
| 957           | 955              | 955                   |

0.3 %

|Sequence Number|Arrival Time Delta|Normalized Arrival Time|
|:-------------:| ----------------:| ---------------------:|
| 33            | 955              | 955                   |
| 79            | 956              | 956                   |
| 376           | 957              | 957                   |
| 405           | 957              | 957                   |
| 495           | 957              | 957                   |
| 681           | 958              | 958                   |
| 710           | 958              | 958                   |
| 957           | 966              | 966                   |

5 %

|Sequence Number|Arrival Time Delta|Normalized Arrival Time|
|:-------------:| ----------------:| ---------------------:|
| 33            | 934              | 934                   |
| 79            | 935              | 935                   |
| 376           | 938              | 938                   |
| 405           | 952              | 952                   |
| 495           | 959              | 959                   |
| 681           | 962              | 963                   |
| 710           | 963              | 963                   |
| 957           | 965              | 965                   |

Loss rate is set by the traffic control program (credit section [2]) to be
0.1%, 0.2%, 0.3%, and 5% (credit section [1]) respectively.

All eight plots are nearly identical regardless of what the loss rate is.
This anomaly could possibly be caused by bugs in the *ts* program or the
*tsTest* driver script (or both), so further investigation is required later
on.

For both arrival time delta and normalized arrival time, the time nearly holds
constant at around 960 (taken from the samples listed above) with very litte
fluctuations and does not seem to change relative to number of timestamp sent.

## RTT
Refer to the plot [tsTestRTT.png](plot/tsTestRTT.png).

10 Miliseconds

|Sequence Number|Arrival Time Delta|Normalized Arrival Time|
|:-------------:| ----------------:| ---------------------:|
| 281           | 246              | 246                   |
| 350           | 247              | 247                   |
| 453           | 248              | 248                   |
| 504           | 249              | 249                   |
| 544           | 249              | 249                   |
| 568           | 250              | 250                   |
| 637           | 250              | 250                   |
| 737           | 251              | 251                   |

30 Miliseconds

|Sequence Number|Arrival Time Delta|Normalized Arrival Time|
|:-------------:| ----------------:| ---------------------:|
| 281           | 477              | 477                   |
| 350           | 478              | 478                   |
| 453           | 479              | 479                   |
| 504           | 480              | 480                   |
| 544           | 480              | 480                   |
| 568           | 481              | 481                   |
| 637           | 492              | 492                   |
| 737           | 495              | 495                   |

50 Miliseconds

|Sequence Number|Arrival Time Delta|Normalized Arrival Time|
|:-------------:| ----------------:| ---------------------:|
| 281           | 717              | 717                   |
| 350           | 734              | 734                   |
| 453           | 738              | 738                   |
| 504           | 740              | 740                   |
| 544           | 740              | 740                   |
| 568           | 741              | 741                   |
| 637           | 741              | 741                   |
| 737           | 742              | 742                   |

70 Miliseconds

|Sequence Number|Arrival Time Delta|Normalized Arrival Time|
|:-------------:| ----------------:| ---------------------:|
| 281           | 935              | 935                   |
| 350           | 935              | 935                   |
| 453           | 935              | 935                   |
| 504           | 937              | 937                   |
| 544           | 937              | 938                   |
| 568           | 938              | 938                   |
| 637           | 939              | 939                   |
| 737           | 940              | 940                   |

Round trip time (RTT) for the virtual network topology between two hosts is set
to be 10 ms, 30 ms, 50 ms, and 70 ms.

Judging from the plot obtained for arrival time deltas, it clearly shows the
timestamp program is very sensitive to the round trip time (referred as RTT
later on); to be more specific, there seems to exist a positive correlation
between round trip time and arrival time deltas, which "intuitively" make
sense. Based on the samples shown above, the arrival time deltas stay pretty
close to 250 milliseconds when RTT is set to 10 ms. All the rest plots
generally follow a "staircase" pattern: the arrival time deltas change very
litte given a fixed RTT; but it changes drastically when RTT increases, just
as shown in the sample, 470 ms - 500 ms given 30 ms RTT, 720 ms - 740 ms given
50 ms RTT, and 930 ms - 940 ms given 70 ms RTT.

Another similar anomaly occurred for the normalized arrival time, which in the
author's opinion should increase monotonically but for some reason is almost
identical to the arrival time deltas. Again, further investigation required.
## Credit
1.Thanks Professor Lu for giving suggestions for improvement.
2.Thanks Nooshin for the instructions on using *tc* and *netem*.
