# Timestamp
*timestamp*, a ~~useless~~ simple one-way *ping* like clone that is capable of
operating in either sender or receiver mode (not both, if both options are
given, the options that come last would be the actual mode) depends on the
arguments given.

## Sending Timestamps
To send 10 timestamps to stdout without any padding bytes, issue:
```bash
./bin/ts -s -c 10
```
To send 10 timestamps with padding bytes of size 4096 bytes each:
```bash
./bin/ts -s -c 10 -b 4096
```

## Receiving Timestamps
**REMEMBER** to define the environment variable **TIMESTAMP_OUTPUT** on the
host executing the receiver if you do not want the timestamp records go
to stdout (which is the default behavior):
```bash
export TIMESTAMP_OUTPUT=logfile.csv
```
note the output does not strictly conform to the format specified by IETF
[comma-separated values](https://tools.ietf.org/html/rfc4180.html) definition
because the line delimeter used is actually LF (no carriage return, as
commonly seen to be used on windows machines).

To receive 10 timestamps from stdin without any padding bytes, issue:
```bash
ts -r -c 10
```
However, the above is normally not what you want; if executing both the sender
and the receiver on the same host is desired, then to send and receive 10
timestamps with 4096 padding bytes each:
```bash
ts -s -c 10 -b 4096 | ts -r -c 10 -b 4096
```
the report (csv) would be recorded in the file designated by the
**TIMESTAMP_OUTPUT** environment variable if it is defined.

To execute the sender and receiver on two different hosts (same argument as
explained above) across an *SSH* channel:
```bash
ts -s -c 10 -b 4096 | ssh joe@ohaton.cs.ualberta.ca ts -r -c 10 -b 4096
```
*joe* is the username for the remote host *ohaton.cs.ualberta.ca*, remember you
have to follow the instructions in the upper level [README.md](../README.md)
to build and **install** the executable on both machines; otherwise the remote
host will not be able to find the executable unless an absolute path is given.

## Usage Message
To show a list of supported command line options and arguments, issue:
```bash
ts --help
```

## tsTest Python Driver Script
To build the *ts* executable and run all 3 tests:
```bash
sudo python tsTest.py -b
```
then the 3 generated plots (in *png* format) along with their reports
(text file) will reside in the *report* folder.
Refer to [README.md](./report/README.md) in *report* folder for the detailed
summary.
