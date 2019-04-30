/************实验二：模拟分页管理方式下的主存分配与回收(位示图)***************************/
#include<iostream>
#include<vector>
#include<string>
#include<cmath>
#define blockSize 64     //物理块个数

using namespace std;

/******************************小结&说明************************************/
/*1.位示图的每一个bit对应一个物理块,应该是一维的，但是理解成二维,行数*列数＝blockNum */
/*2.注意逻辑地址到页号，物理地址到块号的转换，虽然这里不需要考虑这个                */
/*3.没有考虑进程的优先级，按创建顺序依次加入进程队列(使用vector表示，方便删除)      */
/*4.删除进程时注意释放内存                                                   */
/*************************************************************************/

typedef struct mMap{
   int freeNum;           //空闲块数
   int map[(int)sqrt(blockSize)][(int)sqrt(blockSize)];
}BitMap;

typedef struct proc{
   string procName;
   int pageNum;
   int*  pageTable;   //页表，通过指针进程动态分配内存
}Process;


bool createProc(vector<Process>& procs,BitMap& bitMap)   //创建一个进程，加入进程队列(数组),修改位图
{
   Process proc;
   cout<<"input(procName pageNum):";
   cin>>proc.procName >> proc.pageNum;
   if(proc.pageNum<=bitMap.freeNum)         //有足够存储空间.动态分配内存建立页表,加入进程队列(数组)
   {
       proc.pageTable=new int[proc.pageNum];  
       int k=0;
       for(int i=0;i<(int)sqrt(blockSize);i++)
       {
          bool isOK=false;                 //标志分配是否完成 
         for(int j=0;j<(int)sqrt(blockSize);j++)
         {
            if(!bitMap.map[i][j])
            {  
               //k对应页号，((int)sqrt(blockSize))*i+j对应块号
               proc.pageTable[k++]=((int)sqrt(blockSize))*i+j;
               bitMap.map[i][j]=1;
               bitMap.freeNum--;
               if(k==proc.pageNum)
               {
                  isOK=true;
                  break;
               }
            }
         }
         if(isOK)
            break;
       }
       procs.push_back(proc);                   //加入进程队列
       cout<<"create process successfully!"<<endl;
       return true;
   }

   cout<<"error!no enough block"<<endl;    //没有足够的存储空间，进程创建失败
   return false;
}

bool releaseProc(vector<Process>& procs,BitMap& bitMap,string procName) //释放一个进程，从队列中删除，修改位图  
{
   vector<Process>::iterator it;
   for(it=procs.begin();it!=procs.end();it++)
   {
      if(it->procName==procName)         //找到要删除的进程
      {
         int k=0;
         while(k<it->pageNum)            //修改位图
         {
            int i=it->pageTable[k]/((int)sqrt(blockSize));  
            int j=it->pageTable[k]%((int)sqrt(blockSize));
            bitMap.map[i][j]=0;
            bitMap.freeNum++;
            k++;
         }
         delete it->pageTable;          //释放内存
         procs.erase(it);               //删除进程
         cout<<"release process successfully!"<<endl;
         return true;
      }
   }
   cout<<"error when release process!"<<endl;
   return false;
}


void display(bool showMap,const BitMap bitMap,
            bool showQueue,const vector<Process> procs,
            bool showPageTable,string procName)
{
      if(showMap)             //显示位示图
      {
         cout<<"the BitMap is as follows:"<<endl;
         for(int i=0;i<(int)sqrt(blockSize);i++)
         {
            for(int j=0;j<(int)sqrt(blockSize);j++)
               cout<<bitMap.map[i][j]<<" ";
            cout<<"\n";
         }
         cout<<"total free blocks:"<<bitMap.freeNum<<endl;
      }

      if(showQueue)         //显示进程队列(数组)中进程大致信息
      {
         cout<<"the process queue info is as follows(procName,pageNum):"<<endl;
          for(int i=0;i<procs.size();i++)
             cout<<procs[i].procName<<","<<procs[i].pageNum<<endl;
      }

      if(showPageTable)      //显示进程的页表
      {  
         for(int i=0;i<procs.size();i++)
         {
            if(procs[i].procName==procName)
            {
                  cout<<"the pagetable of process "<<procName<<" is as follows(page,block):"<<endl;
                  for(int j=0;j<procs[i].pageNum;j++)
                     cout<<j<<" "<<procs[i].pageTable[j]<<endl;
                  break;
            }
         }
      }
}


void memManage()
{
   BitMap bitMap;
   vector<Process> procs;
   for(int i=0;i<(int)sqrt(blockSize);i++)                  //初始化位图信息
      for(int j=0;j<(int)sqrt(blockSize);j++)
         bitMap.map[i][j]=0;
   bitMap.freeNum=blockSize;

   while(true)
   {
      cout<<"*******************************************************"<<endl;
      cout<<"options:A->create process"<<endl;
      cout<<"        B->show BitMap    "<<endl;
      cout<<"        C->show process queue"<<endl;
      cout<<"        D->show pageTable of a process"<<endl;
      cout<<"        E->release a process"<<endl;
      cout<<"        F->quit"<<endl;
      cout<<"your option:";
      string choose;
      cin>>choose;
      if(choose=="A")                     //创建进程
      {
         createProc(procs,bitMap);
         cout<<"\n";
      }
      else if(choose=="B")               //显示位图
      {
         display(1,bitMap,0,procs,0," ");
         cout<<"\n";
      }
      else if(choose=="C")               //显示进程队列
      {
         display(0,bitMap,1,procs,0," ");
         cout<<"\n";
      }
      else if(choose=="D")              //显示某个进程的进程表
      {
         string procName;
         cout<<"input procName to show pagetable:";
         cin>>procName;
         display(0,bitMap,0,procs,1,procName);
         cout<<"\n";
      }
      else if(choose=="E")             //释放一个进程
      {
         string procName;
         cout<<"input procName to release:";
         cin>>procName;
        releaseProc(procs,bitMap,procName);
         cout<<"\n";
      }
      else if(choose=="F")           //退出
         break;
      else
         cout<<"error! please check your input"<<endl;  
   }
}


int main()
{

   memManage();
   cout<<"end!"<<endl;
   return 0;
}