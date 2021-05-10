# VM Templating
VM templating reduces the service start-up time and initial memory consumption.

# Install QEMU-4.1.0
```
$ apt install git libglib2.0-dev libfdt-dev libpixman-1-dev zlib1g-dev flex bison libnfs-dev libiscsi-dev libcap-dev libattr1-dev
$ git clone git@github.com:osnetsvn/qemu-4.1.0.git
$ cd qemu-4.1.0
$ git checkout origin/mvm -b mvm
$ ./configure --target-list=x86_64-softmmu --enable-virtfs
$ make -j $(nproc)
$ make -j $(nproc)
$ make -j $(nproc) install
```

# Set up hugetlbfs
```
$ echo 2048 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
$ mount -t hugetlbfs hugetlbfs /mnt/hugetlb
```

# Set up tmpfs
```
$ mount -t tmpfs -o size=500M tmpfs /mnt/ramdisk
```

# Create a Template VM
```
$ i=0
$ vcpu=1
$ ram="1G"
$ img="vm.qcow2"
$ mdir="/mnt/hugetlb"
$ max_vcpu=$(( vcpu - 1 ))
$ tport=$(( i + 7000 ))
$ mport=$(( i + 8000 ))
$ sdir="/tmp"

$ qemu-system-x86_64 \
        -machine pc,accel=kvm,kernel_irqchip=on,nvdimm=on \
        -cpu host,host-cache-info=on \
        -smp ${vcpu},cores=${vcpu},threads=1,sockets=1 \
        -m   ${ram},slots=4,maxmem=30G\
        -object memory-backend-file,id=mem0,size=${ram},mem-path=${mdir}/memory,share=on \
        -numa node,nodeid=0,cpus=0-${max_vcpu},memdev=mem0 \
        -drive file=${img},if=virtio \
        -qmp unix:${sdir}/qmp${i}.sock,server,nowait \
        -serial telnet:127.0.0.1:${tport},server,nowait \
        -monitor telnet:127.0.0.1:${mport},server,nowait \
        -parallel none \
        -serial none \
        -net none \
        -vga none \
        -nographic \
        -nodefaults

$ socket="/tmp/qmp0.sock"
$ ddir="/mnt/ramdisk"
$ echo '{"execute":"qmp_capabilities"}{"execute":"migrate-set-capabilities", "arguments":{"capabilities": [{"capability":"bypass-shared-memory", "state":true}]}}' | nc -U $socket
$ echo '{"execute":"qmp_capabilities"}{"execute":"migrate", "arguments":{"uri":"exec:cat' '>' "${ddir}/state\"}}" | nc -U $socket

$ echo '{"execute":"qmp_capabilities"}{"execute":"quit"}' | nc -U $socket
```

# Start a Templated VM
```
$ i=0
$ vcpu=1
$ ram="1G"
$ img="snap.qcow2"
$ mdir="/mnt/hugetlb"
$ ddir="/mnt/ramdisk"
$ max_vcpu=$(( vcpu - 1 ))
$ tport=$(( i + 7000 ))
$ mport=$(( i + 8000 ))
$ sdir="/tmp"

$ qemu-system-x86_64 \
        -machine pc,accel=kvm,kernel_irqchip=on,nvdimm=on \
        -cpu host,host-cache-info=on \
        -smp ${vcpu},cores=${vcpu},threads=1,sockets=1 \
        -m   ${ram},slots=4,maxmem=30G\
        -object memory-backend-file,id=mem0,size=${ram},mem-path=${mdir}/memory,share=off \
        -numa node,nodeid=0,cpus=0-${max_vcpu},memdev=mem0 \
        -drive file=${img},if=virtio \
        -qmp unix:${sdir}/qmp${i}.sock,server,nowait \
        -serial telnet:127.0.0.1:${tport},server,nowait \
        -monitor telnet:127.0.0.1:${mport},server,nowait \
        -incoming "exec:cat ${ddir}/state" \
        -parallel none \
        -serial none \
        -vga none \
        -nographic \
        -osnet_init_ram_state \
        -nodefaults
```

# Migrate a Templated VM from o41 to o40
- For template-aware live migration, we only migrate the additional dirtied memory pages and device states.
- Two key steps to migrate a templated VM.
  - Run a templated VM by `osnet_init_ram_state`.
  - Copy the memory state to the destination.
- Source.
  - Machine: o41.
  - Run a templated VM.
- Destination.
  - Machine: o40.
  - Set up hugetlbfs on o40.
  - Copy the memory template from o41 to hugetlbfs on o40.
  - Run a VM in the migration-listening mode on o40.
```
$ i=0
$ vcpu=1
$ ram="1G"
$ img="snap.qcow2"
$ mdir="/mnt/hugetlb"
$ max_vcpu=$(( vcpu - 1 ))
$ tport=$(( i + 7000 ))
$ mport=$(( i + 8000 ))
$ sdir="/tmp"

$ qemu-system-x86_64 \
        -machine pc,accel=kvm,kernel_irqchip=on,nvdimm=on \
        -cpu host,host-cache-info=on \
        -smp ${vcpu},cores=${vcpu},threads=1,sockets=1 \
        -m   ${ram},slots=4,maxmem=30G\
        -object memory-backend-file,id=mem0,size=${ram},mem-path=${mdir}/memory,share=off \
        -numa node,nodeid=0,cpus=0-${max_vcpu},memdev=mem0 \
        -drive file=${img},if=virtio \
        -qmp unix:${sdir}/qmp${i}.sock,server,nowait \
        -serial telnet:127.0.0.1:${tport},server,nowait \
        -monitor telnet:127.0.0.1:${mport},server,nowait \
        -incoming tcp:10.128.0.40:12345 \
        -parallel none \
        -serial none \
        -vga none \
        -nographic \
        -nodefaults

```
- Migrate the templated VM from o41 to o40.
```
$ socket="/tmp/qmp0.sock"
$ dest="10.128.0.40:12345"
echo '{"execute": "qmp_capabilities"}{"execute": "migrate", "arguments": {"uri": "tcp:'"$dest\"}}" | nc -U $socket
```
