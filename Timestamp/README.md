# Timestamp
This prototype is composed of a single program, *timestamp*, that is capable of
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
note the output is not strictly conforming to the format specified by IETF
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
the report (csv) would be recorded in the environment variable described by
**TIMESTAMP_OUTPUT** if it is defined.

To execute the sender and receiver on two different hosts (same argument as
explained above) across an *SSH* channel:
```bash
ts -s -c 10 -b 4096 | ssh joe@ohaton.cs.ualberta.ca ts -r -c 10 -b 4096
```
*joe* is the username for the remote host, you have to execute:
```bash
sudo make -C build install
```
on both machines; otherwise the remote host will not be able to find the
executable unless an absolute path is given.

## Usage Message
To show a list of supported command line options and arguments, issue:
```bash
ts --help
```
