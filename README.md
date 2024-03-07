## Usage
```
usage: dlblock [-b block-list] command [args]
```

## Example
```sh
# the program you want to run must be dynamically linked

dlblock -b connect curl google.com # makes every `connect(2)` calls fail
# curl: (7) Failed to connect to google.com port 80 after 22 ms: Couldn't connect to server

dlblock -b open,openat touch file # both `open(2)` and `openat(2)` will fail
# touch: cannot touch 'file': Cannot allocate memory
```

## Build
Make sure to have `make` and `gcc` installed
```
git clone https://github.com/sloweax/dlblock
cd dlblock
make install
```
