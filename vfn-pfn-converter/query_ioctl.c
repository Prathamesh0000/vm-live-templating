#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <asm/io.h>

#include <linux/pid.h>
#include<linux/sched.h>
#include <linux/pgtable.h>
#include <linux/mm_types.h>
#include<asm/mmzone.h>

#include "query_ioctl.h"

#define FIRST_MINOR 0
#define MINOR_CNT 1

static dev_t dev;
static struct cdev c_dev;
static struct class *cl;

query_arg_t q;

static int my_open(struct inode *i, struct file *f)
{
	return 0;
}
static int my_close(struct inode *i, struct file *f)
{
	return 0;
}

long  virtualToPhysical(struct mm_struct * mm, unsigned long vaddr ) {
    long pfn_output = -1;
    pgd_t *pgd;
    pgd = pgd_offset( mm,  vaddr);
    if (!pgd_none(*pgd) || !pgd_bad(*pgd)) {
        p4d_t *p4d;
        p4d = p4d_offset( pgd , vaddr);
		if (!p4d_none(*p4d) || !p4d_bad(*p4d)) {
			pud_t *pud;
			pud = pud_offset( p4d , vaddr);
			if (!pud_none(*pud) || !pud_bad(*pud)) {
				pmd_t *pmd;
				pmd = pmd_offset( pud,  vaddr);
				if (!pmd_none(*pmd) || !pmd_bad(*pmd)) {
					pte_t * pte_offset;
					pte_offset = pte_offset_map( pmd,  vaddr);
					if(!pte_none(*pte_offset) && pte_present( * pte_offset)) {
						unsigned long pfn = pte_pfn(*pte_offset);
						if (pfn && pfn_valid(pfn))
						{
							pfn_output = pfn;
						}
					}
				}
			}
		}
    }
    return pfn_output;
}



#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int my_ioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg)
#else
static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
#endif
{
	struct task_struct * pid_struct;
	struct mm_struct * pid_mm_struct;
	switch (cmd)
	{
		case QUERY_GET_VARIABLES:
			printk("query ioctl: Variable get\n");

			if (copy_to_user((query_arg_t *)arg, &q, sizeof(query_arg_t)))
			{
				return -EACCES;
			}
			break;
		case QUERY_CLR_VARIABLES:

			q.vfn = 0;
			q.pfn = 0;
			q.pid = 0;
			printk("query ioctl: Variable cleared\n");

			break;
		case QUERY_SET_VARIABLES:
			if (copy_from_user(&q, (query_arg_t *)arg, sizeof(query_arg_t)))
			{
				return -EACCES;
			}
			q.pfn = 0;
			if (q.pid) {
				struct pid * pid;
				pid = find_get_pid(q.pid);
				if (pid) {
					pid_struct = pid_task(pid, PIDTYPE_PID);
					pid_mm_struct = pid_struct -> mm;

					struct vm_area_struct * vma;

					// for (vma = pid_mm_struct -> mmap; vma; vma = vma -> vm_next) {
					// 	if( q.vfn == vma -> vm_start) {
					// 		q.pfn = virtualToPhysical(pid_mm_struct, vma -> vm_start);
					// 		printk("query ioctl: Variable get: virtual start long %ld, virtual start %lx ,virtual end %lx, pfn %lx\n", vma -> vm_start, vma -> vm_start, vma -> vm_end, q.pfn);
					// 		break;
					// 	}

					// }
			
					q.pfn = virtualToPhysical(pid_mm_struct, q.vfn);
					printk("query ioctl: Variable set virtual long %ld, virtual add %lx  pfn %lx\n", q.vfn, q.pfn);

				}
			}

			printk("ioctl test: pfn: %lx  pid: %d\n", q.pfn,  q.pid);

			if (copy_to_user((query_arg_t *)arg, &q, sizeof(query_arg_t)))
			{
				return -EACCES;
			}
			break;
		default:
			return -EINVAL;
	}

	return 0;
}

static struct file_operations query_fops =
{
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
	.ioctl = my_ioctl
#else
	.unlocked_ioctl = my_ioctl
#endif
};

static int __init query_ioctl_init(void)
{
	int ret;
	struct device *dev_ret;


	if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "query_ioctl")) < 0)
	{
		return ret;
	}

	cdev_init(&c_dev, &query_fops);

	if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0)
	{
		return ret;
	}
	
	if (IS_ERR(cl = class_create(THIS_MODULE, "char")))
	{
		cdev_del(&c_dev);
		unregister_chrdev_region(dev, MINOR_CNT);
		return PTR_ERR(cl);
	}
	if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "query")))
	{
		class_destroy(cl);
		cdev_del(&c_dev);
		unregister_chrdev_region(dev, MINOR_CNT);
		return PTR_ERR(dev_ret);
	}

	return 0;
}

static void __exit query_ioctl_exit(void)
{
	device_destroy(cl, dev);
	class_destroy(cl);
	cdev_del(&c_dev);
	unregister_chrdev_region(dev, MINOR_CNT);
}

module_init(query_ioctl_init);
module_exit(query_ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prathamesh Patil");
MODULE_DESCRIPTION("Query ioctl() Char Driver");
