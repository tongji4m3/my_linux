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
	struct proc* p_proc = proc_table;
	char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
	// u16   selector_ldt = SELECTOR_LDT_FIRST;
	u8    privilege;
	u8    rpl;
	int   eflags;
	int   i, j;
	int   prio;
	for (i = 0; i < NR_TASKS + NR_PROCS; i++)
	{
		if (i >= NR_TASKS + NR_NATIVE_PROCS) {
			p_proc->p_flags = FREE_SLOT;
			p_proc++;
			p_task++;
			continue;
		}
		if (i < NR_TASKS) /* TASK */
		{
			p_task = task_table + i;
			privilege = PRIVILEGE_TASK;
			rpl = RPL_TASK;
			eflags = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1   1 0010 0000 0010(2)*/
			prio = 15;
		}
		else
		{                  /* USER PROC */
			p_task = user_proc_table + (i - NR_TASKS);
			privilege = PRIVILEGE_USER;
			rpl = RPL_USER;
			eflags = 0x202; /* IF=1, bit 2 is always 1              0010 0000 0010(2)*/
			prio = 5;
		}

		strcpy(p_proc->name, p_task->name);
		p_proc->p_parent = NO_TASK;

		if (strcmp(p_task->name, "Init") != 0) {
			p_proc->ldts[INDEX_LDT_C] = gdt[SELECTOR_KERNEL_CS >> 3];
			p_proc->ldts[INDEX_LDT_RW] = gdt[SELECTOR_KERNEL_DS >> 3];

			/* change the DPLs */
			p_proc->ldts[INDEX_LDT_C].attr1 = DA_C | privilege << 5;
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

	while (1) {}
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

//----------------------------------文件管理代码开始处-----------------------------------------

/*****************************************************************************
 *                           文件管理一些常量
 *****************************************************************************/
 //标识是文件还是目录
#define DIR  1
#define FILE  0

/*****************************************************************************
 *                           FCB
 *						即一个个文件目录项
 *****************************************************************************/
typedef struct FCB
{
	char  fileName[256];//文件名称 类似 local
	char  path[256];//文件路径名称 类似/usr/local
	int fileType;//文件类型
	int size;//子文件的大小
	struct FCB* next;//为寻找children而设置的
	struct FCB* parent;//父文件的FCB
	struct FCB* children;//如果文件类型是目录,就有他自己的目录
} FCB;

//当前路径的FCB
FCB* current;
//光盘源码里面不懂为什么不支持malloc,只能用全局数组来模拟动态开辟
FCB fcbs[100];
int fcbindex = 0;

//将当前目录初始化为根目录
void init()
{
	//初始化root的FCB
	//current = (FCB*)malloc(sizeof(FCB));
	current = &fcbs[fcbindex++];;
	current->fileName[0] = '/';
	current->fileName[1] = 0;

	current->path[0] = '/';
	current->path[1] = 0;  // 结束符

	current->fileType = DIR;
	current->size = 0;
}

//添加子节点
void addChildren(FCB* parent, FCB* son)
{
	parent->size++;
	FCB* temp = parent->children;
	parent->children = son;
	son->parent = parent;
	parent->children->next = temp;
}

//展示当前目录下的所有文件消息
void showChildren(FCB* parent)
{
	printf("fileName  fileType\n");
	int i = 0;
	for (FCB* x = parent->children; i < parent->size; x = x->next)
	{
		printf("%s    ", x->fileName);
		if (x->fileType == DIR) printf("DIR");
		else printf("FILE");
		printf("\n");
		++i;
	}
}

//当前目录路径与相对路径拼接即可得到绝对路径
void getAbsolutePath(char* result, char* path)
{
	strcpy(result, current->path);
	//如果是根路径,就不用加/
	if (strcmp(current->path, "/") != 0) strcat(result, "/");
	strcat(result, path);
}

//进入根目录
void cdRoot()
{
	//找到根路径的PCB
	while (strcmp(current->path, "/") != 0)
	{
		current = current->parent;
	}
}

//辅助cd()的函数,循环更改current,使之进入某绝对路径
void cdAbsolute(char* absolutePath)
{
	cdRoot();

	//为绝对路径末尾添加一个/
	int temp = 0;
	while (absolutePath[temp] != 0) ++temp;
	absolutePath[temp] = '/';
	absolutePath[temp + 1] = 0;

	char fileName[128];
	int i = 0, j = 0;
	while (absolutePath[i] != 0)
	{
		if (i == 0)
		{
			//不进根路径
		}
		else if (absolutePath[i] == '/')//读取到了一个文件 例如/usr/local 读取到了usr 不读路径:/ 
		{
			fileName[j] = 0;//标识fileName结束
			//进入该路径
			int index = 0;
			for (FCB* x = current->children; index < current->size; x = x->next)
			{
				if (strcmp(x->fileName, fileName) == 0 && x->fileType == DIR) //只有目录能进
				{
					current = x;
					break;
				}
				++index;
			}
			j = 0;
		}
		else
		{
			fileName[j] = absolutePath[i];
			++j;
		}
		++i;
	}
}

//进入目录
void cd(char* dir)
{

	if (strcmp(dir, ".") == 0 || strcmp(dir, "./") == 0)
	{
		//进入当前目录 不用处理
	}
	else if (strcmp(dir, "..") == 0 || strcmp(dir, "../") == 0)
	{
		//进入上级目录
		if (strcmp(current->path, "/") != 0)
		{
			current = current->parent;
		}
	}
	else if (strcmp(dir, "/") == 0)
	{
		//进入根目录
		cdRoot();
	}
	else if (dir[0] == '/')
	{
		//说明进入的是绝对路径
		cdAbsolute(dir);
	}
	else
	{
		//说明是相对路径 需要从当前目录查找
		char absolutePath[256];
		getAbsolutePath(absolutePath, dir);
		//进入路径
		cdAbsolute(absolutePath);
	}
}

//展示当前路径
void pwd()
{
	printf("%s\n", current->path);
}

//列出当前目录下文件
void list()
{
	showChildren(current);
}

//创建目录或者文件 根据type标识
void createFile(char* dir, int type)
{
	char absolutePath[256];
	if (dir[0] == '/')//说明以绝对路径方式创建
	{
		int i = 0;
		while (dir[i] != 0)
		{
			absolutePath[i] = dir[i];
			++i;
		}
		absolutePath[i] = 0;//结束标识
	}
	else
	{
		getAbsolutePath(absolutePath, dir);
	}
	//此时得到要创建的是形如/usr/local/temp的绝对路径 /usr/local/为已经存在的路径,要创建temp文件,所以要把他们分隔开
	int length = 0;
	while (absolutePath[length] != 0)  ++length;//先得到绝对路径的长度,方便反向查询最后一个/,即要添加文件夹的路径
	char path[256];
	char fileName[256];
	while (length >= 0)//得到最后一个/的位置
	{
		if (absolutePath[length] == '/') break;
		--length;
	}
	int index = 0;
	while (index < length)//读取要建立文件夹的路径 如上述的/usr/local
	{
		path[index] = absolutePath[index];
		++index;
	}
	path[index] = 0;
	index = 0;
	++length;//忽略最后一个/
	while (absolutePath[length] != 0)//读取到要创建的文件夹名称,例如temp
	{
		fileName[index] = absolutePath[length];
		++length;
		++index;
	}
	fileName[index] = 0;

	if (path[0] == 0)//即为根路径,不需要重新进入
	{
		path[0] = '/';
		path[1] = 0;
	}
	else
	{
		//进入刚刚得到的目录 例如:/usr/local
		cdAbsolute(path);
	}
	//printf("%s,%s\n", path, fileName);

	//FCB* son = malloc(sizeof(FCB));
	FCB* son = &fcbs[fcbindex++];

	int i = 0;
	while (fileName[i] != 0)  // 写入路径
	{
		son->fileName[i] = fileName[i];
		++i;
	}
	son->fileName[i] = 0;  // 结束符

	i = 0;
	while (absolutePath[i] != 0)  // 写入路径
	{
		son->path[i] = absolutePath[i];
		++i;
	}
	son->path[i] = 0;  // 结束符
	son->size = 0;

	//创建文件或者目录只是这里有点区别
	if (type == DIR)
	{
		son->fileType = DIR;

		//调用Oranges’代码样例中的open实现创建目录
		int fd = open(absolutePath, O_RDWR);
		if (fd == -1) mkdir(absolutePath);
	}
	else
	{
		son->fileType = FILE;

		int fd = open(absolutePath, O_RDWR);

		if (fd == -1)
		{
			//调用Oranges’代码样例中的open实现创建文件
			fd = open(absolutePath, O_CREAT | O_RDWR);

			char buf[1] = { 0 };
			write(fd, buf, 1);
			close(fd);
		}
	}

	addChildren(current, son);
}

//删除文件
void rm(char* fileName)
{
	//只能删除当前目录下文件
	if (strcmp(current->children->fileName, fileName) == 0)//说明子目录第一个就是要删除的
	{
		current->children = current->children->next;
	}
	else
	{
		int i = 0;
		for (FCB* x = current->children; i < current->size; x = x->next)
		{
			if (strcmp(x->next->fileName, fileName) == 0)
			{
				x->next = x->next->next;
				break;
			}
			++i;
		}
	}
	current->size -= 1;

	char absolutePath[256];
	getAbsolutePath(absolutePath, fileName);
	unlink(absolutePath);//调用底层删除
}
//写文件
void writeFile(char* fileName)
{
	char absolutePath[256];
	getAbsolutePath(absolutePath, fileName);

	int fd = open(absolutePath, O_RDWR);
	if (fd == -1) return;

	char tty_name[] = "/dev_tty0";
	int fd_stdin = open(tty_name, O_RDWR);
	if (fd_stdin == -1) return;
	char writeBuf[1024];  // 写缓冲区
	int endPos = read(fd_stdin, writeBuf, 1024);
	writeBuf[endPos] = 0;
	write(fd, writeBuf, endPos + 1);  // 结束符也应写入
	close(fd);
}
//读文件
void readFile(char* fileName)
{
	char absolutePath[256];
	getAbsolutePath(absolutePath, fileName);
	int fd = open(absolutePath, O_RDWR);
	if (fd == -1) return;

	char buf[1024];
	int n = read(fd, buf, 1024);
	if (n == -1)  // 读取文件内容失败
	{
		close(fd);
		return;
	}

	printf("%s\n", buf);
	close(fd);
}

//操作系统启动时运行,加载磁盘文件信息
//将磁盘中数据以约定的形式取出,重构目录树
//存储在myConfig文件下
void load()
{
	//重构目录树
	//int fd = open("/myConfig", O_RDWR);
	//if (fd == -1) return;

	//char files[2048];
	//int n = read(fd, files, 2048);
	//if (n == -1)  // 读取文件内容失败
	//{
	//	close(fd);
	//	return;
	//}
	//close(fd);
	//printf("%s", files);

	//printf("%s", files);
	////char files[2048]="tongji4m3,d xuanke,d README.md,f #temp,d me.jpg,f hello.java,f temp1.txt,f #root,d myFiles,d passwd,f bochs.c,f #usr,d tomcat,d java,d local,d #";
	////printf("%s", files);

	char files[2048] = "tongji4m3,d xuanke,d README.md,f #temp,d me.jpg,f hello.java,f temp1.txt,f #root,d myFiles,d passwd,f bochs.c,f #usr,d tomcat,d java,d local,d #";


	char fileName[2048];

	int i = 0;
	int j = 0;
	int height = 2;//标识是第二级目录
	while (files[j] != 0)
	{
		current;
		if (files[j] == '#') //到了下一个二级目录
		{
			height = 2;
			++j;
		}
		else if (files[j] == ',') //文件名读取完成
		{
			fileName[i] = 0;

			if (height == 2)//说明是为根目录创建的
			{
				cdRoot();
			}

			++j;//此时读到d或者f,即文件类型
			if (files[j] == 'd')//是目录
			{
				createFile(fileName, DIR);
			}
			else//是文件
			{
				createFile(fileName, FILE);
			}
			//只有二级目录才进入,其他不进入
			if (height == 2) cd(fileName);//进入该二级目录,在该目录下新建文件

			height = 3;//读完二级目录后,剩下的都是三级的文件
			++j;//读完了d
			++j;//跳过了空格 
			i = 0;//重新读取文件
		}
		else
		{
			fileName[i++] = files[j++];
		}
	}
	cdRoot();//回到根目录
	printf("load successful!\n");
}

//height标识了存储第几级别的文件
int serialize(char* files, FCB* father, int i, int height)
{
	int index = 0;
	for (FCB* x = father->children; index < father->size; x = x->next)
	{
		int j = 0;//存储文件名
		while (x->fileName[j] != 0) files[i++] = x->fileName[j++];


		files[i++] = ',';//用,分割同一个文件的内容

		if (x->fileType == DIR) files[i++] = 'd';//存储文件类型
		else files[i++] = 'f';

		files[i++] = ' '; //用' '分隔文件

		//继续存储下一级的文件
		if (x->fileType == DIR && height == 2) i = serialize(files, x, i, height + 1);
		else if (height == 2) ++height;//说明是二级文件

		++index;
	}
	if (height == 3) files[i++] = '#';//三级文件遍历完用#隔开
	return i;
}

//操作系统关机时,保存磁盘文件信息
//将目录树对应的FCB文件以特定形式序列化存储在磁盘中
void store()
{
	char files[2048];//存储序列化信息
	int i = 0;
	cdRoot();//先进入到根目录

	//根目录是第一级,所以存储就从第二级开始
	//目前只支持三级文件的存储
	i = serialize(files, current, i, 2);//递归地将文件序列化到files里 
	files[i++] = 0;

	//先删除原本的myConfig
	unlink("/myConfig");//调用底层删除

	//调用Oranges’代码样例中的open实现创建文件
	int fd = open("/myConfig", O_CREAT | O_RDWR);

	if (fd == -1) return;

	write(fd, files, i);  //写入序列化数据
	close(fd);
	printf("store successful!\n");
}


//----------------------------------文件管理代码结束处-----------------------------------------

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
	char fileName[64];//fileName

	
	clear();//清空控制台
	init();//初始化current为根目录
	//load();//加载磁盘本来就存在的文件
	load();
	
	help();//显示帮助文档

	//文件系统不断循环等待用户响应
	while (1)
	{
		printf("[root@localhost %s]# ", current->path);//模拟linux的命令行交互

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

		if (rdbuf[pointer] != 0)  // 指令后有参数
		{
			pointer++;
			int len = pointer;
			while (rdbuf[pointer] != 0 && rdbuf[pointer] != ' ')  //读取fileName 例如mkdir /root/bochs 则读取到/root/bochs
			{
				fileName[pointer - len] = rdbuf[pointer];
				pointer++;
			}
			fileName[pointer - len] = 0;//读取完了第一个参数
		}

		//根据输入的不同指令,执行不同的操作
		//有些指令带文件名
		if (strcmp(command, "help") == 0) help();
		else if (strcmp(command, "fileHelp") == 0) fileHelp();
		else if (strcmp(command, "clear") == 0) clear();
		else if (strcmp(command, "pwd") == 0) pwd();
		else if (strcmp(command, "ls") == 0) list(current->path);
		else if (strcmp(command, "cd") == 0) cd(fileName);
		else if (strcmp(command, "rm") == 0) rm(fileName);
		else if (strcmp(command, "mkdir") == 0) createFile(fileName,DIR);
		else if (strcmp(command, "touch") == 0) createFile(fileName,FILE);
		else if (strcmp(command, "vim") == 0) writeFile(fileName);
		else if (strcmp(command, "cat") == 0) readFile(fileName);
		else if (strcmp(command, "store") == 0) store();//手动将磁盘文件保存
		else if (strcmp(command, "ls1") == 0) ls(current->path);
		else notFound();
	}
}

/*======================================================================*
							   TestB
 *======================================================================*/
void TestB() { spin("TestB"); }



/*****************************************************************************
 *                                TestC
 *****************************************************************************/
void TestC() { spin("TestC"); }



/*****************************************************************************
 *                                help
 *                              帮助文档
 *****************************************************************************/
void help()
{
	printf("-----------help document------------");
	printf("1.help----help document\n");
	printf("2.fileHelp----fileSystem help document\n");
	printf("3.clear----clear the terminal\n");
	printf("------------------------------------");

}

void fileHelp()
{
	printf("-----------fileSystem help document------------");
	printf("fileHelp");
	printf("format");
	printf("pwd");
	printf("cd fileName");
	printf("mkdir fileName");
	printf("rm fileName");
	printf("ls");
	printf("touch fileName");
	printf("open fileName");
	printf("createFile fileName");
	printf("close fileName");
	printf("vim fileName");
	printf("cat fileName");
	printf("delete fileName");
	printf("showDirBlocks");
	printf("showFileBlocks");
	printf("showOpenFiles");
	printf("------------------------------------");
}

void notFound()
{
	printf("command not found!");
	printf("input help to get command");
}


/*****************************************************************************
 *                                clear
 *                           清空控制台显示
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

