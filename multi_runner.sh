

create_cow_from_base()
{

    #TODO: create args for base and templates
    if [[ $# -eq 0 ]]; then
        echo "Please enter ID for creating COW images"
        exit 1
    fi

    id=$1
    storageImg=$2
    dir=$(pwd) 

    #storageImg=$dir"/vm-mem-template/ubuntu-image-"${id}".img"
    base=$dir"/ubuntu-ssh.img"

    # Check for root
    if [ "$EUID" -ne 0 ]
    then echo "Please run as root"
    exit
    fi


    # Check if folder exists
    if [ ! -d $dir"/vm-template/" ];
        then
            echo "creating path :"$dir"/vm-template/"
            mkdir "$dir"/vm-template/""
    fi

    # check if template exists
    if [ ! -f "$storageImg" ]; then
        echo "$storageImg does not exist. Creating image from "${base}
        echo "qemu-img create -f qcow2 -b ${base}  ${storageImg}"
        qemu-img create -f qcow2 -b $base $storageImg
        sudo chown $USER ${storageImg}
    fi

}


echo "Running $1 vm's with $2 cores and ${3} size"

noOfVm=${1}
noOfCore=${2}
memSize=${3}
sshport="10000"


dir=$(pwd) 

base=$dir"ubuntu-ssh.img"
backingStore="/run/vm"


for ((i = 1; i <= $noOfVm; i++)) 
do 
    storageImg=$dir"/vm-template/ubuntu-image-"${i}".img"

    echo "Running ${i}th vm with $2 cores "
    create_cow_from_base $i $storageImg
#    sudo bash run_vm_template.sh -o memory_backend -v $noOfCore -r $memSize -m ${storageImg} -t $backingStore -i ${i} &
    sudo bash run_vm_template.sh -o template -v $noOfCore -r $memSize -m ${storageImg} -t $backingStore -i ${i} &
    sleep 0.2m
    free >> freelogs.txt
#    sudo bash ps-mem.sh
done

