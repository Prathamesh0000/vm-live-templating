/*
 * vhost_scsi host device
 *
 * Copyright IBM, Corp. 2011
 *
 * Authors:
 *  Stefan Hajnoczi   <stefanha@linux.vnet.ibm.com>
 *
 * This work is licensed under the terms of the GNU LGPL, version 2 or later.
 * See the COPYING.LIB file in the top-level directory.
 *
 */

#ifndef VHOST_SCSI_H
#define VHOST_SCSI_H

#include "hw/qdev.h"
#include "hw/virtio/virtio-scsi.h"
#include "hw/virtio/vhost.h"
#include "hw/virtio/vhost-scsi-common.h"

enum vhost_scsi_vq_list {
    VHOST_SCSI_VQ_CONTROL = 0,
    VHOST_SCSI_VQ_EVENT = 1,
    VHOST_SCSI_VQ_NUM_FIXED = 2,
};

#define TYPE_VHOST_SCSI "vhost-scsi"
#define VHOST_SCSI(obj) \
        OBJECT_CHECK(VHostSCSI, (obj), TYPE_VHOST_SCSI)

typedef struct VHostSCSI {
    VHostSCSICommon parent_obj;
} VHostSCSI;

#endif
