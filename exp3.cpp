/***********模拟磁盘管理：成组链接法*************/
#include<iostream>
#include<stack>
#include<map>
#include<vector>
#include<string>
using namespace std;


/******************************memo***************************************/
/*1.磁盘的最小存取单位是扇区！也就是物理块，大小为512字节(或4k等)                 */
/*2.多个扇区成组，在windows中成为簇，在linux中称为块(block)                    */
/*3.内存的最小存取单位是字节！！！                                            */
/*4.内存中设置一个栈，保存当前可用组的盘块数和盘块号；　此外还有下一组的起始盘块号    */
/*5.每一组的第一块，登记的是下一组的信息;                                      */
/*6.注意理解初始化后给S的赋值以及删除第一个group                                */
/*7.注意理解错开了一个位置！！！！！！！！！！！！！！！！                         */
/*8.M.groups[i]代表的是第i组的首块中保存的信息(即第i+1组的信息); 
    M.groups[i].id是第i组的首块的地址；
    Ｍ.groups[i].N是第i+1组中的空闲块数量;
    M.groups[i].freeBlockID保存第i+1组中空闲块的地址　　　　　　　　　　　　　　　*/
/*************************************************************************/



typedef struct first                       //每一组的第一个磁盘块，用于记录链接的另一组的块数和块号
{
    int id;                                //组首块编号
    int N;                                 //空闲块数量
    vector<int> freeBlockID;               //空闲块编号(用stack不方便打印，改用vector)
} firstBlock;

/*typedef struct normal                      //普通的磁盘块(本次实验不需要对其内容赋值),内表示它的存在即可
{
    int id;
    string content;                        //文件块存储内容，这里没有用到
}normalBlcok; */                           //对于磁盘管理而言，只需要知道每组第一块的内容即可，其他块的内容并没有意义，反而增加负担

/*typedef struct gourp
{
    int groupID;
    firstBlock firstblock;
}Group;*/

typedef firstBlock specialBlock;          //定义一个专用块，结构同每组的第一个磁盘块，存储当前可用组的信息

typedef struct manager
{
    int blockNum;                         //磁盘块总数
    int groupSize;                        //每组盘块数量
    int groupNum;                         //组数
    vector<firstBlock> groups;            //记录下一组的信息
}Manager;

Manager M;                               //记录空闲块信息
specialBlock S;                          //专用块spatialBlock
map<string, vector<int> > F;             //fileCatalog,文件目录，存储文件名及其物理块号


void init()
{
    cout<<"输入总的物理块数:";
    cin>>M.blockNum;
    cout<<"输入每一组占用物理块数:";
    cin>>M.groupSize;

    M.groupNum=M.blockNum/M.groupSize +1;

    int i=1;                              //块的编号从１开始计(０有特殊用途)
    firstBlock tmpBlock;
    while(i<=M.blockNum)                  //给每一物理块分组
    {
        if(i%M.groupSize==1)             //ｉ是该组的第一块(暂不考虑专用块)
        {  
            if(i>M.groupSize)
            {
                M.groups.push_back(tmpBlock);
                tmpBlock.freeBlockID.clear();
            }
           

            if(M.blockNum-i<M.groupSize) //是最后一组
            {
                tmpBlock.id=i;                                         
                tmpBlock.N=M.blockNum-i+1;
                tmpBlock.freeBlockID.push_back(0);                    //后面没有组，下一组首块地址置0
                tmpBlock.freeBlockID.push_back(i);
            }
            else
            {
                tmpBlock.id=i;
                tmpBlock.N=M.groupSize;
                tmpBlock.freeBlockID.push_back(i);                   //设置本组的首块地址
            } 
        }
        else                            //ｉ是普通块，添加进其对应组的首块信息
        {
            tmpBlock.freeBlockID.push_back(i);
        }

        i++; 
    }
    M.groups.push_back(tmpBlock);
    
    //整理，使得S存放第一组的信息，groups[0]存放第二组信息、gruops[1]存放第三组信息
    //从而形成错位，使得每组的第一块中登记了下一组空闲块的块数和块号！！！仔细理解！！
   
    S=M.groups[0];                      //初始化后的第一组信息复制到专用块  
    S.id=-1;                            //专用块保存第一组信息，没有首块
    M.groups.erase(M.groups.begin());
    for(int i=0;i<M.groups.size();i++)
        M.groups[i].id-=M.groupSize;
    
    cout<<"物理块数:"<<M.blockNum<<",组大小:"<<M.groupSize<<",组数:"<<M.groupNum<<",初始化完成!"<<endl;
    cout<<"\n";
}

void allocate()
{
    string filename;
    int filesize;
    cout<<"输入文件名:";
    cin>>filename;
    while(F.find(filename)!=F.end())
    {
        cout<<"该文件名已存在!";
        cout<<"输入文件名:";
        cin>>filename;
    }

    cout<<"输入文件大小:";
    cin>>filesize;
    if(filesize==0)
    {
        cout<<"文件大小应该大于１!"<<endl;
        return;
    }
    int freeBlocks=0;
    freeBlocks+=S.N;
    for(int i=0;i<M.groups.size();i++)
        freeBlocks+=M.groups[i].N;
    if(freeBlocks<filesize)
     {
         cout<<"磁盘空间不足!"<<endl;
         return;
     }

    /*分配空间*/
    vector<int> fileblock;
    while(filesize)
    {
        if(S.N>1)
        {
            if(S.freeBlockID[0]==0)                             //如果是最后一组(多了一个０，特殊处理)
            {
                fileblock.push_back(S.freeBlockID[S.N]);             
                S.freeBlockID.erase(S.freeBlockID.begin()+S.N);   
                S.N--;   
            }
            else
            {
               S.N--;
               fileblock.push_back(S.freeBlockID[S.N]);             //分配的物理块加入文件目录
               S.freeBlockID.erase(S.freeBlockID.begin()+S.N);      //减少一个空闲块（最后一个）
            }
            
            
        }
        else if(S.N==1)
        {
            if(S.freeBlockID[0]==0)                             //最后一组，特殊处理
            {
                fileblock.push_back(S.freeBlockID[S.N]);  
                S.freeBlockID.erase(S.freeBlockID.begin()+S.N);   
                S.N--;                    
            }
            else
            {
                int tmp=S.freeBlockID[0];
                for(int i=0;i<M.groups.size();i++)                  //把将要分配的首盘块中的内容读出去
                    if(M.groups[i].id==S.freeBlockID[0])
                    {
                        S=M.groups[i];
                        S.id=-1;
                        M.groups.erase(M.groups.begin()+i);
                        break;
                     }
                fileblock.push_back(tmp);                           //分配的物理块加入文件目录
            }
            
            
        }
        filesize--;
    }
    F[filename]=fileblock;
}

void release()
{
    string filename;
    cout<<"输入要删除的文件名:";
    cin>>filename;
    while(F.find(filename)==F.end())
    {
        cout<<"该文件名不存在!";
        cout<<"输入文件名:";
        cin>>filename;
    }
    vector<int> tmp=F[filename];
    int filesize=tmp.size();
    while(filesize)
    {
        if(S.N==M.groupSize ||(S.N==M.groupSize-1 && S.freeBlockID[0]==0))    //当前组已满,将Ｓ中的内容移动到下一个要回收的盘块中！(S可能对应最后一组)
        {
            S.id=tmp[0];                        //将要回收的块作为新的组的首块
            M.groups.push_back(S);
            S.id=-1;
            S.freeBlockID.clear();
            S.freeBlockID.push_back(tmp[0]);
            tmp.erase(tmp.begin());
            S.N=1;
        }
        else
        {
           S.freeBlockID.push_back(tmp[0]);
           tmp.erase(tmp.begin());
           S.N++;
        }

        filesize--; 
    }
    F.erase(filename);
}

void display()
{
    cout<<"\n";
    /*显示空闲块信息,最后一块中的0代表没有下一组*/
    cout<<"序号\t物理块数\t块号(0代表最后一组)"<<endl;
    if(S.N!=0)                                                    //磁盘未分配完
    {
        cout<<"0\t"<<S.N<<"\t\t";                                 //S对应的是第一组信息
        for(int i=0;i<S.freeBlockID.size();i++)
            cout<<S.freeBlockID[i]<<" ";
        cout<<"\n";
    }
    
    for(int i=0;i<M.groups.size();i++)
    {
        cout<<i+1<<"\t"<<M.groups[i].N<<"\t\t";
        for(int j=0;j<M.groups[i].freeBlockID.size();j++)
            cout<<M.groups[i].freeBlockID[j]<<" ";
        cout<<"\n";
    }


    /*显示文件信息*/
    cout<<"文件名\t"<<"物理块号"<<endl;
    map<string, vector<int> >::iterator it;
    for(it=F.begin();it!=F.end();it++)
    {
        cout<<it->first<<"\t";
        for(int i=0;i<it->second.size();i++)
            cout<<(it->second)[i]<<" ";
        cout<<"\n";
    }

}


void dispatch()
{
    init();
    string ch;
    while(true)
    {
        cout<<"\n";
        cout<<"********************************* 输入 *********************************"<<endl;
        cout<<"A(allocate):创建文件"<<endl;
        cout<<"R(release):删除文件"<<endl;
        cout<<"D(display)：显示物理块情况"<<endl;
        cout<<"Q(quit)：退出"<<endl;
        cout<<"请输入:";
        cin>>ch;

        if(ch=="A")
            allocate();
        else if(ch=="R")
            release();
        else if(ch=="D")
            display();
        else if(ch=="Q")
            break;
        else
            cout<<"输入错误！"<<endl;
        
    }
}

int main()
{
    dispatch();
    cout<<"hello cg! the task is done!"<<endl;
    return 0;
}



