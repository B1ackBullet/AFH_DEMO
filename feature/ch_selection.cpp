#include<iostream>
#include<ctime>
using namespace std;

int PNSeq[12],PN_CNT;//PN序列相关参数
int CH_hop;//跳频阶跃
int CH_list[100],CH_CNT;//可用跳频信道集
bool CH_flag[100];
bool AFH_LQA(int ch)
{
    int x = rand()%1000;
    if(x%2 == 1)return false;
    return true;
}

void AFH_ch_sel(int mode,int ch_num)
{
    if(mode == 1)//使用配置1：直接映射
    {

        for(int i=0;i<12;i++)
        {
            PNSeq[i]=PNSeq[i]%ch_num;    
        }
   
    }

    else if (mode == 2)//使用配置2：预处理映射
    {
        
        CH_hop = 7;//设置跳频阶跃
        CH_CNT = 0;//初始化可用信道集计数

        //对全部信道进行信道质量评估并标记可用信道
        for(int i=0;i<=ch_num;i++)
        {
            if(AFH_LQA(i) == true) //信道可用，加入可用信道集
            {
                CH_list[CH_CNT++]=i;
                CH_flag[i] = true;
            }
            else 
            {
                CH_flag[i]= false;
            }
        }

        //对每个频点分别映射
        for(int i=0;i<12;i++)
        {
            PNSeq[i] = (PNSeq[i]+CH_hop)%ch_num;//计算临时跳频信道

            //判定临时信道是否可用
             if(CH_flag[PNSeq[i]] == false )//当前信道不可以，在可用信道集中进行映射
             {
                PNSeq[i]%=CH_CNT;
                PNSeq[i]=CH_list[PNSeq[i]];
             }


        }

   
    }
    

}
int main()
{
    srand(time(nullptr));
    cout<<"PNSeq Value:"<<endl;
    for(int i=0;i<12;i++)
    {
        PNSeq[i]=rand()%127;    
        cout<< PNSeq[i]<<" ";
    }
    cout<<endl;
    //使用模式1进行信道配置
    AFH_ch_sel(1,79);
    cout<<"Use Mode 1-->get PNSeq:"<<endl;
    for(int i=0;i<12;i++)
    {          
        cout<< PNSeq[i]<<" ";
    }
    cout<<endl;

    //使用模式2进行信道配置
    AFH_ch_sel(2,79);
    cout<<"Use Mode 2-->get PNSeq:"<<endl;
    for(int i=0;i<12;i++)
    {          
        cout<< PNSeq[i]<<" ";
    }
    cout<<endl;
    cout<<"CH_CNT:"<<CH_CNT<<endl;
    for(int i=0;i<CH_CNT;i++)
    {          
        cout<< CH_list[i]<<" ";
    }
    cout<<endl;
    system("pause");
    return 0;

}