# Prototype
This prototype is composed of a single program, *timestamp*, that is capable of
operating in either sender or receiver mode (not both, if both corresponding
options are given, the options that come last would be the actual mode) depends
on the arguments given.

## Sending Timestamps
To send 10 timestamps to stdout, issue:
```bash
./timestamp -s -c 10
```

## Receiving Timestamps
**REMEMBER** to define the environment variable **TIMESTAMP_OUTPUT** to specify
where the output file containing timestamp records would go:
```bash
export TIMESTAMP_OUTPUT=logfile
```
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
