# Timestamp
This prototype is composed of a single program, *timestamp*, that is capable of
operating in either sender or receiver mode (not both, if both options are
given, the options that come last would be the actual mode) depends on the
arguments given.

## Sending Timestamps
To send 10 timestamps to stdout, issue:
```bash
./timestamp -s -c 10
```

## Receiving Timestamps
**REMEMBER** to define the environment variable **TIMESTAMP_OUTPUT** to specify
where the output file containing timestamp records would go:
```bash
export TIMESTAMP_OUTPUT=logfile.csv
```
note the output is not strictly conforming to the format specified by IETF
[comma-separated values](https://tools.ietf.org/html/rfc4180.html) definition
because the line delimeter used is actually LF (no carriage return, as
commonly seen to be used on windows machines).

To receive 10 timestamps from stdin, issue:
```bash
./timestamp -r -c 10
```
However, the above is normally not what you want; use shell redirection
instead:
```bash
./timestamp -s -c 10 | ./timestamp -r -c 10
```
then the report (csv) would be recorded in *logfile*.

## Usage Message
To show a list of supported command line options and arguments, issue:
```bash
./timestamp --help
```
