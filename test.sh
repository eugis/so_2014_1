set -e

function _test() {
    local ipc="$1"
    local listen="$2"
    local connect="$3"

    echo "Testing $ipc"
    make IPC=$ipc
    bin/reset db fixtures.txt

    bin/server $listen &
    server=$!
    sleep 0.1

    if [[ ! $connect ]]; then
        connect="$listen/$server"
    fi

    # If any of these fail, `set -e` will force our exit
    bin/client $connect list
    bin/client $connect buy 1
    bin/client $connect get 1

    kill -SIGINT $server
    wait $server
}

function test() {
    _test $@ > /dev/null
    echo -e "OK $@"
}

test FILESIG tmp
test SOCKETS 5000 127.0.0.1:5000
test SHMEM $0 $0
test MQUEUES $0 $0
test FIFOS tmp

echo "Finished"