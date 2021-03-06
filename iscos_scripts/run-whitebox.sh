#!/bin/bash
# Huaicheng Li <huaicheng@cs.uchicago.edu>
# Run VM with lightnvm support: FEMU as a whitebox SSD (OpenChannel-SSD)

## image directory
#IMGDIR=$HOME/images
## virtual machine disk image
#OSIMGF=$IMGDIR/u14s.qcow2

IMGDIR=$HOME/lighthost/iskos_images
# virtual machine disk image
OSIMGF=$IMGDIR/40G

FEMU_BUILDDIR=$HOME/femu/build_femu



if [[ ! -e "$OSIMGF" ]]; then
	echo ""
	echo "VM disk image couldn't be found ..."
	echo "Please prepare a usable VM image and place it as $OSIMGF"
	echo "Once VM disk image is ready, please rerun this script again"
	echo ""
	exit
fi

sudo $FEMU_BUILDDIR/x86_64-softmmu/qemu-system-x86_64 \
    -name "FEMU-whitebox-SSD" \
    -enable-kvm \
    -cpu host \
    -smp 4 \
    -m 12G \
    -device virtio-scsi-pci,id=scsi0 \
    -device scsi-hd,drive=hd0 \
    -drive file=$OSIMGF,if=none,aio=native,cache=none,format=raw,id=hd0 \
    -device femu,devsz_mb=10240,namespaces=1,lmetasize=16,nlbaf=5,lba_index=3,mdts=10,lnum_ch=2,lnum_lun=8,lnum_pln=2,lsec_size=4096,lsecs_per_pg=4,lpgs_per_blk=512,femu_mode=0,computation_mode=1 \
    -net user,hostfwd=tcp::8080-:22 \
    -net nic,model=virtio #\
#    -nographic \
#    -qmp unix:./qmp-sock,server,nowait
