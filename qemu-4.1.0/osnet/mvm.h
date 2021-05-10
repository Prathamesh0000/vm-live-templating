#ifndef _OSNET_MVM_H
#define _OSNET_MVM_H

#define OSNET_MVM 1
#if OSNET_MVM 
//#include "qemu/option.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <linux/kvm.h>
#include <sys/ioctl.h>
#include "../include/sysemu/kvm_int.h"

#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <sys/un.h>

#define OSNET_STR_LEN 100
#define OSNET_MAX_VCPU_ID 1024
#define OSNET_NULL_CHAR '\0'
#define OSNET_FALSE_VALUE -1
#define SOCK_DIR    "/tmp"
#define TERMINATION "termination"

struct osnet_cpumap {
        int pcpus[OSNET_MAX_VCPU_ID];
        unsigned int nvcpus;
        bool is_valid;
        char path[OSNET_STR_LEN];
};

#define OSNET_KVM_MSI_HASHTAB_SIZE 256
#define OSNET_KVMIO 0xBE
#define OSNET_KVM_SET_CPUMAP   _IOW(OSNET_KVMIO, 0x00, struct osnet_cpumap)

#include "../include/sysemu/kvm.h"
/* accel/kvm/kvm-all.c */
struct KVMState
{
    AccelState parent_obj;

    int nr_slots;
    int fd;
    int vmfd;
    int coalesced_mmio;
    int coalesced_pio;
    struct kvm_coalesced_mmio_ring *coalesced_mmio_ring;
    bool coalesced_flush_in_progress;
    int vcpu_events;
    int robust_singlestep;
    int debugregs;
#ifdef KVM_CAP_SET_GUEST_DEBUG
    QTAILQ_HEAD(, kvm_sw_breakpoint) kvm_sw_breakpoints;
#endif
    int max_nested_state_len;
    int many_ioeventfds;
    int intx_set_mask;
    bool sync_mmu;
    bool manual_dirty_log_protect;
    /* The man page (and posix) say ioctl numbers are signed int, but
     * they're not.  Linux, glibc and *BSD all treat ioctl numbers as
     * unsigned, and treating them as signed here can break things */
    unsigned irq_set_ioctl;
    unsigned int sigmask_len;
    GHashTable *gsimap;
#ifdef KVM_CAP_IRQ_ROUTING
    struct kvm_irq_routing *irq_routes;
    int nr_allocated_irq_routes;
    unsigned long *used_gsi_bitmap;
    unsigned int gsi_count;
    QTAILQ_HEAD(, KVMMSIRoute) msi_hashtab[OSNET_KVM_MSI_HASHTAB_SIZE];
#endif
    KVMMemoryListener memory_listener;
    QLIST_HEAD(, KVMParkedVcpu) kvm_parked_vcpus;

    /* memory encryption */
    void *memcrypt_handle;
    int (*memcrypt_encrypt_data)(void *handle, uint8_t *ptr, uint64_t len);

    /* For "info mtree -f" to tell if an MR is registered in KVM */
    int nr_as;
    struct KVMAs {
        KVMMemoryListener *ml;
        AddressSpace *as;
    } *as;
};

struct osnet_cpumap osnet_cpumap;
void osnet_initialize_cpumap(const char *optarg, struct osnet_cpumap *cpumap);
void osnet_default_initialize(struct osnet_cpumap *cpumap);
void osnet_get_path(const char *optarg, struct osnet_cpumap *cpumap);
void osnet_get_cpumap(struct osnet_cpumap *cpumap);
void osnet_print_cpumap(const struct osnet_cpumap *cpumap);
bool osnet_check_cpumap_validity(const MachineState *ms,
                                 struct osnet_cpumap *cpumap);
void osnet_kvm_set_cpumap(const MachineState *ms,
                          const struct osnet_cpumap *cpucmap);
void *osnet_run_cpumap_server(void *arg);
void osnet_terminate_cpumap_server(void);

#endif  /* OSNET_MVM */
#endif  /* _OSNET_MVM_H */

//QemuOptsList osnet_qemu_cpumap_opts = {
//        .name = "osnet_cpumap",
//        .implied_opt_name = "cpumap",
//        .merge_lists = true,
//        .head = QTAILQ_HEAD_INITIALIZER(osnet_qemu_cpumap_opts.head),
//        .desc = {
//                        {
//                                .name = "path",
//                                .type = QEMU_OPT_STRING,
//                                .help = "path to the VCPU-To-PCPU file",
//                        },
//                        {/* End of list */},
//        },
//};
