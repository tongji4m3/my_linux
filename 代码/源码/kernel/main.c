
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

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
#define LINE 15



//----------------------------------应用级游戏五子棋开始处-----------------------------------------
void draw(int arr[LINE][LINE]);
int check(int arr[LINE][LINE],int x,int y);
int win(int arr[LINE][LINE]);

void draw(int arr[LINE][LINE])
{
	int i, j;

	printf("  1 2 3 4 5 6 7 8 9 A B C D E F\n");
	
	for(i = 0; i < LINE; i++)
	{
		printf("%X", i + 1);
		for(j = 0; j < LINE; j++)
		{
			switch(arr[i][j])
			{
				case 0 : printf(" +"); break;
				case 1 : printf(" O"); break;
				case 2 : printf(" X"); break;
			}
		}
		printf("\n");
	}
}

int check(int arr[LINE][LINE],int x,int y)
{	
	
	if(x>=1&&x<=LINE&&y>=1&&y<=LINE)
	{
		if(arr[x-1][y-1]!=0){
			return 0;
		}
		else return 1;
	}
	else if(x<1||x>LINE||y<1||y>LINE)
	{
			return 0;
	}
}

int win(int arr[LINE][LINE])
{	
	int i,j;
	for(i=0;i < LINE; i++)
	{
		for(j = 0; j < LINE-4; j++){
	
			if(arr[i][j]==arr[i+1][j+1]&&
				arr[i+1][j+1]==arr[i+2][j+2]&&
			    arr[i+2][j+2]==arr[i+3][j+3]&&
			    arr[i+3][j+3]==arr[i+4][j+4]&&
				arr[i][j]!=0)                   //向右倾斜的斜线上连成五子
				{
					return arr[i][j];
				}
			else if(arr[i][j]==arr[i-1][j+1]&&
	    		     arr[i-1][j+1]==arr[i-2][j+2]&&
					 arr[i-2][j+2]==arr[i-3][j+3]&&
					 arr[i-3][j+3]==arr[i-4][j+4]&&
					 arr[i][j]!=0)              //向左倾斜的斜线上连成五子
					 {
					     return arr[i][j];
					 }
			else if(arr[i][j]==arr[i][j+1]&&
	   		    	 arr[i][j+1]==arr[i][j+2]&&
					 arr[i][j+2]==arr[i][j+3]&&
					 arr[i][j+3]==arr[i][j+4]&&
					 arr[i][j]!=0)              //横线上连成五子
					 {
					 	return arr[i][j];
					 }                        
			else if(arr[i][j]==arr[i+1][j]&&
	    			 arr[i+1][j]==arr[i+2][j]&&
					 arr[i+2][j]==arr[i+3][j]&&
					 arr[i+3][j]==arr[i+4][j]&&
					 arr[i][j]!=0)              //竖线上连成五子
					 {
					 	return arr[i][j];
					 }
			}
	}
}
int startFive(int fd_stdin)
{
	int arr[LINE][LINE] = {0};//用于存储五子棋的棋盘
	int x = 0, y = 0;//用于录入下棋的坐标
	int flag = 1;//flag标记玩家
	int a = 0;//存放win()的返回值
	int b = 0;//存放check的返回值
	do
	{
	 	//system("clear");
	 	clear();
		draw(arr);//通过draw函数画棋盘
	    printf("Player %d > ", flag);
	    char command[64];//得到读取到的命令
	    //与用户交换相关
		char rdbuf[512];//读缓冲区
		
		//一次从流中读取512字节放到缓冲区里面
		int r = read(fd_stdin, rdbuf, 512);
		rdbuf[r] = 0;//r为字符串实际的结尾位置,这样标识了字符串的结束位置
		
		//取出用户输入的指令
		int pointer = 0;
		
		

		while (rdbuf[pointer] != 0 && rdbuf[pointer] != ' ')
		{
			command[pointer] = rdbuf[pointer];
			pointer++;
		}
		command[pointer] = 0;
		if (strcmp(command, "exit") == 0) 
		{
		clear();
		return;
		}
		
		
		//例如A A,1 1
		char xx,yy;
		xx=rdbuf[0];
		yy=rdbuf[2];
		//printf("%c %c",xx,yy);
		
		
	
		if(xx=='1')x=1;
		else if(xx=='2')x=2;
		else if(xx=='3')x=3;
		else if(xx=='4')x=4;
		else if(xx=='5')x=5;
		else if(xx=='6')x=6;
		else if(xx=='7')x=7;
		else if(xx=='8')x=8;
		else if(xx=='9')x=9;
		else if(xx=='A')x=10;
		else if(xx=='B')x=11;
		else if(xx=='C')x=12;
		else if(xx=='D')x=13;
		else if(xx=='E')x=14;
		else if(xx=='F')x=15;
		else
		{
			printf("Wrong,Please input again");
		}
		
		if(yy=='1')y=1;
		else if(yy=='2')y=2;
		else if(yy=='3')y=3;
		else if(yy=='4')y=4;
		else if(yy=='5')y=5;
		else if(yy=='6')y=6;
		else if(yy=='7')y=7;
		else if(yy=='8')y=8;
		else if(yy=='9')y=9;
		else if(yy=='A')y=10;
		else if(yy=='B')y=11;
		else if(yy=='C')y=12;
		else if(yy=='D')y=13;
		else if(yy=='E')y=14;
		else if(yy=='F')y=15;
		else
		{
			printf("Wrong, please play chess again");
		}
		printf("%d %d",x,y);
		//scanf("%d%d", &x, &y);
		check(arr,x,y);//判断下棋的位置是否是重复位置或者下出棋盘
		b =check(arr,x,y);
		if(b==0){	
		printf("%d",arr[x-1][y-1]);
			printf("Please play chess again.");
			continue;
		}
		arr[x - 1][y - 1] = flag;
		win(arr);//判断是否能赢
		a =win(arr);
		if(a==1)	printf("Player1  win.");
		else if(a==2)	printf("Player2  win.");
		
		flag ^= 3;
		
		
		
	}while(1);

	return 0;
}
//----------------------------------应用级游戏五子棋代码结束处-----------------------------------------

//----------------------------------应用级游戏扫雷代码开始处-----------------------------------------
char show_mine[22][22];
int real_mine[22][22];
int flag[22][22];
void init_mine(int row, int col)//初始化两个雷阵
{
    int i, j;

    for (i = 0; i < row; i++) 
    {
        for (j = 0; j < col; j++)
        {
            show_mine[i][j] = '*';//用户雷阵 
            real_mine[i][j] = '0';//设计雷阵 
            
        }
    }
}

void init_flag(int row, int col)//初始化DFS标识，防止重复深搜 
{
	int i, j;

    for (i = 0; i < row; i++) 
    {
        for (j = 0; j < col; j++)
        {
            if(i == 0||i == row-1||j == 0||j == col-1)
            {
            	flag[i][j] = '1';
			}
            else
            {
            	flag[i][j] = '0';
			}
        }
    }
}
void print_player(int row, int col)//打印玩家棋盘
{
    int i = 0;
    int j = 0;
    printf("0  ");
    for (i = 1; i <row-1; i++)
    {
    	if(i < 10)
    	{
    		printf("%d  ", i);
		}
        else
        {
        	printf("%d ", i);
		};//打印横标（1--10）
    }
    printf("\n");
    for (i = 1; i <row-2; i++)
    {
    	if(i < 10)
    	{
    		printf("%d  ", i);
		}
        else
        {
        	printf("%d ", i);
		};//打印竖标（1--10）
		
        for (j = 1; j < col-1; j++)
        {
            printf("%c  ", show_mine[i][j]);//玩家棋盘数组
        }
        printf("\n");
    }
    printf("%d ",row-2);//开始打印最后一行
    for (i = 1; i < row-1; i++)
    {
    	printf("%c  ", show_mine[row-2][i]);
    }
    printf("\n");
}

void print_mine(int row, int col)//打印设计棋盘
{
    int i = 0;
    int j = 0;
    printf("0  ");
    for (i = 1; i <row - 1; i++)
    {  
//打印横标（0--10）
    	if(i < 10)
    	{
    		printf("%d  ", i);
		}
        else
        {
        	printf("%d ", i);
		};
    }
    printf("\n");
    for (i = 1; i <row - 2; i++)
    {
        if(i < 10)
    	{
    		printf("%d  ", i);
		}
        else
        {
        	printf("%d ", i);
		};//打印竖标（1--10）
        for (j = 1; j < col - 1; j++)
        {
            printf("%c  ", real_mine[i][j]);
        }
        printf("\n");
    }
    printf("%d ",col-2);//开始打印最后一行
    for (i = 1; i < row - 1; i++)
    {
        printf("%c  ", real_mine[row-2][i]);	
    }
    printf("\n");
}

void set_mine(int row, int col)//给设计者棋盘布雷
{
    int x = 0;
    int y = 0;
    int count = 40;//雷总数
    row = row - 2;
    col = col - 2;
    while (count)//雷布完后跳出循环
    {
        int x = (int)((get_ticks() * 314159 / 7) % row) + 1;//产生1到row的随机数，在数组下标为1到row-2的范围内布雷
        int y = (int)((get_ticks() * 314159 / 13) % col) + 1;//产生1到col的随机数，在数组下标为1到col-2的范围内布雷
        if (real_mine[x][y] == '0')//找不是雷的地方布雷
        {
            real_mine[x][y] = '1';
            count--;
        }
    }
}

int count_mine(int x, int y)//检测周围8个区域雷的个数
{
    int count = 0;
    int x3[] = {x - 1, x , x + 1};
    int y3[] = {y - 1, y , y + 1};
    int i ,j;
    for(i = 0; i < 3; i++)
    {
    	for(j = 0; j < 3; j++)
    	{
    		if(real_mine[x3[i]][y3[j]] == '1')
    		{
    			count++;
			}
		}
	}

    return count;
}
void open_mine_dfs(int x, int y)//空白区炸开函数 DFS 
{
	if(x<1||x>20||y<1||y>20)return;
	if(flag[x][y] == '0')
	{
		flag[x][y] = '1';
		if(show_mine[x][y] == '0')
		{
			int x3[] = {x - 1, x , x + 1};
    		int y3[] = {y - 1, y , y + 1};
    		int i ,j;
    		for(i = 0; i < 3; i++)
    		{
    			for(j = 0; j < 3; j++)
    			{
    		
    				int rowx = x3[i];
    				int coly = y3[j];
    				show_mine[rowx][coly] = count_mine(rowx, coly) + '0';//显示该坐标周围雷数
    				if(flag[rowx][coly] == '0' && show_mine[rowx][coly] == '0')
    				{
    					open_mine_dfs(x3[i], y3[j]);
					}
    				
					
				}
			}
		}
		
	}
	return;

}

int count_show_mine(int row, int col)//判断剩余未知区域的个数，个数为雷数时玩家赢
{
    int count = 0;
    int i = 0;
    int j = 0;
    for (i = 1; i <= row - 2; i++)
    {
        for (j = 1; j <= col - 2; j++)
        {
            if (show_mine[i][j] == '*')
            {
                count++;
            }
        }

    }
    return count;
}


void startSwipe(int fd_stdin)//扫雷游戏
{
	int i,j,time;
	int wix,wiy;
	int row = 22;
	int col = 22; 

	char temp1[64];
	char temp2[64];


	do 
	{
		
		int result = 0;
		init_mine(row,col);//初始化玩家棋盘和设计者棋盘
		set_mine(row,col);//给设计者棋盘布雷
		clear();
		print_player(row,col);//打印玩家棋盘
		while(1)
		{
		int x = 0;
    		int y = 0;
    		int count = 0;
    
    		init_flag(row,col);
    	
    		printf("Please input row and col number(restart:-1)\n");
    		
    		for(wix = 1;wix <= 2;)
		{
			//scanf("%d,%d",&row,&colum);
			char rdbuf[512];//读缓冲区
	    		char command[64];//得到读取到的命令
			//一次从流中读取512字节放到缓冲区里面
			int r = read(fd_stdin, rdbuf, 512);
			rdbuf[r] = 0;//r为字符串实际的结尾位置,这样标识了字符串的结束位置
			int pointer = 0;
			while (rdbuf[pointer] != 0 && rdbuf[pointer] != ' ')
			{
				command[pointer] = rdbuf[pointer];
				pointer++;
			}
			command[pointer] = 0;
			if (strcmp(command, "exit") == 0) 
			{
			clear();
			return;
			}
			//取出用户输入的指令
			x = 0;
    			y = 0;
			pointer = 0;
			for (int i = 0;; i++)
			{
				if(rdbuf[pointer]==' ')
				break;
				x= x*10+rdbuf[pointer] - '0';
				pointer++;
			}
			//printf("%d",rdbuf[0]);
			pointer++;
			for (int i = 0;; i++)
			{
				if(rdbuf[pointer]==',')
				break;
				y= y*10+rdbuf[pointer] - '0';
				pointer++;
			}
			//printf("%d %d",x,y);
			if (x == -1 || y == -1)
			{
				break;
			}
			else
			{
				if ((x >= 1 && x <= row-2) && (y >= 1 && y <= col-2))//判断输入坐标是否有误，输入错误重新输入
    				{
					if (real_mine[x][y] == '0')//没踩到雷
					{
		    				char ch = count_mine(x,y);
		    				show_mine[x][y] = ch + '0';//数字对应的ASCII值和数字字符对应的ASCII值相差48，即'0'的ASCII值
		    				open_mine_dfs(x, y);
					}
					else if (real_mine[x][y]=='1')//踩到雷
					{
		    				result = 1;
					}
					break;

    				}
    				else
    				{
					printf("input error!\n");
					wix = 2;
    				}

			}

		}
			
		if (count_show_mine(row,col) == 40)//若玩家棋盘的'*'个数为雷数时，扫雷完成，游戏胜利
		{
			clear();
		    	print_mine(row,col);//打印设计者棋盘
		    	printf("player win!\n");
	//            finish = clock();//取结束时间
	//            printf("spend %d s\n",(int) (finish - start) / CLOCKS_PER_SEC);
		    	break;
		}
        	if (result)//判断是否踩到雷
        	{
        		clear();
            	
//            	finish = clock();//取结束时间
//            	printf("spend %d s\n", (int)(finish - start) / CLOCKS_PER_SEC);
            	print_mine(row,col);//打印设计者雷阵查看雷的分布
            	printf("you died.\n");

            	break;
        	}
        	clear();
			print_player(row,col);//打印玩家棋盘
		}

		printf("------Now You Will Enter The Next Game----\n");
		for(time = 0;time < 10;time++)
		{
			milli_delay(500);
			printf("=====>");
		}
		

	} while (1);
	//KillProcess(3);
	
}

/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{
	disp_str("-----\"kernel_main\" begins-----\n");

	struct task* p_task;
	struct proc* p_proc= proc_table;
	char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
	u16   selector_ldt = SELECTOR_LDT_FIRST;
        u8    privilege;
        u8    rpl;
	int   eflags;
	int   i, j;
	int   prio;
	for (i = 0; i < NR_TASKS+NR_PROCS; i++) {
	        if (i < NR_TASKS) {     /* 任务 */
                        p_task    = task_table + i;
                        privilege = PRIVILEGE_TASK;
                        rpl       = RPL_TASK;
                        eflags    = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1 */
			prio      = 15;
                }
                else {                  /* 用户进程 */
                        p_task    = user_proc_table + (i - NR_TASKS);
                        privilege = PRIVILEGE_USER;
                        rpl       = RPL_USER;
                        eflags    = 0x202; /* IF=1, bit 2 is always 1 */
			prio      = 5;
                }

		strcpy(p_proc->name, p_task->name);	/* name of the process */
		p_proc->pid = i;			/* pid */
		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
		       sizeof(struct descriptor));
		p_proc->ldts[0].attr1 = DA_C | privilege << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
		       sizeof(struct descriptor));
		p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;
		p_proc->regs.cs	= (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ds	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.es	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.fs	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ss	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = eflags;

		/* p_proc->nr_tty		= 0; */

		p_proc->run_state = 1;
		p_proc->run_count = 0;
		p_proc->p_flags = 0;
		p_proc->p_msg = 0;
		p_proc->p_recvfrom = NO_TASK;
		p_proc->p_sendto = NO_TASK;
		p_proc->has_int_msg = 0;
		p_proc->q_sending = 0;
		p_proc->next_sending = 0;

		for (j = 0; j < NR_FILES; j++)
			p_proc->filp[j] = 0;

		p_proc->ticks = p_proc->priority = prio;

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

        /* proc_table[NR_TASKS + 0].nr_tty = 0; */
        /* proc_table[NR_TASKS + 1].nr_tty = 1; */
        /* proc_table[NR_TASKS + 2].nr_tty = 1; */

	k_reenter = 0;
	ticks = 0;

	p_proc_ready	= proc_table;

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
	int fd = open("/myConfig", O_RDWR);
	if (fd == -1) return;

	char files[2048];
	int n = read(fd, files, 2048);
	if (n == -1)  // 读取文件内容失败
	{
		close(fd);
		return;
	}
	close(fd);
	printf("%s", files);

	//printf("%s", files);
	//char files[2048]="tongji4m3,d xuanke,d README.md,f #temp,d me.jpg,f hello.java,f temp1.txt,f #root,d myFiles,d passwd,f bochs.c,f #usr,d tomcat,d java,d local,d #";
	//printf("%s", files);

	//char files[2048] = "tongji4m3,d xuanke,d README.md,f #temp,d me.jpg,f hello.java,f temp1.txt,f #root,d myFiles,d passwd,f bochs.c,f #usr,d tomcat,d java,d local,d #";


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
		else if (height == 2) files[i++] = '#';//说明是二级文件

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





//----------------------------------管理牧场代码开始处-----------------------------------------

/*
* 管理牧场小游戏
* 
小程序要求:
农夫要修理牧场的一段栅栏，他测量了栅栏，发现需要N块木头，每块木头长度为整数Li个长度单位，
于是他购买了一个很长的，能锯成N块的木头，即该木头的长度是Li的总和。

但是农夫自己没有锯子，请人锯木的酬金跟这段木头的长度成正比。
为简单起见，不妨就设酬金等于所锯木头的长度。
例如，要将长度为20的木头锯成长度为8，7和5的三段，
第一次锯木头将木头锯成12和8，花费20；
第二次锯木头将长度为12的木头锯成7和5花费12，总花费32元。
如果第一次将木头锯成15和5，则第二次将木头锯成7和8，那么总的花费是35（大于32）.

输入格式：输入第一行给出正整数N（N<104），表示要将木头锯成N块。第二行给出N个正整数，表示每块木头的长度。

输出格式：输出一个整数，即将木头锯成N块的最小花费。


小程序实现思路:
通过反向思考,如果每次对半分的时候,最后结果花费最小，
那么,在只有碎片的时候,每一次都拿出最小的两个进行拼凑，就是对半分的逆过程了，
所以只需要有一个每次都能取出最小元素的优先队列即可，所有就只要构建一个优先队列就能解决问题了。

堆实现思路:
设计一个每次都能取出最小元素的优先队列即可。
总体思路是，每一次往优先队列加入元素时，把元素放在尾部，然后让这个元素“上浮”到合适的位置,构成一个堆,堆顶为最小元素。
每次取出元素时，取出堆顶元素，然后让堆顶元素与最后一个元素交换，并且让堆的数量减一，堆顶元素“下沉”。
这样，不管是取出元素还是加入元素，都构成了一个堆。
*/

//实现一个堆
typedef struct MinPQ
{
	int N;//元素个数,从1开始
	int pq[1000];
} MinPQ;

//定义一个指向堆的指针
MinPQ* minPQ;

//交换堆数组中两个元素的值
void  exch(int i, int j)
{
	int temp = minPQ->pq[i];
	minPQ->pq[i] = minPQ->pq[j];
	minPQ->pq[j] = temp;
}

//让指定位置堆元素上浮,使得堆仍然有序
void swim(int k)
{
	while (k > 1 && minPQ->pq[k] < minPQ->pq[k / 2])
	{
		exch(k, k / 2);
		k /= 2;
	}
}

//让指定位置堆元素下沉,使得堆仍然有序
void sink(int k)
{
	while (2 * k <= minPQ->N)
	{
		int j = 2 * k;
		if (j + 1 <= minPQ->N && minPQ->pq[j] > minPQ->pq[j + 1])
		{
			++j;
		}
		if (minPQ->pq[k] <= minPQ->pq[j])
		{
			break;
		}
		exch(k, j);
		k = j;
	}
}

//删除堆最小的元素
int delMin()
{
	int max = minPQ->pq[1];
	exch(1, minPQ->N--);
	sink(1);
	return max;
}

//往堆中添加元素
void push(int i)
{
	minPQ->pq[++(minPQ->N)] = i;
	swim(minPQ->N);
}


//等价于用n个元素构建一个二叉树,n个元素在树的最底层
// 并且使得除了root外,其他节点相加最小

/*
 * 反过来思考,如果每次对半分的时候,最后结果最小
 * 那么,在只有碎片的时候,每一次都拿出最小的两个进行拼凑
 * 就是对半分的逆过程了
 * 所以只需要有一个每次都能取出最小元素的优先队列即可
 */
void manageRanch(int fd_stdin)
{
	//输入正整数N(分成N块,N<1024),及N个正整数(每块长度) 格式为形如:8:4,5,1,2,1,3,1,1
	printf("input N(size) and each of values(length of each block)\n");
	printf("example:  8:4,5,1,2,1,3,1,1 \n");
	

	//与用户交换相关
	char rdbuf[512];//读缓冲区

	//一次从流中读取512字节放到缓冲区里面
	int r = read(fd_stdin, rdbuf, 512);
	rdbuf[r] = 0;//r为字符串实际的结尾位置,这样标识了字符串的结束位置

	//取出用户输入的指令
	int pointer = 0;

	int N = rdbuf[pointer++]-'0'; //读N 如:8:4,5,1,2,1,3,1,1的8

	int sum = 0;
	MinPQ temp = { 0 };
	minPQ = &temp;
	for (int i = 0; i < N; ++i)
	{
		++pointer;
		int len= rdbuf[pointer] - '0';
		++pointer;
		push(len);
	}
	while (minPQ->N >= 2)
	{
		int min1 = delMin();
		int min2 = delMin();
		sum += min1 + min2;
		push(min1 + min2);
	}
	printf("sum:%d\n", sum);
}
//----------------------------------管理牧场代码结束处-----------------------------------------










//----------------------------------内存管理代码开始处-----------------------------------------
//内存空间管理
struct Memory {

	// 内存大小为640K
	int totalMemory;
	int size;
	int position;//用来记录DividedRecord的元素位置
	struct Block* first;//以节点Block形式建立一个链表（数组）,first指向头节点
	int Block_size;
	struct Record* head;//记录已分配的内存块
	int Record_size;
};
struct Memory M1;

struct Block {
	int head;
	int size;
	int jobId;
	int IsFree;
	struct Block* next;
};
struct Block* BK1;

struct Block blocks[50];
int block_index = 0;

struct Block* initBlock(int head, int size, int jobId) {

	BK1->head = head;
	BK1->size = size;
	BK1->jobId = jobId;
	BK1->IsFree = 1;
	M1.Block_size++;
	return BK1;
};

struct Block* newBlock(int head, int size, int jobId) {
	struct Block* BK2 = &blocks[block_index++];//初始化新建单个内存块
	BK2->head = head;
	BK2->size = size;
	BK2->jobId = jobId;
	BK2->IsFree = 1;
	return BK2;
};


struct Record {
	int head;
	int size;
	int JobId;
	struct Record* next;
};
struct Record* RD1, *RD2;

struct Record records[50];
int record_index = 0;

struct Record* initRecord(int head, int size, int JobId) {
	RD1->head = head;
	RD1->size = size;
	RD1->JobId = JobId;
	M1.Record_size++;
	return RD1;
};


struct Record* newRecord(int head, int size, int JobId) {
	RD2 = &records[record_index++];//初始化新建单个记录块
	RD2->head = head;
	RD2->size = size;
	RD2->JobId = JobId;
	return RD2;
};

//添加子节点
void addChildrenBlock(int location, struct Block *son)
{
	int position = 0;
	struct Block* head = M1.first;
	while ((position < location) && (position < M1.Block_size)) {
		position++;
		head = head->next;
	}
	if (position == M1.Block_size - 1) {
		head->next = son;
	}
	else {
		son->next = head->next;
		head->next = son;
	}
	M1.Block_size++;
}

void addChildrenRecord(struct Record* son)
{
	int position = 0;
	struct Record* head = M1.head;
	while (position < M1.Record_size - 1) {
		position++;
		head = head->next;
	}
	head->next = son;
	M1.Record_size++;
}

void deleteBlock(int location)
{
	struct Block* head = M1.first;
	int position = 0;
	if (location != 0) {
		while (position < location - 1) {
			position++;
			head = head->next;
		}
		head->next = head->next->next;
	}
	else {
		M1.head = head->next;
	}
	M1.Block_size--;
}

void deleteRecord(int location)
{
	struct Record* head = M1.head;
	int position = 0;
	if (location != 0) {
		while (position < location - 1) {
			position++;
			head = head->next;
		}
		head->next = head->next->next;
	}
	else {
		M1.head = head->next;
	}
	M1.Record_size--;
}

struct Block* getBlock(int location) {
	struct Block* temp1 = M1.first;
	int position = 0;
	while (position < location) {
		temp1 = temp1->next;
		position++;
	}
	return temp1;
}

struct Record* getRecord(int location) {
	struct Record* temp1 = M1.head;
	int position = 0;
	while (position < location) {
		temp1 = temp1->next;
		position++;
	}
	return temp1;
}



struct Memory initMemory() {
	M1.totalMemory = 640;
	M1.size = M1.totalMemory;
	M1.position = 0;
	M1.Block_size = 0;
	M1.Record_size = 0;
	BK1 = &blocks[block_index++];//初始化Block链表头
	RD1 = &records[record_index++];//初始化Record链表头
	M1.first = initBlock(0, M1.size, 0);
}


void DoAllocation(int size, struct Block* block, int jobId, int location) {
	// 如果当前分区较大
	if (block->size > size) {
		struct Block* split = newBlock(block->head + size, block->size - size, 0);//更新内存块的信息
		addChildrenBlock(location, split);//将新的内存块加入到数组
	}
	block->size = size;//内存块缩小，且变为不可用
	block->IsFree = 0;
	block->jobId = jobId;
	//更新记录数组
	if (M1.head == 0) {
		struct Record* temp2 = initRecord(block->head, block->size, block->jobId);
		M1.head = temp2;
	}
	else {
		struct Record* temp2 = newRecord(block->head, block->size, block->jobId);
		addChildrenRecord(temp2);
	}
	M1.position += 1;
}


//首次适应算法
int FristFit(int size, int jobId) {
	int location = 0;
	// 遍历分区链表
	int position = 0;
	struct Block* head = M1.first;
	for (location = 0; location < M1.Block_size; location++) {
		// 分配成功
		struct Block* temp = getBlock(location);
		if (temp->IsFree && (temp->size >= size)) {
			DoAllocation(size, temp, jobId, location);
			return 1;
		}
	}
	// 分配失败
	return 0;
}

//最佳适应算法
int BestFit(int size, int jobId) {
	int flag = -1;
	int min = M1.size + 1;//作为一个临时值
	struct Block* bestBlock = newBlock(0, 0, 0);
	int location = 0;
	int i = 0;
	// 遍历分区链表
	for (location = 0; location < M1.Block_size; location++) {
		struct Block* temp = getBlock(location);
		if (temp->IsFree && (temp->size >= size)) {
			if (min > temp->size) {
				min = temp->size;//一旦找到，min就会阻断之后的循环
				bestBlock = temp;
				flag = location;
			}
		}
	}
	// 是否成功分配
	if (flag != -1) {
		DoAllocation(size, bestBlock, jobId, flag);
		return 1;
	}
	else {
		return 0;
	}
}


//内存释放算法
void Re_connect(int jobId) {
	int location = 0;
	//将记录已分配资源的数组中的内存块释放
	struct Record* head1;
	head1 = M1.head;
	for (location = 0; location < M1.Record_size - 1; location++) {
		if (head1->JobId == jobId) {
			deleteRecord(location);
			break;
		}
		head1 = head1->next;

	}
	struct Block* head2 = M1.first;
	//遍历分区链表
	for (location = 0; location < M1.Block_size; location++) {

		if (head2->jobId == jobId) {
			struct Block* temp1 = getBlock(location + 1);
			//如果回收分区不是尾分区且后一个分区为空闲, 则与后一个分区合并
			if (location < M1.Block_size - 1 && temp1->IsFree == 1) {
				struct Block* next = getBlock(location + 1);
				head2->size += next->size;
				deleteBlock(location + 1);
			}
			//如果回收分区不是首分区且前一个分区为空闲, 则与前一个分区合并
			struct Block* temp2 = getBlock(location - 1);
			if (location > 0 && temp2->IsFree == 1) {
				struct Block* previous = getBlock(location - 1);
				previous->size += head2->size;
				deleteBlock(location);
				head2 = previous;
			}
			head2->IsFree = 1;
			head2->jobId = 0;
		}
		head2 = head2->next;
	}
}

int JobId[11] = { 1, 2, 3, 2, 4, 3, 1, 5, 6, 7, 6 };
int act[11] = { 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1 };//0表示申请，1表示释放
int size[11] = { 130, 60, 100, 60, 200, 100, 130, 140, 60, 50, 60 };

int *getCurrentInstruct(int i, int FitWay) {
	int temp[3] = { JobId[i], act[i], size[i] };
	if (act[i] == 0) {
		if (FitWay == 1) {
			int x = FristFit(temp[2], temp[0]);
		}
		if (FitWay == 2) {
			int x = BestFit(temp[2], temp[0]);
		}
	}
	else {
		Re_connect(temp[0]);
	}
	printf("------------------------\n");
	printf("Allocated resources：\n");
	printf("startAddress size jobId\n");

	for (int j = 0; j < M1.Record_size; j++) {
		struct Record* print_temp = getRecord(j);
		printf("%d %d %d\n", print_temp->head, print_temp->size, print_temp->JobId);
	}

	printf("------------------------\n");

	struct Block* ForDivideRecord = M1.first;

	printf("------------------------\n");
	printf("Unallocated resources：\n");
	printf("startAddress size jobId\n");
	for (int i = 0; i < M1.Block_size; i++) {
		struct Block* temp_block = getBlock(i);
		if (temp_block->IsFree) {
			printf("%d  %d  %d\n", temp_block->head, temp_block->size, temp_block->jobId);
		}
	}
	printf("------------------------\n");
	printf("\n");

	return temp;
}

void memoryAlloc(int fd_stdin) {

	char rdbuf[512];//读缓冲区
	char command[64];//得到读取到的命令
	//不能一次展示完
	//一次从流中读取512字节放到缓冲区里面
	printf("\n");
	printf("please input a number（1 for FristFit），(2 for BestFit)：");
	int r = read(fd_stdin, rdbuf, 512);
	rdbuf[r] = 0;//r为字符串实际的结尾位置,这样标识了字符串的结束位置
	//取出用户输入的指令,并且转为实际命令
	int pointer = 0;

	initMemory();
	int FitWay = rdbuf[pointer++] - '0';

	for (int i = 0; i < 11; i++) {
		int enter = read(fd_stdin, rdbuf, 512);
		if (act[i] == 0) {
			printf("task%d apply%dK\n", JobId[i], size[i]);
		}
		else {
			printf("task%d release%dK\n", JobId[i], size[i]);
		}
	}
	printf("\n");
	for (int i = 0; i < 11; i++) {
		int enter = read(fd_stdin, rdbuf, 512);
		if (act[i] == 0) {
			printf("task%d apply%dK\n", JobId[i], size[i]);
		}
		else {
			printf("task%d release%dK\n", JobId[i], size[i]);
		}
		getCurrentInstruct(i, FitWay);
	}

	//读到第一个空格的位置,即为命令 
	while (rdbuf[pointer] != 0 && rdbuf[pointer] != ' ')
	{
		command[pointer] = rdbuf[pointer];
		pointer++;
	}
	command[pointer] = 0;
	if (strcmp(command, "exit") == 0) return;
}

typedef struct PageItem
{
	int pageNumber;//页号
	int memoryNumber;//内存块号 -1表示不在内存,0-3表示在内存块中
	int status;//是否调入内存 0没调入内存,1调入内存
	int visited;//访问字段 可以记录上次访问时间,实现LRU算法
}PageItem;

//要执行的指令序列
//一共执行320条指令
int length = 320;
int sequence[320] = { 41,42,27,28,273,274,116,117,192,193,62,63,67,68,54,55,192,193,16,17,305,306,177,178,220,221,183,184,230,231,33,34,190,191,35,36,70,71,54,55,98,99,54,55,276,277,153,154,283,284,256,257,294,295,257,258,297,298,63,64,178,179,132,133,290,291,267,268,288,289,109,110,194,195,51,52,64,65,3,4,134,135,4,5,28,29,12,13,302,303,149,150,172,173,16,17,278,279,220,221,258,259,145,146,301,302,57,58,76,77,34,35,269,270,93,94,296,297,188,189,251,252,35,36,231,232,2,3,292,293,133,134,159,160,94,95,182,183,23,24,129,130,125,126,273,274,201,202,213,214,86,87,310,311,201,202,236,237,23,24,250,251,173,174,211,212,4,5,295,296,80,81,308,309,8,9,36,37,23,24,211,212,99,100,311,312,163,164,312,313,79,80,253,254,66,67,156,157,111,112,208,209,65,66,170,171,83,84,91,92,30,31,233,234,218,219,312,313,305,306,312,313,133,134,195,196,60,61,164,165,124,125,269,270,29,30,232,233,111,112,203,204,160,161,217,218,60,61,315,316,275,276,286,287,184,185,227,228,74,75,210,211,105,106,300,301,196,197,249,250,206,207,235,236,5,6,230,231,116,117,299,300,69,70,296,297,58,59,242,243,233,234,272,273,2,3,285,286,128,129,315,316,140,141,213,214,98,99,307,308,98,99,124,125,119,120,271,272,60,61 };

//得到指令序列
//通过伪随机数, 每次都得到同样的一组随机数, 便于测试与复现
/*void getSequence()
{
	//该作业有320条指令 所以指令地址应该是[0,320)
	//因为还要执行m+1,所以第m条指令应该在[0,319)之间

	//m m+1
	sequence[0] = rand() % 319;//[0-319)
	sequence[1] = sequence[0] + 1;
	for (int i = 2; i + 3 < length; i += 4)
	{
		//跳转到[0,m-1)  m1,m1+1
		int last = rand() % (sequence[i - 2] - 1);
		sequence[i] = last;
		sequence[i + 1] = last + 1;

		//为了不越界,m2应该在[m1+2,319)之间,这样m2+1才不会越界
		//所以未[0,317-m1)+m1+2
		last = (rand() % (317 - sequence[i])) + sequence[i] + 2;
		sequence[i + 2] = last;
		sequence[i + 3] = last + 1;
	}
	
	//最后两个序列没有赋值
	//就随意赋予320条指令中任意的连续两处位置
	 
	int hi = length - 1;
	sequence[hi - 1] = rand() % 319;
	sequence[hi] = sequence[hi - 1] + 1;


}
*/
//实现请求调页
void requestPage(int fd_stdin)
{
	//getSequence();//得到指令序列
	int pageFaultCount = 0;//缺页次数
	int visitCount = 0;//记录访问字段

	int pagesLength = 32;
	PageItem pages[32];//该作业共有32页

	for (int i = 0; i < pagesLength; i++)
	{
		//初始化每个页面
		PageItem temp = { i,-1,0,-1 };//每个页面索引为i 初始都不在内存中 上次访问时间为-1
		pages[i] = temp;
	}
	int memory[4];//分配给该作业的4个内存块,记录存储的页号
	//初始化没有页面在内存块中
	for (int i = 0; i < 4; i++)
	{
		memory[i] = -1;
	}

	//对于每条指令的执行,都进行展示
	for (int i = 0; i < length; i++)
	{
		printf("-----------------------------------\n");
		printf("Current sequence:%d \n", sequence[i]);

		int pageNumber = sequence[i] / 10;//该指令属于哪个页面
		PageItem* page = &pages[pageNumber];
		if (page->status == 1)//该页面在内存中
		{
			page->visited = visitCount++;//记录最近一次访问时间
			printf("Not missing pages!\n");
		}
		else //调入内存
		{
			++pageFaultCount;
			int isFree = 0;//内存块中是否有空余位置 0为无,1为有,默认无
			for (int j = 0; j < 4; j++)
			{
				if (memory[j] == -1)//内存还有空余位置,可直接存放页面
				{
					memory[j] = page->pageNumber;
					page->memoryNumber = j;
					page->status = 1;
					isFree = 1;//说明有空闲位置
					page->visited = visitCount++;//记录最近一次访问时间
					printf("Missing pages! Not need Page replacement!\n");
					break;
				}
			}
			if (!isFree)//需要调用LRU置换算法
			{
				//选取内存块中最近最久未使用的页面
				int index = 0, oldest = pages[memory[0]].visited;
				for (int j = 1; j < 4; j++)
				{
					int currentVisited = pages[memory[j]].visited;
					if (oldest > currentVisited)
					{
						index = j;
						oldest = currentVisited;
					}
				}
				//模拟把原来的块调出内存
				pages[memory[index]].status = 0;//不在内存中
				pages[memory[index]].memoryNumber = -1;//不在内存中

				//index即为需要替换的块
				memory[index] = page->pageNumber;
				page->memoryNumber = index;
				page->status = 1;
				page->visited = visitCount++;//记录最近一次访问时间
				printf("Missing pages! Need Page replacement!\n");
			}
		}
		/*if (i <= 10)
		{
			printf("\nUsed pages:  \n");
			printf("PageNumber MemoryNumber Status VisitedTime \n");
			for (int j = 0; j < pagesLength; j++)
			{
				if (pages[j].visited >= 0)
				{
					printf("     %d           %d         %d        %d\n", j, pages[j].memoryNumber, pages[j].status, pages[j].visited);
				}
			}
		}*/


		printf("BlockNumber StorePages\n");
		for (int j = 0; j < 4; j++)
		{
			printf("    %d          %d\n", j, memory[j]);
		}

		char rdbuf[512];//读缓冲区
		char command[64];//得到读取到的命令
		//不能一次展示完
		//一次从流中读取512字节放到缓冲区里面
		int r = read(fd_stdin, rdbuf, 512);
		rdbuf[r] = 0;//r为字符串实际的结尾位置,这样标识了字符串的结束位置

		//取出用户输入的指令,并且转为实际命令
		int pointer = 0;
		//读到第一个空格的位置,即为命令 
		while (rdbuf[pointer] != 0 && rdbuf[pointer] != ' ')
		{
			command[pointer] = rdbuf[pointer];
			pointer++;
		}
		command[pointer] = 0;
		if (strcmp(command, "exit") == 0) return;
	}
	//printf("Page fault rate:%f \n", ((double)pageFaultCount / 320));

}
//----------------------------------内存管理代码结束处-----------------------------------------
void killproc(char *pid)
{
    int b = *pid - 48;
    if(b >= 0 && b <= NR_TASKS)   printf("operation to proc %d denied \n",b);
    else if(b < NR_TASKS + NR_PROCS)
    {
        proc_table[b].p_flags = 1;
        processManage();
    }
    else printf("can not find proc with pid:%d \n",b);
}

void pauseproc(char *pid)
{
    int b = *pid - 48;
    if(b >= 0 && b <= NR_TASKS)   printf("operation to proc %d denied \n",b);
    else if(b < NR_TASKS + NR_PROCS)
    {
        proc_table[b].run_state = 0;
        processManage();
    }
    else printf("can not find proc with pid:%d \n",b);
}

void resumeproc(char *pid)
{
    int b = *pid - 48;
    if(b >= 0 && b <= NR_TASKS)   printf("operation to proc %d denied \n",b);
    else if(b < NR_TASKS + NR_PROCS)
    {
        proc_table[b].run_state = 1;
        processManage();
    }
    else printf("can not find proc with pid:%d \n",b);
}


void processManage()
{	
int i;
	printf("======================================================\n");
	printf("                    Process Manage\n");
	printf("======================================================\n");
	printf("%3s      |%10s         |%12s      |%10s    \n", "PID","name","priority","running?");
	for ( i = 0 ; i < NR_TASKS + NR_PROCS; ++i )
	{
		if ( proc_table[i].p_flags == 1) continue;
		printf("%3d      |%10s         |%12d      |      ", proc_table[i].pid, proc_table[i].name, proc_table[i].priority);
        if(proc_table[i].run_state>0)
        {
        printf("YES\n");
        }
        else
        {
        printf("NO\n");
        }
	}
	printf("================================================================================\n");
	processHelp();
}
//----------------------------------游戏代码开始处-----------------------------------------



//----------------------------------游戏代码结束处-----------------------------------------

//----------------------------------公共代码开始处-----------------------------------------
/*****************************************************************************
 *                                help
 *                              帮助文档
 *****************************************************************************/
void help()
{
	printf("-----------help document------------\n");
	printf("1.help----help document\n");
	printf("2.fileHelp----fileSystem help document\n");
	printf("3.clear----clear the terminal\n");
	printf("4.manageRanch----manage ranch program\n");
	printf("5.memoryHelp----memorySystem help document\n");
	printf("6.processHelp----processSystem help document\n");
	printf("6.gameHelp----game help document\n");
	printf("------------------------------------\n");

}
void gameHelp() {
	printf("-----------game help document------------\n");
	printf("swipe:the game of mine swiping\n");
	printf("five:the game of five in row\n");
	printf("------------------------------------------\n");
}

void processHelp() {
	printf("-----------processSystem help document------------\n");
	printf("processHelp:show process help\n");
	printf("killproc [pid]:kill the process\n");
	printf("pauseproc [pid]:pause the process\n");
	printf("resumeproc [pid]:resume the process\n");
	printf("--------------------------------------------------\n");
}

void memoryHelp() {
	printf("-----------memorySystem help document------------\n");
	printf("memoryHelp:show memory help\n");
	printf("memoryAlloc:memory allocation\n");
	printf("requestPage:memory request page\n");
}

void fileHelp()
{
	printf("-----------fileSystem help document------------\n");
	printf("fileHelp:show file help\n");
	printf("clear:clear the console\n");
	printf("pwd:show current directory\n");
	printf("ls:show files\n");
	printf("cd:change directory\n");
	printf("rm:remove a directory or file\n");
	printf("mkdir:create directory\n");
	printf("touch:create a file\n");
	printf("vim:edit the file\n");
	printf("cat:show the file\n");
	printf("store:store the fileSystem\n");
	printf("ls1:show bottom files\n");
	printf("------------------------------------\n");
}

void notFound()
{
	printf("command not found!");
	printf("input help to get command");
}


void clear()
{	
	clear_screen(0,console_table[current_console].cursor);
    console_table[current_console].crtc_start = console_table[current_console].orig;
    console_table[current_console].cursor = console_table[current_console].orig;    
}

void doTest(char *path)
{
    struct dir_entry *pde = find_entry(path);
    printf(pde->name);
    printf("\n");
    printf(pde->pass);
    printf("\n");
}

//----------------------------------公共代码结束处-----------------------------------------



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
	char pid[5];//Process ID

	
	
	init();//初始化current为根目录
	//load();//加载磁盘本来就存在的文件
	load();
	Play();
	clear();//清空控制台

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
		else if (strcmp(command, "memoryHelp") == 0) memoryHelp();//内存命令帮助
		else if (strcmp(command, "memoryAlloc") == 0)memoryAlloc(fd_stdin);//动态分区分配
		else if (strcmp(command, "requestPage") == 0) requestPage(fd_stdin);//请求分页管理
		else if (strcmp(command, "manageRanch") == 0) manageRanch(fd_stdin);//牧场管理
		
				
		else if (strcmp(command, "processHelp") == 0) processManage();
		else if (strcmp(command, "gameHelp") == 0) gameHelp();
		else if (strcmp(command, "killproc") == 0) killproc(fileName);
		else if (strcmp(command, "pauseproc") == 0) pauseproc(fileName);
		else if (strcmp(command, "resumeproc") == 0) resumeproc(fileName);
		else if (strcmp(command, "five") == 0) startFive(fd_stdin);//the game of five in row
		else if (strcmp(command, "swipe") == 0) startSwipe(fd_stdin);//the game of mine swiping
		
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


void Play()
{
for (int i=0;i<10;i++)
{
clear();
printf("                                                                               \n");
printf("                                                                               \n");
printf("                                                                               \n");
printf("                                                                               \n");
printf("                                                                               \n");
printf("                                                                               \n");     
printf("                                                                          .... \n");
printf("                                                                      -:!!:::;*\n");
printf("                                                                   ,~=#! -.    \n");
printf("                                                                   !!!;        \n");
printf("                                                                     -!        \n");
printf("                                                                     $.        \n");
printf("                                                                    =-         \n");
printf("                                                                    *          \n");
printf("                                                                    =          \n");
printf("                                                                    #.         \n");
printf("                                                                    .=,        \n");
printf("                                                                      ;;.      \n");
printf("                                                                       .;!:-   \n");
printf("                                                                          .:!;-\n");
printf("                                                                              :\n");
printf("                                                                               \n");

milli_delay(1000);clear(); 
clear(); 
printf("                                                                               \n");
printf("                                                                               \n");
printf("                                                                               \n");
printf("                                                                             -:\n");
printf("                                                                          ~!*~,\n");
printf("                                                                        .=:    \n");
printf("                                                              ....      ~#     \n");
printf("                                                          -:!!:::;**-   :=     \n");
printf("                                                       ,~=#! -.    .@, .#,     \n");
printf("                                                       !!!;         ,@-#-      \n");
printf("                                                         -!          :@.       \n");
printf("                                                         $.           .$#*;!=#$\n");
printf("                                                        =-                     \n");
printf("                                                        *                      \n");
printf("                                                        =                      \n");
printf("                                                        #.                     \n");
printf("                                                        .=,                  ~*\n");
printf("                                                          ;;.                  \n");
printf("                                                           .;!:-               \n");
printf("                                                              .:!;-.           \n");
printf("                                                                  :**;~,..     \n");
printf("                                                                      .~;!!****\n");

milli_delay(1000);clear(); 
printf("                                                                             !$\n");
printf("                                                                         ,!$*, \n");
printf("                                                                     .~**:,    \n");
printf("                                                                 -:*!~-.       \n");
printf("                                                              ~!*~,       ,~;;*\n");
printf("                                                            .=:          ,==!!$\n");
printf("                                                  ....      ~#              ;$-\n");
printf("                                              -:!!:::;**-   :=            ,=!  \n");
printf("                                           ,~=#! -.    .@, .#,          ,==.   \n");
printf("                                           !!!;         ,@-#-          :#,     \n");
printf("                                             -!          :@.        ,*#:       \n");
printf("                                             $.           .$#*;!=#$!,          \n");
printf("                                            =-                                 \n");
printf("                                            *                              *#!*\n");
printf("                                            =                              .-!=\n");
printf("                                            #.                                 \n");
printf("                                            .=,                  ~***;::~~~:!=$\n");
printf("                                              ;;.                    .,-~~~-,. \n");
printf("                                               .;!:-                           \n");
printf("                                                  .:!;-.                       \n");
printf("                                                      :**;~,..           ..,,-:\n");
printf("                                                          .~;!!******===**!;:,\n");

milli_delay(1000);clear(); 

printf("                                                                 !$*!,\n");
printf("                                                             ,!$*,  .;#,\n");
printf("                                                         .~**:,       ;#\n");
printf("                                                     -:*!~-.         -#:    ...\n");
printf("                                                  ~!*~,       ,~;;**=@=;;!!;~~-\n");
printf("                                                .=:          ,==!!$$*~.        \n");
printf("                                      ....      ~#              ;$-            \n");
printf("                                  -:!!:::;**-   :=            ,=!              \n");
printf("                               ,~=#! -.    .@, .#,          ,==.               \n");
printf("                               !!!;         ,@-#-          :#,             ....\n");
printf("                                 -!          :@.        ,*#:              -@#;~\n");
printf("                                 $.           .$#*;!=#$!,                    -:\n");
printf("                                =-                                             \n");
printf("                                *                              *#!*$*,         \n");
printf("                                =                              .-!=$@@*;;;;*=$=\n");
printf("                                #.                                 ,@$,,,,,,.\n");
printf("                                .=,                  ~***;::~~~:!=$!--:!===*;;:\n");
printf("                                  ;;.                    .,-~~~-,.         ..,-\n");
printf("                                   .;!:-                                       \n");
printf("                                      .:!;-.                          .-~!**===\n");
printf("                                          :**;~,..           ..,,-:!===!~.\n");
printf("                                              .~;!!******===**!;:,\n");

milli_delay(1000);clear(); 
printf("                                                     !$*!,                     \n");
printf("                                                 ,!$*,  .;#,                   \n");
printf("                                             .~**:,       ;#                   \n");
printf("                                         -:*!~-.         -#:    ...,---~~:~~::~\n");
printf("                                      ~!*~,       ,~;;**=@=;;!!;~~----,,,...   \n");
printf("                                    .=:          ,==!!$$*~.                    \n");
printf("                          ....      ~#              ;$-                 ---,.  \n");
printf("                      -:!!:::;**-   :=            ,=!                  .=*!!=##\n");
printf("                   ,~=#! -.    .@, .#,          ,==.                           \n");
printf("                   !!!;         ,@-#-          :#,             .....          .\n");
printf("                     -!          :@.        ,*#:              -@#;~;==-    .-=$\n");
printf("                     $.           .$#*;!=#$!,                    -:!*!#$;;:~.  \n");
printf("                    =-                                                #;       \n");
printf("                    *                              *#!*$*,         .!@~        \n");
printf("                    =                              .-!=$@@*;;;;*=$=:,          \n");
printf("                    #.                                 ,@$,,,,,,.              \n");
printf("                    .=,                  ~***;::~~~:!=$!--:!===*;;:~:::;;;.    \n");
printf("                      ;;.                    .,-~~~-,.         ..,--~-,,,:@,   \n");
printf("                       .;!:-                                              #;   \n");
printf("                          .:!;-.                          .-~!**===***!**$!    \n");
printf("                              :**;~,..           ..,,-:!===!~.                 \n");
printf("                                  .~;!!******===**!;:,                         \n");
milli_delay(1000);clear(); 
printf("                                         !$*!,                                 \n");
printf("                                     ,!$*,  .;#,                               \n");
printf("                                 .~**:,       ;#                               \n");
printf("                             -:*!~-.         -#:    ...,---~~:~~::~~           \n");
printf("                          ~!*~,       ,~;;**=@=;;!!;~~----,,,...   .;.         \n");
printf("                        .=:          ,==!!$$*~.                     :-         \n");
printf("              ....      ~#              ;$-                 ---,.  -$.         \n");
printf("          -:!!:::;**-   :=            ,=!                  .=*!!=##!           \n");
printf("       ,~=#! -.    .@, .#,          ,==.                           $,          \n");
printf("       !!!;         ,@-#-          :#,             .....          .@-          \n");
printf("         -!          :@.        ,*#:              -@#;~;==-    .-=$            \n");
printf("         $.           .$#*;!=#$!,                    -:!*!#$;;:~.              \n");
printf("        =-                                                #;                   \n");
printf("        *                              *#!*$*,         .!@~                    \n");
printf("        =                              .-!=$@@*;;;;*=$=:,                      \n");
printf("        #.                                 ,@$,,,,,,.                          \n");
printf("        .=,                  ~***;::~~~:!=$!--:!===*;;:~:::;;;.                \n");
printf("          ;;.                    .,-~~~-,.         ..,--~-,,,:@,               \n");
printf("           .;!:-                                              #;               \n");
printf("              .:!;-.                          .-~!**===***!**$!                \n");
printf("                  :**;~,..           ..,,-:!===!~.                             \n");
printf("                      .~;!!******===**!;:,                              \n");
milli_delay(1000);clear(); 
printf("                             !$*!,                                             \n");
printf("                         ,!$*,  .;#,                                           \n");
printf("                     .~**:,       ;#                                           \n");
printf("                 -:*!~-.         -#:    ...,---~~:~~::~~                       \n");
printf("              ~!*~,       ,~;;**=@=;;!!;~~----,,,...   .;.                   \n");
printf("            .=:          ,==!!$$*~.                     :-                \n");
printf("  ....      ~#              ;$-                 ---,.  -$.                 \n");
printf("!!:::;**-   :=            ,=!                  .=*!!=##!                       \n");
printf(" -.    .@, .#,          ,==.                           $,                \n");
printf("        ,@-#-          :#,             .....          .@-                  \n");
printf("         :@.        ,*#:              -@#;~;==-    .-=$                    \n");
printf("          .$#*;!=#$!,                    -:!*!#$;;:~.                    \n");
printf("                                              #;                                \n");
printf("                           *#!*$*,         .!@~                             \n");
printf("                           .-!=$@@*;;;;*=$=:,                            \n");
printf("                               ,@$,,,,,,.                                 \n");
printf("                 ~***;::~~~:!=$!--:!===*;;:~:::;;;.                     \n");
printf(".                    .,-~~~-,.         ..,--~-,,,:@,                   \n");
printf(";!:-                                              #;                   \n");
printf("  .:!;-.                          .-~!**===***!**$!                   \n");
printf("      :**;~,..           ..,,-:!===!~.                                   \n");
printf("          .~;!!******===**!;:,                              \n");
milli_delay(1000);clear(); 
printf("                 !$*!,                                                  \n");
printf("             ,!$*,  .;#,                                                \n");
printf("         .~**:,       ;#                                                \n");
printf("     -:*!~-.         -#:    ...,---~~:~~::~~                            \n");
printf("  ~!*~,       ,~;;**=@=;;!!;~~----,,,...   .;.                          \n");
printf(".=:          ,==!!$$*~.                     :-                          \n");
printf("~#              ;$-                 ---,.  -$.                          \n");
printf(":=            ,=!                  .=*!!=##!                            \n");
printf("#,          ,==.                           $,                           \n");
printf("-          :#,             .....          .@-                           \n");
printf("        ,*#:              -@#;~;==-    .-=$                             \n");
printf("#*;!=#$!,                    -:!*!#$;;:~.                               \n");
printf("                                  #;                                    \n");
printf("               *#!*$*,         .!@~                                     \n");
printf("               .-!=$@@*;;;;*=$=:,                                       \n");
printf("                   ,@$,,,,,,.                                           \n");
printf("     ~***;::~~~:!=$!--:!===*;;:~:::;;;.                                 \n");
printf(".        .,-~~~-,.         ..,--~-,,,:@,                                \n");
printf("                                      #;                                \n");
printf("                      .-~!**===***!**$!                                 \n");
printf("..           ..,,-:!===!~.                                              \n");
printf(";!!******===**!;:,                              \n");
milli_delay(1000);
clear(); 

printf(" !$*!,                                                  \n");
printf("*,  .;#,                                                \n");
printf("      ;#                                                \n");
printf("     -#:    ...,---~~:~~::~~                            \n");
printf(";;**=@=;;!!;~~----,,,...   .;.                          \n");
printf("!!$$*~.                     :-                          \n");
printf(";$-                 ---,.  -$.                          \n");
printf("!                  .=*!!=##!                            \n");
printf("                           $,                           \n");
printf("           .....          .@-                           \n");
printf("          -@#;~;==-    .-=$                             \n");
printf("             -:!*!#$;;:~.                               \n");
printf("                  #;                                    \n");
printf("#!*$*,         .!@~                                     \n");
printf("-!=$@@*;;;;*=$=:,                                       \n");
printf("   ,@$,,,,,,.                                           \n");
printf("!=$!--:!===*;;:~:::;;;.                                 \n");
printf(",.         ..,--~-,,,:@,                                \n");
printf("                      #;                                \n");
printf("      .-~!**===***!**$!                                 \n");
printf(",-:!===!~.                                              \n");
printf(":,                              \n");
}
milli_delay(3000);clear(); 
}








/*****************************************************************************
 *                                panic
 *****************************************************************************/
PUBLIC void panic(const char *fmt, ...)
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

