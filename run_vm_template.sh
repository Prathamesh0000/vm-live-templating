#!/bin/bash



op=""
vcpu=0
ram=0
img=""
tdir="/run/vm/"
idx=0
qemu=$(which kvm)
machine="pc"

echo "using machine type ${machine}"

run_base()
{
        local i=$1
        local vcpu=$2
        local ram=$3
        local img=$4
        local tdir=$5
        local max_vcpu=$(( vcpu - 1 ))
        local port=$(( i + 8800 ))
        local sdir="/tmp"

        $qemu -machine ${machine},accel=kvm \
         -cpu host\
         -smp ${vcpu},cores=${vcpu},threads=1,sockets=1 \
         -m   ${ram},slots=4,maxmem=10240M\
         -drive file=${img},if=virtio \
         -net user,hostfwd=tcp::$((10000+${i}))-:22 \
         -monitor telnet:127.0.0.1:$((1000+${i})),server,nowait \
         -serial telnet:127.0.0.1:$((2000+${i})),server,nowait \
         -net nic 
     
}

run_by_memory_backend()
{
        local i=$1
        i=0
        local vcpu=$2
        local ram=$3
        local img=$4
        local tdir=$5
        local max_vcpu=$(( vcpu - 1 ))
        local port=$(( i + 8800 ))
        local sdir="/tmp"

        $qemu -machine ${machine},accel=kvm,kernel_irqchip=on \
            -cpu host,host-cache-info=on \
            -smp ${vcpu},cores=${vcpu},threads=1,sockets=1 \
            -m   ${ram},slots=4,maxmem=10240M\
            -object memory-backend-file,id=mem0,size=${ram},mem-path=${tdir}/memory,share=on \
            -numa node,nodeid=0,cpus=0-${max_vcpu},memdev=mem0 \
            -drive file=${img},if=virtio \
            -net user,hostfwd=tcp::$((10000+${i}))-:22 \
            -net nic \
            -qmp unix:${sdir}/qmp-${i}.sock,server,nowait \
            -monitor telnet:127.0.0.1:$((1000+${i})),server,nowait \
            -serial telnet:127.0.0.1:$((2000+${i})),server,nowait \
            -nographic
}

run_by_template()
{
        local i=$1
        local vcpu=$2
        local ram=$3
        local img=$4
        local tdir=$5
        local max_vcpu=$(( vcpu - 1 ))
        local port=$(( i + 8800 ))
        local sdir="/tmp"

        $qemu -machine ${machine},accel=kvm,kernel_irqchip=on \
            -cpu host,host-cache-info=on \
            -smp ${vcpu},cores=${vcpu},threads=1,sockets=1 \
            -m   ${ram},slots=4,maxmem=10240M\
            -object memory-backend-file,id=mem0,size=${ram},mem-path="${tdir}/memory",share=off \
            -numa node,nodeid=0,cpus=0-${max_vcpu},memdev=mem0 \
            -drive file=${img},if=virtio \
            -net user,hostfwd=tcp::$((10000+${i}))-:22 \
            -net nic \
            -qmp unix:${sdir}/qmp-${i}.sock,server,nowait \
            -monitor telnet:127.0.0.1:$((1000+${i})),server,nowait \
            -serial telnet:127.0.0.1:$((2000+${i})),server,nowait \
            -nographic
}

help()
{
        echo "Usage: $1 -o <op> -v <vcpu> -r <ram size> -m <image> -t <template dir> -i <index> -h"
}

if [[ $# -eq 0 ]]; then
                                help $0
        exit 1
fi

while getopts "o:v:r:m:t:i:h" opt; do
        case ${opt} in
                o)
                        op=$OPTARG ;;
                v)
                        vcpu=$OPTARG ;;
                r)
                        ram=$OPTARG ;;
                m)
                        img=$OPTARG ;;
                t)
                        tdir=$OPTARG ;;
                i)
                        idx=$OPTARG ;;
                h)
                        help $0
                        exit 1
                        ;;
        esac
done
shift $(( OPTIND - 1 ))

echo $op
echo "id: ${idx}, tcp: $((10000+${idx})), monitor: 127.0.0.1:$((1000+${idx})), serial: 127.0.0.1:$((2000+${idx})), qmp socked:qmp-${idx}.sock "

if [[ $op == "base" ]]; then
        run_base $idx $vcpu $ram $img &
elif [[ $op == "memory_backend" ]]; then
        [[ ! -d $tdir ]] && echo "$tdir does not exist" && exit 1
        run_by_memory_backend $idx $vcpu $ram $img $tdir &
elif [[ $op == "template" ]]; then
        [[ ! -d $tdir ]] && echo "$tdir does not exist" && exit 1
        run_by_template $idx $vcpu $ram $img $tdir &
else
        echo "No such operation: $op"
        echo "- reg"
        echo "- backed_memory"
        echo "- template"
fi

