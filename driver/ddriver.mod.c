#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
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
__used __section("__versions") = {
	{ 0x3364d393, "module_layout" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x9b2039e8, "__register_chrdev" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0xc5850110, "printk" },
	{ 0xc959d152, "__stack_chk_fail" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x8a4617fe, "try_module_get" },
	{ 0xab33919d, "module_put" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "59AFA2A28D6C4DE35876891");
