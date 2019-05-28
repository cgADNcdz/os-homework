#include<iostream>
#include<vector>
#include<cmath>
#include<map>
#include<string>
#include<ctime>

using namespace std;

typedef struct normal                      //普通的磁盘块(本次实验不需要对其内容赋值),内表示它的存在即可
{
    int id;
}normalBlcok;

int main()
{
   srand((unsigned)time(NULL));
   for(int i=0;i<10;i++)
   {
       int a=rand()%5;
       cout<<a<<endl;
   }
  
   
   cout<<"hello cg!"<<endl;
   return 0;
}