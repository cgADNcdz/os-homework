/***************实验一：模拟处理器调度（按优先级数调度）*******/
#include<iostream>
#include<string>

using namespace std;

/**************************memo*******************************/
/*1.进程的指针改用优先队列或许更方便，但是指定了要指针,这里自己写了队列 */
/*2.对指针的理解还是太嫩了！！z总是记不住　　　　　　　　　　　　　　　　*/
/*************************************************************/


enum STATUS{
   R,E                                 //R代表就绪状态，Ｅ代表结束    
};

typedef struct pcb{
   string name;
   int time;
   int priority;
   STATUS status;
    pcb* next;
}PCB;


/*****************优先队列操作*****************************/
bool createQueue(PCB* head)            //创建带头结点的单链表
{
   head->next=NULL;
   return true;
}

bool isEmpty(PCB* head)                //判断队列是否为空
{
   if(!head->next)
      return true;
   return false;
}

bool enQueue(PCB* head,PCB* tmp)       //插入优先队列
{

   if(head->next==NULL) 
   {
      head->next=tmp;
      return true;
   }
   PCB* p=head;
   PCB* q=p->next;     
   while(q && tmp->priority <= q->priority )
   {
      p=p->next;
      q=q->next;
   }
   if(!q)     //如果移动到了队尾
   {
      p->next=tmp;
      return true;
   }

   p->next=tmp;
   tmp->next=q;
   return true;
}

bool deQueue(PCB* head,PCB* first)    //队首进程出队（不释放！）
{
   PCB* tmp=head->next;
   head->next=tmp->next;

   tmp->next=NULL;
   *first=*tmp;   //tmp指向的内容传出               
   delete tmp;
   return true;
}

void displayQueue(PCB* head)         //输出就绪队列情况
{
   if(!head->next)
   {
      cout<<"empty"<<endl;
      return;
   }

   PCB* tmp=head->next;
   while(tmp)
   {
      cout<<"(name,time,status,priority):";
      cout<<tmp->name <<" "<<tmp->time <<" R " << tmp->priority <<endl;
      tmp=tmp->next;
  }
}



/************************进程相关*************************/
void createProc(PCB* head)
{
   PCB* tmp=new PCB;
   cout<<"input name,time,priority:";
   cin>> tmp->name >> tmp->time >>tmp->priority;
   tmp->status=R;
   tmp->next=NULL;
   enQueue(head,tmp);
}

void runFirstProc(PCB* first)             //运行队首进程
{
   first->time--;
   first->priority--;
   if(!first->time)
      first->status=E;
} 

void procScheduling()
{
   PCB* head=new PCB;
   createQueue(head);                      //创建进程队列
   int procNum;
   cout<<"input the number of process:";
   cin>>procNum;
   for(int i=0;i<procNum;i++)
   {
      cout<<i+1<<"th process,";
      createProc(head);                   //创建一个进程并加入队列
   }

   cout<<"\nthe processes you create(order by priority):"<<endl;
   displayQueue(head);
   cout<<"\n";

   cout<<"***********************process scheduling*******************************"<<endl;
   int i=1;
   while(!isEmpty(head))                  //就绪队列非空，进行调度
   {
         cout<<i<<"th schelduling"<<endl;
         PCB* first=new PCB;              
         deQueue(head,first);
         runFirstProc(first);
        
         cout<<"running process(name,time,status,priority):";
         string status=(first->status)?"E":"R";
         cout<<first->name<<" "<<first->time<<" "<<status<<" "<<first->priority<<endl;
         cout<<"processes in readyQueue(order by priority):"<<endl;
         displayQueue(head);
         
         if(!first->time)                //运行进程时间结束，释放
         {
            cout<<"process "<<first->name<<" is END"<<endl;
            delete first;
         }
         else                            //运行进程再次加入队列
         {
            enQueue(head,first);             
         }

         cout<<"###after "<<i<<"th scheduling,processes in readyQueue(order by priority):"<<endl;
         displayQueue(head);
         cout<<"input(E:end,N:next):";
       
         string ch;
         cin>>ch;
         if(ch=="E")
            break;
         
         i++;
         cout<<"\n";
   }
   delete head;
}


int main()
{
   procScheduling();
   cout<<"end!"<<endl;
   return 0;
}