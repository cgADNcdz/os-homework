/**************实验五：模拟进程同步***************************/
#include<iostream>
#include<ctime>
using namespace std;

/****************************** memo ***********************************/
/*1.并没有严格按照信号量的count,queue形式组织                               */
/*2.关于没有使用互斥信号量的原因：这里临界区只有一条指令,不存在两个进程同时访问临界区
　　　　　　　　　　　　　　　　　的情况!!!!!!!!!!!!                          */
/*2.p、v等指令的模拟本来应该带有参数，为了方便，所有参数都设置成了去全局变量!!     */
/*3.没有队列导致判断等待队列、就绪对列比较麻烦                                */
/*4.p v操作和结束进程可能改变进程状态,因此指令完成后只能改变RUN状态的进程为READY  */
/*5.goto语句是应该修改PC，而不是修改当前进程的breakpoint(会随PC改变)           */
/***********************************************************************/

typedef int semaphore;
typedef string products;
semaphore used;              //两个同步信号量，表示缓冲区使用个数和剩余个数
semaphore remains;

enum STATE {RUN,READY,WAIT,FINISH};
enum REASON {USED,REMAINS,NOWAIT};
enum TYPE {PRODUCER,CONSUMER};
typedef struct pcb
{
    string name;
    int id;
    TYPE type;
    STATE state;
    REASON waitReason;
    int breakpoint;
}PCB;
PCB* producers;
PCB* consumers;
PCB* curr;                                            //指向当前进程
                     
#define P 0
#define V 1
#define PRODUCE 2
#define PUT 3
#define GET 4
#define CONSUME 5
#define GOTO 6
#define NOP 7

void (*instructions[8])(void);                    //指针数组，每一项存储个函数地址(指令)
int PA[5]={PRODUCE,P,PUT,V,GOTO};                //生产者的指令序列入口地址
int SA[5]={P,GET,V,CONSUME,GOTO};
int PC;

int produceNum;
int consumeNum;
int IN;                                          //生产的产品在缓冲区中的下标
int OUT;
int bufferSize;
string* BUFF;
products c;                                      //生产的一个产品
products x;                                      //取出的一个产品
int productID=0;                                 //每个产品的唯一编码


/***********指令（函数）的定义****************/
void p()
{
    if(curr->type==PRODUCER)
    {
        remains--;
        if(remains<0)
        {
           curr->state=WAIT;
           curr->waitReason=REMAINS;
        }
    }
    else if(curr->type==CONSUMER)
    {
        used--;
        if(used<0)
        {
            curr->state=WAIT;
            curr->waitReason=USED;
        }
    }
}

void  v()
{
    if(curr->type==PRODUCER)
    {
        used++;
        if(used<=0)
        {
           while(true)                               //随机选择等待队列的一个进程就绪(used信号量的等待对列在consumers中)
           {
               bool flag=false;
               for(int i=0;i<consumeNum;i++)
                    if(consumers[i].state==WAIT)
                    {
                        flag=true;
                        break;
                    }
                if(!flag)   break;                 //如果该信号量等待队列空

               int i=rand()%consumeNum;
               if(consumers[i].state==WAIT)
               {
                   consumers[i].state=READY;
                   consumers[i].waitReason=NOWAIT;
                   break;
               }
           }
        }
    }
    else if(curr->type==CONSUMER)
    {
        remains++;
        if(remains<=0)
        {
            while(true)
            {  
                bool flag=false;
                for(int i=0;i<consumeNum;i++)
                    if(producers[i].state==WAIT)
                    {
                        flag=true;
                        break;
                    }
                if(!flag)   break;                 //如果该信号量等待队列空

                int i=rand()%produceNum;
                if(producers[i].state==WAIT)
                {
                    producers[i].state=READY;
                    producers[i].waitReason=NOWAIT;
                    break;
                }
            }
        }
    }
}

void produce()
{
    c="product"+to_string(productID);
    productID++;
}

void put()
{
    BUFF[IN]=c;
    IN=(IN+1)%bufferSize;
}

void get()
{
    x=BUFF[OUT];
    OUT=(OUT+1)%bufferSize;
}

void consume()
{
    cout<<"消费了产品:"<<x<<endl;
}

void goto0()
{
    if(curr->type==PRODUCER)
        PC=0;   
    else if(curr->type==CONSUMER)
        PC=0;
}

void nop()
{
    /*空操作*/
}



/***********程序的主要组成*****************/
void init()                                    //初始化
{
    cout<<"输入缓冲区大小:";
    cin>>bufferSize;
    remains=bufferSize;
    used=0;
    BUFF=new string[bufferSize];
    IN=0;
    OUT=0;

    cout<<"输入(生产者个数　消费者个数):";
    cin>>produceNum>>consumeNum;

    producers=new PCB[produceNum];
    consumers=new PCB[consumeNum];

    for(int i=0;i<produceNum;i++)
    {
        producers[i].name="producer"+to_string(i);
        producers[i].id=i;
        producers[i].state=READY;
        producers[i].waitReason=NOWAIT;
        producers[i].type=PRODUCER;
        producers[i].breakpoint=0;
    }

     for(int i=0;i<consumeNum;i++)
    {
        consumers[i].name="consumer"+to_string(i);
        consumers[i].id=i;
        consumers[i].state=READY;
        consumers[i].waitReason=NOWAIT;
        consumers[i].type=CONSUMER;
        consumers[i].breakpoint=0;
    }
    
    instructions[0]=p;
    instructions[1]=v;
    instructions[2]=produce;
    instructions[3]=put;
    instructions[4]=get;
    instructions[5]=consume;
    instructions[6]=goto0;
    instructions[7]=nop;

    curr=&producers[rand()%(produceNum)];       //这里curr只是为了配合dispatch，这个curr并不会直接运行         
    PC=0;
  
}

void dispatch()                                //处理器调度   
{ 
    curr->breakpoint=PC;                       //保护(上一个进程)现场

    bool flag=false;                           //判断是否有就绪进程
    for(int i=0;i<produceNum;i++)
        if(producers[i].state==READY)
        {
            flag=true;
            break;
        }
    for(int j=0;j<consumeNum;j++)
        if(consumers[j].state==READY)
        {
            flag=true;
            break;
        }
   
    if(flag)                                    //随机选择一个就绪进程投入运行
    {
        while(true)                           
        {
            int tmpid=rand()%(produceNum+consumeNum);
            if(tmpid<=produceNum-1)            //随机初步选择的是生产者
            {
                if(producers[tmpid].state==READY)
                {
                    curr=&producers[tmpid];
                    curr->state=RUN;
                    PC=curr->breakpoint;
                    break;
                }
            }
            else                             //随机初步选择的是消费者
            {
                if(consumers[tmpid-produceNum].state==READY)
                {
                    curr=&consumers[tmpid-produceNum];
                    curr->state=RUN;
                    PC=curr->breakpoint;
                    break;
                }
            }   
        } 
       
    }

    else
    {
        cout<<"已经没有就绪进程"<<endl;
        return;
    }  
}

void simulation()                              //模拟处指令执行
{
    int j;
    if(curr->type==PRODUCER)
        j=PA[PC++];
    else if(curr->type==CONSUMER)
        j=SA[PC++];

    instructions[j]();                       //执行指令
    if(j==GOTO)
    {
        cout<<curr->name<<"完成了一个产品的生产/消费,需要结束它吗？(yes,no):";
        string ch;
        cin>>ch;
        while(ch!="yes" && ch!="no")
        {
            cout<<"输入错误，请重新输入：";
            cin>>ch;
        }
        if(ch=="yes")
        {
            curr->state=FINISH;
        }
    }
 
    if(curr->state==RUN)                                  //执行指令后，重置状态为就绪(状态已经改为WAIT、FINISH除外)
        curr->state=READY;
}

void display()
{
    //生产者、消费者信息
    cout<<"\n";
    cout<<"***********************************当前信息************************************"<<endl;
    cout<<"生产者进程:"<<"name\t\tid\ttype\t\tstate\twaitReason\tbreakpoint"<<endl;
    for(int i=0;i<produceNum;i++)
    {
        string state;
        string waitReason;
        switch(producers[i].state)
        {
            case 0:state="RUN";break;
            case 1:state="READY";break;
            case 2:state="WAIT"; break;
            case 3:state="FINISH";break;
        }
        switch(producers[i].waitReason)
        {
            case 0:waitReason="USED";break;
            case 1:waitReason="REMAINS";break;
            case 2:waitReason="NOWAIT";break;
        }
        cout<<"         "<<producers[i].name<<"\t"<<producers[i].id<<"\t"<<(producers[i].type==0? "producer":"consumer")<<"\t"<<state<<"\t"<<waitReason<<"\t\t"<<producers[i].breakpoint<<endl;
    }
    cout<<"\n";

    cout<<"消费者进程:"<<"name\t\tid\ttype\t\tstate\twaitReason\tbreakpoint"<<endl;
    for(int i=0;i<consumeNum;i++)
    {
        string state;
        string waitReason;
        switch(consumers[i].state)
        {
            case 0:state="RUN";break;
            case 1:state="READY";break;
            case 2:state="WAIT";break;
            case 3:state="FINISH";break;
        }
        switch(consumers[i].waitReason)
        {
            case 0:waitReason="USED";break;
            case 1:waitReason="REMAINS"; break;
            case 2:waitReason="NOWAIT";
        }
        cout<<"         "<<consumers[i].name<<"\t"<<consumers[i].id<<"\t"<<(consumers[i].type==0? "producer":"consumer")<<"\t"<<state<<"\t"<<waitReason<<"\t\t"<<consumers[i].breakpoint<<endl;
   
    }
    cout<<"\n";

    //信号量、缓冲区、产品信息
    cout<<"used(已占用缓冲区个数):"<<used<<endl;
    cout<<"remains(剩余缓冲区个数):"<<remains<<endl;
    cout<<"缓冲区产品：";
    int i=OUT;
    while(i!=IN)
    {
        cout<<BUFF[i]<<" ";
        i=(i+1)%bufferSize;
    }
    cout<<"\n";

    //当前进程信息
    string state;
    string waitReason;
    switch(curr->state)
        {
            case 0:state="RUN";break;
            case 1:state="READY";break;
            case 2: state="WAIT"; break;
            case 3:state="FINISH";break;
        }
    switch(curr->waitReason)
        {
            case 0:waitReason="USED"; break;
            case 1:waitReason="REMAINS";break;
            case 2: waitReason="NOWAIT";break;
        }
    cout<<"当前进程:"<<curr->name<<"\t"<<curr->id<<"\t"<<(curr->type==0? "producer":"consumer")<<"\t"<<state<<"\t"<<waitReason<<"\t\t"<<curr->breakpoint<<endl;   
    string instruction;
    if(curr->type==PRODUCER)
    {
        switch(PA[PC])
        {
            case 0:instruction="P";break;
            case 1:instruction="V";break;
            case 2:instruction="PRODUCE";break;
            case 3:instruction="PUT";break;
            case 4:instruction="GET";break;
            case 5:instruction="CONSUME";break;
            case 6:instruction="GOTO";break;
            case 7:instruction="NOP";break;
        }
    }
    else if(curr->type==CONSUMER)
    {
        switch(SA[PC])
        {
            case 0:instruction="P";break;
            case 1:instruction="V";break;
            case 2:instruction="PRODUCE";break;
            case 3:instruction="PUT";break;
            case 4:instruction="GET";break;
            case 5:instruction="CONSUME";break;
            case 6:instruction="GOTO";break;
            case 7:instruction="NOP";break;
        }
    }
    cout<<"将执行指令:"<<instruction<<endl;;
    cout<<"**********************************end info******************************************"<<endl;
}

int main()
{

    srand((unsigned)time(NULL));
    init();

    while(true)
    {
        dispatch();
        display();
        simulation();
        cout<<"继续？(n(next) or q(quit)):";
        string ch;
        cin>>ch;
        while(ch!="n" && ch!="q")
        {
            cout<<"输入错误，请重新输入:";
            cin>>ch;
        }
        if(ch=="q")
            break;
    }

    delete[] producers;
    delete[] consumers;
    delete[] BUFF;
    cout<<"end!"<<endl;
    return 0;
}