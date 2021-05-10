#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0xb2a92228, "module_layout" },
	{ 0xc839db4c, "device_destroy" },
	{ 0xa957f454, "class_destroy" },
	{ 0x892cb022, "device_create" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xdacf1a6a, "cdev_del" },
	{ 0xebed185e, "__class_create" },
	{ 0x41e6152d, "cdev_add" },
	{ 0xd0e4160e, "cdev_init" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x94890b99, "pid_task" },
	{ 0x2fde9d8e, "find_get_pid" },
	{ 0x362ef408, "_copy_from_user" },
	{ 0xb44ad4b3, "_copy_to_user" },
	{ 0xc5850110, "printk" },
	{ 0xa94a09bb, "mem_section" },
	{ 0x7cd8d75e, "page_offset_base" },
	{ 0xb53396e9, "pv_ops" },
	{ 0xdad13544, "ptrs_per_p4d" },
	{ 0x5367b4b4, "boot_cpu_data" },
	{ 0x72d79d83, "pgdir_shift" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "6A21AABE92A118FC63D5795");
