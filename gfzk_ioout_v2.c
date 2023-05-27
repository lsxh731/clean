#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>

// gfzk_V2.0 2023-04-28
// arch/arm/plat-mxc/include/mach/iomux-mx6q.h 
// arch/arm/mach-mx6/board-mx6q_topeet.h 
// arch/arm/mach-mx6/board-mx6q_topeet.c

#define DEVICE_NAME				"ioout"
#define DRIVER_NAME             "ioout"

//22 pin
static int gpios[] = {
		IMX_GPIO_NR(3,15),		//(low)EIM_DA15 	COOLING 		序号1		冷却-继电器
		IMX_GPIO_NR(3,0),		//(low)EIM_DA0 		FIELD			序号2		励磁-继电器
		IMX_GPIO_NR(5,6),		//(low)DISP0_DAT12 	FAN				序号3		风机-继电器
		IMX_GPIO_NR(3,2),		//(low)EIM_DA2 		ST   			序号4   	滑台-继电器
		IMX_GPIO_NR(3,3),		//(low)EIM_DA3 		EXT21 			序号5  		自定21
		IMX_GPIO_NR(3,9),		//(low)EIM_DA9 		HB				序号6		静压轴承油源-继电器  
		IMX_GPIO_NR(3,7),		//(low)EIM_DA7 		MUX_A0			序号7		复用器A0
		IMX_GPIO_NR(3,8),		//(low)EIM_DA8 		MUX_A1			序号8		复用器A1
		IMX_GPIO_NR(3,4),		//(low)EIM_DA4 		MUX_A2			序号9		复用器A2
		IMX_GPIO_NR(3,10),		//(low)EIM_DA10 	MUX_A3			序号10		复用器A3
		IMX_GPIO_NR(2,17), 		//(low)EIM_A21 		CUS11EN			序号11		隔离电源使能Custom11_nEN 未用
		IMX_GPIO_NR(2,16), 		//(low)EIM_A22 		1K0				序号12		从柜1软启动-1K0
		IMX_GPIO_NR(6,6), 		//(low)EIM_A23 		1K1				序号13		从柜1主继电器-1K1
		IMX_GPIO_NR(5,4), 		//(low)EIM_A24 		2K0				序号14		从柜2软启动-2K0
		IMX_GPIO_NR(2,22),		//(low)EIM_A16 		2K1				序号15		从柜2主继电器-2K1
		IMX_GPIO_NR(2,21),		//(low)EIM_A17 		3K0				序号16		从柜3软启动-3K0
		IMX_GPIO_NR(2,20),		//(low)EIM_A18 		3K1				序号17		从柜3主继电器-3K1
		IMX_GPIO_NR(2,19),		//(low)EIM_A19 		4K0				序号18		从柜4软启动-4K0
		IMX_GPIO_NR(2,18),		//(low)EIM_A20 		4K1				序号19		从柜4主继电器-4K1
		IMX_GPIO_NR(6,4), 		//(low)CSI0_DAT18 	RUN				序号20		模控使能信号
		IMX_GPIO_NR(6,5), 		//(low)CSI0_DAT19 	Custom12_nEN	序号21		隔离电源使能Custom12_nEN
};


static int itop6x_ioout_open(struct inode *inode, struct file *file) {
		return 0;
}

static int itop6x_ioout_close(struct inode *inode, struct file *file) {
	return 0;
}

static long itop6x_ioout_ioctl(struct file *filep, unsigned int cmd,
		unsigned long arg)
{
	int ret;
	//printk("%s: cmd = %d\n", __FUNCTION__, cmd);
	switch(cmd) {
		case 0:
			gpio_set_value(gpios[arg], 0);
			break;
		case 1:
			gpio_set_value(gpios[arg], 1);
			break;
		default:
			return -EINVAL;
	}
	ret = gpio_get_value(gpios[arg]);
	printk("####gpios[%d] = %d###\n",arg,ret);
	return 0;
}

static ssize_t itop6x_ioout_write(struct file *filp, char *buffer, size_t count, loff_t *ppos)
{
	char str[20];

	memset(str, 0, 20);

	if(copy_from_user(str, buffer, count))
	{
		printk("Error\n");

		return -EINVAL;
	}

	printk("%s", str);
#if 0
	if(!strncmp(str, "1", 1))
		gpio_set_value(EIM_D17_FAN, 1);
	else
		gpio_set_value(EIM_D17_FAN, 0);
#endif
	return count;
}

static struct file_operations itop6x_ioout_ops = {
	.owner			= THIS_MODULE,
	.open			= itop6x_ioout_open,
	.release		= itop6x_ioout_close, 
	.unlocked_ioctl	= itop6x_ioout_ioctl,
	.write			= itop6x_ioout_write,
};

static struct miscdevice itop6x_misc_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &itop6x_ioout_ops,
};

static int itop6x_ioout_probe(struct platform_device *pdev)
{
	int ret,i;
	
	for(i=0; i<sizeof(gpios)/sizeof(gpios[0]); i++){
		gpio_free(gpios[i]);
		ret = gpio_request(gpios[i], "gpio");
		if (ret){
			printk("\n request GPIO %d failed!!! \n", gpios[i]);
			}
			gpio_direction_output(gpios[i],0);
			gpio_set_value(gpios[i],1);
			ret = gpio_get_value(gpios[i]);
			printk("####gpio[%d] value is = %d\n", i, ret);
	}
#if 0
	gpio_set_value(EIM_D17_FAN, 1);
	ret = gpio_get_value(EIM_D17_FAN);
	printk("####EIM_D17_FAN value = %d\n", ret);
	gpio_set_value(EIM_D17_FAN, 0);
	ret = gpio_get_value(EIM_D17_FAN);
	printk("####EIM_D17_FAN value = %d\n", ret);
	
	gpio_set_value(DISP0_DAT8_3K1, 1);
	ret = gpio_get_value(DISP0_DAT8_3K1);
	printk("####DISP0_DAT8_3K1 value = %d\n", ret);
	gpio_set_value(DISP0_DAT8_3K1, 0);
	ret = gpio_get_value(DISP0_DAT8_3K1);
	printk("####DISP0_DAT8_3K1 value = %d\n", ret);
	
	gpio_set_value(EIM_D17_FAN, 1);
	ret = gpio_get_value(EIM_D17_FAN);
	printk("####EIM_D17_FAN value = %d\n", ret);
	
	gpio_set_value(DISP0_DAT8_3K1, 1);
	ret = gpio_get_value(DISP0_DAT8_3K1);
	printk("####DISP0_DAT8_3K1 value = %d\n", ret);
#endif	
	

	ret = misc_register(&itop6x_misc_dev);

	printk(DEVICE_NAME "#### ioout ### tinitialized\n");

	return 0;
}

static int itop6x_ioout_remove (struct platform_device *pdev)
{
	int i;
	
	for(i=0; i<sizeof(gpios)/sizeof(gpios[0]); i++)
		gpio_free(gpios[i]);
	misc_deregister(&itop6x_misc_dev);
	printk(DEVICE_NAME "#### ioout ### remove\n");

	return 0;
}


static struct platform_driver itop6x_ioout_driver = {
	.probe = itop6x_ioout_probe,
	.remove = itop6x_ioout_remove,
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
};

static int __init itop6x_ioout_init(void) {
	return platform_driver_register(&itop6x_ioout_driver);
}

static void __exit itop6x_ioout_exit(void) {
	platform_driver_unregister(&itop6x_ioout_driver);
}

module_init(itop6x_ioout_init);
module_exit(itop6x_ioout_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TOPEET Inc.");
MODULE_DESCRIPTION("I.MX6 ioout Driver");
