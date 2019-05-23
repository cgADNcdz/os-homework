#include<iostream>
#include<vector>
#include<cmath>
#include<map>
#include<string>


using namespace std;

typedef struct normal                      //普通的磁盘块(本次实验不需要对其内容赋值),内表示它的存在即可
{
    int id;
}normalBlcok;

int main()
{
   normalBlcok a;
   a.id=4;

   normalBlcok b;
   b.id=7;

   a=b;

   cout<<a.id<<endl;
   
   cout<<"hello cg!"<<endl;
   return 0;
}