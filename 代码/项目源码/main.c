#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{
    struct task* p_task;
    struct proc* p_proc= proc_table;
    char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
    // u16   selector_ldt = SELECTOR_LDT_FIRST;
    u8    privilege;
    u8    rpl;
    int   eflags;
    int   i, j;
    int   prio;
    for (i = 0; i < NR_TASKS+NR_PROCS; i++)
    {
        if (i >= NR_TASKS + NR_NATIVE_PROCS) {
            p_proc->p_flags = FREE_SLOT;
            p_proc++;
            p_task++;
            continue;
        }
        if (i < NR_TASKS) /* TASK */
        {    
            p_task    = task_table + i;
            privilege = PRIVILEGE_TASK;
            rpl       = RPL_TASK;
            eflags    = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1   1 0010 0000 0010(2)*/
            prio      = 15;     
        }
        else 
        {                  /* USER PROC */
            p_task    = user_proc_table + (i - NR_TASKS);
            privilege = PRIVILEGE_USER;
            rpl       = RPL_USER;
            eflags    = 0x202; /* IF=1, bit 2 is always 1              0010 0000 0010(2)*/
            prio      = 5;     
        }

        strcpy(p_proc->name, p_task->name); 
        p_proc->p_parent = NO_TASK;

        if (strcmp(p_task->name, "Init") != 0) {
            p_proc->ldts[INDEX_LDT_C]  = gdt[SELECTOR_KERNEL_CS >> 3];
            p_proc->ldts[INDEX_LDT_RW] = gdt[SELECTOR_KERNEL_DS >> 3];

            /* change the DPLs */
            p_proc->ldts[INDEX_LDT_C].attr1  = DA_C   | privilege << 5;
            p_proc->ldts[INDEX_LDT_RW].attr1 = DA_DRW | privilege << 5;
        }
        else {      /* INIT process */
            unsigned int k_base;
            unsigned int k_limit;
            int ret = get_kernel_map(&k_base, &k_limit);
            assert(ret == 0);
            init_descriptor(&p_proc->ldts[INDEX_LDT_C],
                  0, /* bytes before the entry point
                      * are useless (wasted) for the
                      * INIT process, doesn't matter
                      */
                  (k_base + k_limit) >> LIMIT_4K_SHIFT,
                  DA_32 | DA_LIMIT_4K | DA_C | privilege << 5);

            init_descriptor(&p_proc->ldts[INDEX_LDT_RW],
                  0, /* bytes before the entry point
                      * are useless (wasted) for the
                      * INIT process, doesn't matter
                      */
                  (k_base + k_limit) >> LIMIT_4K_SHIFT,
                  DA_32 | DA_LIMIT_4K | DA_DRW | privilege << 5);
        }

        p_proc->regs.cs = INDEX_LDT_C << 3 | SA_TIL | rpl;
        p_proc->regs.ds =
        p_proc->regs.es =
        p_proc->regs.fs =
        p_proc->regs.ss = INDEX_LDT_RW << 3 | SA_TIL | rpl;
        p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;

        p_proc->regs.eip = (u32)p_task->initial_eip;
        p_proc->regs.esp = (u32)p_task_stack;
        p_proc->regs.eflags = eflags;

        /* p_proc->nr_tty       = 0; */

        p_proc->p_flags = 0;
        p_proc->p_msg = 0;
        p_proc->p_recvfrom = NO_TASK;
        p_proc->p_sendto = NO_TASK;
        p_proc->has_int_msg = 0;
        p_proc->q_sending = 0;
        p_proc->next_sending = 0;
        p_proc->pid = i;

        for (j = 0; j < NR_FILES; j++)
            p_proc->filp[j] = 0;

        p_proc->ticks = p_proc->priority = prio;

        p_task_stack -= p_task->stacksize;
        p_proc++;
        p_task++;
    }

    k_reenter = 0;
    ticks = 0;

    p_proc_ready = proc_table;

    init_clock();
    init_keyboard();

    restart();

    while(1){}
}




/*****************************************************************************
 *                                get_ticks
 *****************************************************************************/
PUBLIC int get_ticks()
{
    MESSAGE msg;
    reset_msg(&msg);
    msg.type = GET_TICKS;
    send_recv(BOTH, TASK_SYS, &msg);
    return msg.RETVAL;
}


/*======================================================================*
                               TestA
 *======================================================================*/
void TestA()
{
    char tty_name[] = "/dev_tty0";

    //获取标准输入输出流
    int fd_stdin = open(tty_name, O_RDWR);
    assert(fd_stdin == 0);
    int fd_stdout = open(tty_name, O_RDWR);
    assert(fd_stdout == 1);

    char rdbuf[512];//读缓冲区
    char command[64];//得到读取到的命令


    char currentDir[1024] = "/";//默认当前在根目录/
    //文件系统不断循环等待用户响应
    while (1)
    {
        printf("[root@localhost %s]#a ", currentDir);//模拟linux的命令行交互

        //一次从流中读取512字节放到缓冲区里面
        int r = read(fd_stdin, rdbuf, 512);
        rdbuf[r] = 0;//r为字符串实际的结尾位置,这样标识了字符串的结束位置

        //取出用户输入的指令,并且转为实际命令
        int pointer = 0;
        //读到第一个空格的位置,即为主要命令 
        //例如mkdir test,则读取到mkdir
        while (rdbuf[pointer] != 0 && rdbuf[pointer] != ' ')
        {
            command[pointer] = rdbuf[pointer];
            pointer++;
        }
        command[pointer] = 0;//读取前部分指令结束,即类似读取到了mkdir
        printf("%s", command);
    }
}

/*======================================================================*
                               TestB
 *======================================================================*/
void TestB(){ spin("TestB"); }



/*****************************************************************************
 *                                TestC
 *****************************************************************************/
void TestC(){ spin("TestC"); }


/*****************************************************************************
 *                                panic
 *****************************************************************************/
PUBLIC void panic(const char* fmt, ...)
{
    int i;
    char buf[256];

    /* 4 is the size of fmt in the stack */
    va_list arg = (va_list)((char*)&fmt + 4);

    i = vsprintf(buf, fmt, arg);

    printl("%c !!panic!! %s", MAG_CH_PANIC, buf);

    /* should never arrive here */
    __asm__ __volatile__("ud2");
}

/*****************************************************************************
 *                                clear
 *****************************************************************************/
void clear()
{
    char* p = (char*)VGA_MEM_BASE;
    for (int i = 0; i < PIX_WIDTH * PIX_HEIGHT; ++i) {
        p[i] = 0;
    }
    console_table[current_console].crtc_start = 0;
    console_table[current_console].cursor = 0;
}
