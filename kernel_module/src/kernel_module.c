#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <asm/io.h>

MODULE_INFO(intree, "Y");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bartlomiej Dmitruk");
MODULE_DESCRIPTION("Kernel module for RISC-V SoC");
MODULE_VERSION("0.01");

#define SYKT_GPIO_BASE_ADDR (0x00100000)
#define SYKT_GPIO_SIZE (0x8000)
#define SYKT_EXIT (0x3333)
#define SYKT_EXIT_CODE (0x7F)

#define ADDR_A (SYKT_GPIO_BASE_ADDR + 0xC8)
#define ADDR_S (SYKT_GPIO_BASE_ADDR + 0xE0)
#define ADDR_W (SYKT_GPIO_BASE_ADDR + 0xD8)

#define MAX_ARG_VALUE 1000

static void __iomem *baseptrA;
static void __iomem *baseptrS;
static void __iomem *baseptrW;
static void __iomem *baseptr;

static struct proc_dir_entry *directory;

static ssize_t read_reg_W(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
    char buffer[16];
    int len = snprintf(buffer, sizeof(buffer), "%d\n", readl(baseptrW));
    if (*offs >= len) return 0;
    if (copy_to_user(user_buffer, buffer, len)) return -EFAULT;
    *offs += len;
    return len;
}

static ssize_t read_reg_A(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
    char buffer[16];
    int len = snprintf(buffer, sizeof(buffer), "%d\n", readl(baseptrA));
    if (*offs >= len) return 0;
    if (copy_to_user(user_buffer, buffer, len)) return -EFAULT;
    *offs += len;
    return len;
}

static ssize_t read_reg_S(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
    char buffer[16];
    int len = snprintf(buffer, sizeof(buffer), "%d\n", readl(baseptrS));
    if (*offs >= len) return 0;
    if (copy_to_user(user_buffer, buffer, len)) return -EFAULT;
    *offs += len;
    return len;
}

static ssize_t write_reg(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
    char buffer[16];
    int write_int;

    if (count > sizeof(buffer) - 1) return -EINVAL;
    if (copy_from_user(buffer, user_buffer, count)) return -EFAULT;
    buffer[count] = '\0';
    if (kstrtoint(buffer, 10, &write_int)) return -EINVAL;

    if (write_int <= 0 || write_int > MAX_ARG_VALUE) return -EINVAL;

    writel(write_int, baseptrA);
    writel(1, baseptrS);
    return count;
}

static const struct file_operations proc_fops_A = {
    .read = read_reg_A,
    .write = write_reg,
};

static const struct file_operations proc_fops_S = {
    .read = read_reg_S,
};

static const struct file_operations proc_fops_W = {
    .read = read_reg_W,
};

static int my_init_module(void) {
    baseptr = ioremap(SYKT_GPIO_BASE_ADDR, SYKT_GPIO_SIZE);
    baseptrA = ioremap(ADDR_A, 4);
    baseptrS = ioremap(ADDR_S, 4);
    baseptrW = ioremap(ADDR_W, 4);

    if (!baseptr || !baseptrA || !baseptrS || !baseptrW) {
        printk(KERN_ERR "Error mapping GPIO memory.\n");
        return -ENOMEM;
    }

    directory = proc_mkdir("proj4dmibar", NULL);
    if (!directory) {
        printk(KERN_ERR "Error creating proc directory.\n");
        iounmap(baseptr);
        iounmap(baseptrA);
        iounmap(baseptrS);
        iounmap(baseptrW);
        return -ENOMEM;
    }

    if (!proc_create("rejA", 0666, directory, &proc_fops_A) ||
        !proc_create("rejS", 0444, directory, &proc_fops_S) ||
        !proc_create("rejW", 0444, directory, &proc_fops_W)) {
        printk(KERN_ERR "Error creating proc entries.\n");
        remove_proc_entry("rejA", directory);
        remove_proc_entry("rejS", directory);
        remove_proc_entry("rejW", directory);
        remove_proc_entry("proj4dmibar", NULL);
        iounmap(baseptr);
        iounmap(baseptrA);
        iounmap(baseptrS);
        iounmap(baseptrW);
        return -ENOMEM;
    }

    return 0;
}

static void my_cleanup_module(void) {
    writel(SYKT_EXIT | (SYKT_EXIT_CODE << 16), baseptr);
    iounmap(baseptr);
    remove_proc_entry("rejA", directory);
    remove_proc_entry("rejS", directory);
    remove_proc_entry("rejW", directory);
    remove_proc_entry("proj4dmibar", NULL);
}

module_init(my_init_module);
module_exit(my_cleanup_module);

