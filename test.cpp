#include<iostream>
#include<vector>

using namespace std;

typedef struct p
{
   int a;
   int b;
}P;

void gg(P* p)
{
   p=new P;
   p->a=1;
   p->b=2;
}

int main()
{
   P* p;
   //gg(p);
   
   cout<<p->a<<" "<<p->b<<endl;


   cout<<"hello cg!"<<endl;
   return 0;
}