//https://elixir.bootlin.com/linux/latest/source/arch/x86/kernel/cpu/proc.c
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <generated/utsrelease.h>
#include <linux/cpufreq.h>

#include <linux/smp.h>
#include <linux/timex.h>
#include <linux/string.h>
#include <linux/cpufreq.h>


#include <linux/mm.h>
#include <linux/hugetlb.h>
#include <linux/mman.h>
#include <linux/mmzone.h>
#include <linux/quicklist.h>
#include <linux/swap.h>
#include <linux/vmstat.h>
#include <linux/atomic.h>
#include <linux/vmalloc.h>
#ifdef CONFIG_CMA
#include <linux/cma.h>
#endif
#include <asm/page.h>
#include <asm/pgtable.h>

#include <linux/sched.h>
#include <linux/time.h>
#include <linux/kernel_stat.h>


#include <stdarg.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/types.h>
#include <linux/ctype.h>
#include <linux/kallsyms.h>
#include <linux/math64.h>
#include <linux/uaccess.h>
#include <linux/ioport.h>
#include <linux/dcache.h>
#include <linux/cred.h>
#include <linux/uuid.h>
#include <linux/of.h>
#include <net/addrconf.h>
#include <linux/siphash.h>
#include <linux/compiler.h>
#ifdef CONFIG_BLOCK
#include <linux/blkdev.h>
#endif

#include <asm/page.h>
#include <asm/sections.h>
#include <asm/byteorder.h>

#include <linux/string_helpers.h>


__weak void arch_freq_prepare_all(void)
{
}

static const u16 decpair[100] =
{
#define _(x) (__force u16) cpu_to_le16(((x % 10) | ((x / 10) << 8)) + 0x3030)
    _( 0), _( 1), _( 2), _( 3), _( 4), _( 5), _( 6), _( 7), _( 8), _( 9),
    _(10), _(11), _(12), _(13), _(14), _(15), _(16), _(17), _(18), _(19),
    _(20), _(21), _(22), _(23), _(24), _(25), _(26), _(27), _(28), _(29),
    _(30), _(31), _(32), _(33), _(34), _(35), _(36), _(37), _(38), _(39),
    _(40), _(41), _(42), _(43), _(44), _(45), _(46), _(47), _(48), _(49),
    _(50), _(51), _(52), _(53), _(54), _(55), _(56), _(57), _(58), _(59),
    _(60), _(61), _(62), _(63), _(64), _(65), _(66), _(67), _(68), _(69),
    _(70), _(71), _(72), _(73), _(74), _(75), _(76), _(77), _(78), _(79),
    _(80), _(81), _(82), _(83), _(84), _(85), _(86), _(87), _(88), _(89),
    _(90), _(91), _(92), _(93), _(94), _(95), _(96), _(97), _(98), _(99),
#undef _
};

char *put_dec_trunc8(char *buf, unsigned r)
{
    unsigned q;

    /* 1 <= r < 10^8 */
    if (r < 100)
        goto out_r;

    /* 100 <= r < 10^8 */
    q = (r * (u64)0x28f5c29) >> 32;
    *((u16 *)buf) = decpair[r - 100*q];
    buf += 2;

    /* 1 <= q < 10^6 */
    if (q < 100)
        goto out_q;

    /*  100 <= q < 10^6 */
    r = (q * (u64)0x28f5c29) >> 32;
    *((u16 *)buf) = decpair[q - 100*r];
    buf += 2;

    /* 1 <= r < 10^4 */
    if (r < 100)
        goto out_r;

    /* 100 <= r < 10^4 */
    q = (r * 0x147b) >> 19;
    *((u16 *)buf) = decpair[r - 100*q];
    buf += 2;
out_q:
    /* 1 <= q < 100 */
    r = q;
out_r:
    /* 1 <= r < 100 */
    *((u16 *)buf) = decpair[r];
    buf += r < 10 ? 1 : 2;
    return buf;
}

char *put_dec_full8(char *buf, unsigned r)
{
    unsigned q;

    /* 0 <= r < 10^8 */
    q = (r * (u64)0x28f5c29) >> 32;
    *((u16 *)buf) = decpair[r - 100*q];
    buf += 2;

    /* 0 <= q < 10^6 */
    r = (q * (u64)0x28f5c29) >> 32;
    *((u16 *)buf) = decpair[q - 100*r];
    buf += 2;

    /* 0 <= r < 10^4 */
    q = (r * 0x147b) >> 19;
    *((u16 *)buf) = decpair[r - 100*q];
    buf += 2;

    /* 0 <= q < 100 */
    *((u16 *)buf) = decpair[q];
    buf += 2;
    return buf;
}

char *put_dec(char *buf, unsigned long long n)
{
    if (n >= 100*1000*1000)
        buf = put_dec_full8(buf, do_div(n, 100*1000*1000));
    /* 1 <= n <= 1.6e11 */
    if (n >= 100*1000*1000)
        buf = put_dec_full8(buf, do_div(n, 100*1000*1000));
    /* 1 <= n < 1e8 */
    return put_dec_trunc8(buf, n);
}

int num_to_str(char *buf, int size, unsigned long long num)
{
    char tmp[sizeof(num) * 3] __aligned(2);
    int idx, len;


    if (num <= 9)
    {
        tmp[0] = '0' + num;
        len = 1;
    }
    else
    {
        len = put_dec(tmp, num) - tmp;
    }

    if (len > size)
        return 0;
    for (idx = 0; idx < len; ++idx)
        buf[idx] = tmp[len - idx - 1];
    return len;
}

static void show_val_kb(struct seq_file *m, const char *s, unsigned long num)
{
    char v[32];
    static const char blanks[7] = {' ', ' ', ' ', ' ',' ', ' ', ' '};
    int len;

    len = num_to_str(v, sizeof(v), num << (PAGE_SHIFT - 10));

    seq_write(m, s, 16);

    if (len > 0)
    {
        if (len < 8)
            seq_write(m, blanks, 8 - len);

        seq_write(m, v, len);
    }
    seq_write(m, " kB\n", 4);
}


static void *c_start(struct seq_file *m, loff_t *pos)
{
    *pos = cpumask_next(*pos - 1, cpu_online_mask);
    if ((*pos) < nr_cpu_ids)
        return &cpu_data(*pos);
    return NULL;
}

static void *c_next(struct seq_file *m, void *v, loff_t *pos)
{
    ++(*pos);
    return c_start(m, pos);
}

static int o_show(struct seq_file *m, void *v)
{

    seq_puts(m,"\n=============Memory=============\n");

    struct sysinfo i;
    unsigned long committed;
    long available;
    unsigned long pages[NR_LRU_LISTS];
    int lru;

    si_meminfo(&i);
    committed = percpu_counter_read_positive(&vm_committed_as);


    for (lru = LRU_BASE; lru < NR_LRU_LISTS; lru++)
        pages[lru] = global_node_page_state(NR_LRU_BASE + lru);

    available = si_mem_available();

    show_val_kb(m, "MemTotal:       ", i.totalram);
    show_val_kb(m, "MemFree:        ", i.freeram);
    show_val_kb(m, "Buffers:        ", i.bufferram);
    show_val_kb(m, "Active:         ", pages[LRU_ACTIVE_ANON] +
                pages[LRU_ACTIVE_FILE]);
    show_val_kb(m, "Inactive:       ", pages[LRU_INACTIVE_ANON] +
                pages[LRU_INACTIVE_FILE]);
    show_val_kb(m, "Shmem:          ", i.sharedram);
    show_val_kb(m, "Dirty:          ",
                global_node_page_state(NR_FILE_DIRTY));
    show_val_kb(m, "Writeback:      ",
                global_node_page_state(NR_WRITEBACK));
    seq_printf(m, "KernelStack:    %8lu kB\n",
               global_zone_page_state(NR_KERNEL_STACK_KB));
    show_val_kb(m, "PageTables:     ",
                global_zone_page_state(NR_PAGETABLE));

    seq_puts(m,"\n");
    seq_puts(m,"\n=============Time=============\n");

    struct timespec uptime;
    struct timespec idle;
    u64 nsec;
    u32 rem;
    int i1;

    nsec = 0;
    for_each_possible_cpu(i1)
    nsec += (__force u64) kcpustat_cpu(i1).cpustat[CPUTIME_IDLE];

    get_monotonic_boottime(&uptime);
    idle.tv_sec = div_u64_rem(nsec, NSEC_PER_SEC, &rem);
    idle.tv_nsec = rem;
    seq_printf(m, "Uptime\t\t: %lu.%02lu (s)\nIdletime\t: %lu.%02lu (s)\n",
               (unsigned long) uptime.tv_sec,
               (uptime.tv_nsec / (NSEC_PER_SEC / 100)),
               (unsigned long) idle.tv_sec,
               (idle.tv_nsec / (NSEC_PER_SEC / 100)));

    return 0;
}

static void c_stop(struct seq_file *m, void *v)
{
    o_show(m,v);
}

static int version_proc_show(struct seq_file *m, void *v)
{
    struct cpuinfo_x86 *c = v;
    unsigned int cpu;

    if(c->cpu_index == 0)
    {
        seq_puts(m,"\n============Vesion============\n");
        seq_printf(m,"Linux version %s",UTS_RELEASE);
        seq_puts(m,"\n\n");
        seq_puts(m,"\n=============CPU==============\n");
    }

    cpu = c->cpu_index;
    seq_printf(m, "processor\t: %u\n"
               "model name\t: %s\n",
               cpu,
               c->x86_model_id[0] ? c->x86_model_id : "unknown");
    seq_printf(m, "physical id\t: %d\n", c->phys_proc_id);
    seq_printf(m, "core id\t\t: %d\n", c->cpu_core_id);
    seq_printf(m, "cpu cores\t: %d\n", c->booted_cores);

    if (c->x86_cache_size >= 0)
    {
        seq_printf(m, "cache size\t: %d KB\n", c->x86_cache_size);
    }
    seq_printf(m, "clflush size\t: %u\n", c->x86_clflush_size);
    seq_printf(m, "cache_alignment\t: %d\n", c->x86_cache_alignment);
    seq_printf(m, "address sizes\t: %u bits physical, %u bits virtual\n",
               c->x86_phys_bits, c->x86_virt_bits);

    seq_puts(m,"\n");

    return 0;
}

static struct seq_operations proc_ops =
{
    .start = c_start,
    .next = c_next,
    .stop = c_stop,
    .show = version_proc_show,
};

static int version_proc_open(struct inode *inode, struct file *file)
{
    arch_freq_prepare_all();
    return seq_open(file, &proc_ops);
}

static const struct file_operations version_proc_fops =
{
    .open		= version_proc_open,
    .read		= seq_read,
    .llseek		= seq_lseek,
    .release	= seq_release,
};

static int __init proc_version_init(void)
{
    proc_create("my_info", 0, NULL, &version_proc_fops);
    return 0;
}

static void __exit proc_version_exit(void)
{

    remove_proc_entry("my_info", NULL);
}

MODULE_LICENSE("GPL");
module_init(proc_version_init);
module_exit(proc_version_exit);