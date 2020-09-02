#!/bin/bash

help()
{
        echo "Usage: $1 -o <op> -s <socket> -t <template dir>"
}


[[ $# -eq 0 ]] && help $0 && exit 1

op=""
socket=""
tdir="/run/vm"

while getopts "o:s:t:h" opt; do
        case ${opt} in
                o)
                        op=$OPTARG ;;
                s)
                        socket=$OPTARG ;;
                t)
                        tdir=$OPTARG ;;
                h)
                        help $0
                        exit 1
                        ;;
        esac
done
shift $(( OPTIND - 1 ))

if [[ $op == "save" ]]; then
        echo '{"execute":"qmp_capabilities"}{"execute":"migrate-set-capabilities", "arguments":{"capabilities": [{"capability":"bypass-shared-memory", "state":true}]}}' | nc -U $socket
        echo '{"execute":"qmp_capabilities"}{"execute":"query-migrate-capabilities"}' | nc -U $socket | tail -n 1 | sed 's/{/\n{/g'
        echo '{"execute":"qmp_capabilities"}{"execute":"migrate", "arguments":{"uri":"exec:cat' '>' "${tdir}/state\"}}" | nc -U $socket

elif [[ $op == "status" ]]; then
        echo '{"execute":"qmp_capabilities"}{"execute":"query-migrate"}' | nc -U $socket
        echo '{"execute":"qmp_capabilities"}{"execute":"query-status"}' | nc -U $socket
elif [[ $op == "quit" ]]; then
        echo '{"execute":"qmp_capabilities"}{"execute":"quit"}' | nc -U $socket
elif [[ $op == "powerdown" ]]; then
        echo '{"execute":"qmp_capabilities"}{"execute":"system_powerdown"}' | nc -U $socket
elif [[ $op == "netdev_add" ]]; then
        echo '{"execute":"qmp_capabilities"}{"execute":"netdev_add","arguments":{"type":"tap", "ifname":"qtap0", "id":"mytap", "script":"no", "downscript":"no", "vhost":"on"}}' | nc -U $socket
        echo '{"execute":"qmp_capabilities"}{"execute":"device_add","arguments":{"driver":"virtio-net", "netdev":"mytap", "id":"vnic"}}' | nc -U $socket
else
        echo "No such operation: $op"
        echo "- netdev_add"
        echo "- save"
        echo "- status"
        echo "- powerdown"
        echo "- quit"
fi
