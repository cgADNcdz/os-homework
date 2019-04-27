/**********实验四：fork()系统调用创建进程*********************/
#include<iostream>
#include<unistd.h>
#define  N 99
using namespace std;

//备注：写报告的时候多运行几次，检验并行
void testFork()
{

       /*for(int i=0;i<9;i++)       //这段必然是先执行的，之后才会创建子进程
       {
            cout<<"#################this is the "<<i<<"th running of father before create processes,";
            cout<<"pid="<<(int)getpid()<<endl;
       } */
      
      int pid1=fork();
      if(!pid1)                         //子进程１
      {
         for(int i=0;i<N;i++)
            {
               cout<<"**********this is the "<<i<<"th running of son1,";
               cout<<" pid="<<(int)getpid()<<endl;
            }
      }
      else if(pid1>0)                  //子进程１创建后返回的父进程
      {
            for(int i=0;i<N;i++)
            {
               cout<<"##########this is the "<<i<<"th running of father after create proc1,";
               cout<<"pid="<<(int)getpid()<<endl;
            } 


            int pid2=fork();
            if(!pid2)                 //子进程２
            {
                  for(int i=0;i<N;i++)
                  {
                     cout<<"*****this is the "<<i<<"th running of son2,";
                     cout<<"pid="<<(int)getpid()<<endl;
                  } 
            }
            else if(pid2>0)          //子进程２创建后返回的父进程
            {
                    for(int i=0;i<N;i++)
                  {
                     cout<<"#####this is the "<<i<<"th running of father after create proc2,";
                     cout<<"pid="<<(int)getpid()<<endl;
                  } 
            }
            else
            {
               cout<<"error when creating son process2!"<<endl;
            }
            
      }

      else
      {
            cout<<"error when creating son process1！"<<endl;
      }
}



int main()
{
   testFork();
   return 0;
}