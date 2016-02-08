# Performance Analysis using Mininet and Iperf
The analysis is performed using 3 distinct metrics under a simplified case of
**linear** network topology with 1 switch and 2 hosts:  
![topo](./doc/SingleSwitchTopo.png)  
The numbers assigned to the switch and 2 hosts correspond to the default names
given by mininet, which are actually named "h1", "s1", and "h2".
The bandwidth for both links are set to the maximum possible 1Gbps for mininet
version 2.2.1.
(Please refer to [SingleSwitchTopo.py](./SingleSwitchTopo.py) for furthur
 implementation details)  
After the setup 3 separate tests are performed as demonstrated below.
## File Size
| File Size (MB) | Average (Mbps) | Standard Deviation  |
|:--------------:| --------------:| -------------------:|
| 2              | 1045.0         |  79.37568897338781  |
| 4              | 911.0          |  42.31430018327138  |
| 8              | 876.6          |  45.224993090104505 |
| 16             | 880.0          |  11.180339887498949 |
| 32             | 882.8          |  11.189280584559492 |
| 64             | 854.8          |  6.685805860178712  |
| 128            | 832.8          |  4.764451699828639  |
| 256            | 831.8          |  3.5637059362410923 |
| 512            | 824.0          |  2.8284271247461903 |
| 1024           | 824.6          |  3.5777087639996634 |
## Latency
| Latency (MS) | Average (Mbps) | Standard Deviation   |
|:------------:| --------------:| --------------------:|
| 1            | 827.2          | 4.147288270665545    |
| 21           | 746.2          | 4.658325879540847    |
| 41           | 645.6          | 10.573551910309043   |
| 61           | 474.4          | 4.159326868617084    |
| 81           | 346.8          | 8.167006795638166    |
| 101          | 257.6          | 25.461735997374568   |
| 121          | 219.6          | 2.073644135332772    |
| 141          | 175.4          | 12.660963628413123   |
| 161          | 102.5          | 48.573758347486354   |
| 171          | 113.2          | 38.71304689636299    |
## Loss Rate
| Loss Rate (%) | Average (Mbps) | Standard Deviation   |
|:-------------:| --------------:| --------------------:|
| 0             | 807.0          | 3.6742346141747673   |
| 1             | 280.4          | 45.5828915274141     |
| 2             | 58.4           | 12.212698309546502   |
| 3             | 30.18          | 4.318796128552495    |
| 4             | 15.9           | 2.1587033144922905   |
