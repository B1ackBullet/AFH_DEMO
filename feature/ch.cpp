#include<iostream>
#include<ctime>
#include<algorithm>
#include<set>
#include"mseq.h"
#include"ch.h"
#include"mseq.cpp"
using namespace std;

int PN_CNT;//PN序列相关参数
int CH_hop;//跳频阶跃
int CH_CNT;//可用跳频信道集
int RSSI_stl,SNR_stl,PRR_stl;//标准阈值，低于该标准表示信道质量不合格
double k1,k2,k3,CH_fa_stl;//信道因子权重，信道因子标准阈值
set<int> CH_list;


struct channel
{
    int loss_cnt;//信道不合格次数集
    int rssi,snr,prr;
    int status;
};

channel CH_info[100];

int AFH_LQA_get_rssi(int ch_num)
{
    int x = rand()%100;
    CH_info[ch_num].rssi=x;
    return x;
}

int AFH_LQA_get_snr(int ch_num)
{
    int x = rand()%100;
    CH_info[ch_num].snr=x;
    return x;
}

int AFH_LQA_get_prr(int ch_num)
{
    int x = rand()%100;
    CH_info[ch_num].prr=x;
    return x;
}

bool AFH_LQA(int mode,int ch_num)
{
    int x = rand()%1000;
    if(mode == 1)//使用配置1：使用阈值判定
    {
        if( AFH_LQA_get_prr(ch_num) < PRR_stl &&
            AFH_LQA_get_rssi(ch_num) < RSSI_stl &&
            AFH_LQA_get_snr(ch_num) < SNR_stl)return false;//PRR、RSSI、SNR均不达标
        return true;
    }

    if(mode == 2)//使用配置2：使用信道影响因子判定
    {
        if( AFH_LQA_get_prr(ch_num) < PRR_stl &&
            AFH_LQA_get_rssi(ch_num) < RSSI_stl &&
            AFH_LQA_get_snr(ch_num) < SNR_stl)return false;//PRR、RSSI、SNR均不达标

        if( k1*AFH_LQA_get_prr(ch_num) + 
            k2*AFH_LQA_get_rssi(ch_num) + 
            k3*AFH_LQA_get_snr(ch_num) < CH_fa_stl)return false;//信道质量因子不达标
        return true;
    }

    return true;

}

void AFH_ch_init(int ch_tot)//初始化信道相关参数
{
    for(int i=0;i<=ch_tot;i++)
    {
        CH_info[i].loss_cnt = 0;
        CH_info[i].rssi = AFH_LQA_get_rssi(i);
        CH_info[i].snr = AFH_LQA_get_snr(i);
        CH_info[i].prr = AFH_LQA_get_prr(i);
        if(AFH_LQA(1,i) == true) 
            { 
                CH_info[i].status = 1;//标记信道状态：可用
            }
        else 
            {   
                CH_info[i].status = 0;//标记信道状态：不可用
            }
    }
}
void AFH_ch_sel(int mode,int ch_tot)
{
    if(mode == 1)//使用配置1：直接映射
    {
        for(int i=0;i<12;i++)
        {
            PNSeq[i]=PNSeq[i]%ch_tot;    
            CH_info[PNSeq[i]].status = 2;//标记信道状态：已用
        }   
    }

    else if (mode == 2)//使用配置2：预处理映射
    {       
        CH_hop = 7;//设置跳频阶跃
        CH_CNT = 0;//初始化可用信道集计数

        //对全部信道进行信道质量评估并标记可用信道

        CH_list.clear();
        for(int i=0;i<=ch_tot;i++)
        {
            if(AFH_LQA(1,i) == true) //信道可用，加入可用信道集
            {
                CH_list.insert(i);
                CH_info[i].status = 1;//标记信道状态：可用
            }
            else 
            {
                CH_info[i].status = 0;//标记信道状态：不可用
            }
        }
        
        //对每个频点分别映射
        for(int i=0;i<12;i++)
        {
            PNSeq[i] = (PNSeq[i]+CH_hop)%ch_tot;//计算临时跳频信道

            //判定临时信道是否可用
             if(CH_info[PNSeq[i]].status == 0)//当前信道不可用，在可用信道集中进行映射
             {
                PNSeq[i]%=CH_list.size();
                CH_CNT = CH_list.size();
                for(set<int>::iterator it = CH_list.begin();it != CH_list.end();it++)
                {
                    CH_CNT--;
                    if(CH_CNT == 0)
                    {
                        PNSeq[i] = *it;
                        break;
                    }
                }
                          
             }
             CH_info[PNSeq[i]].status = 2;//标记信道状态：已用
             CH_list.erase(PNSeq[i]);
           
        }  
    }    
}

int AFH_ch_update(int mode,int ch_num,int ch_tot)
{
    if(AFH_LQA(1,ch_num) == false)
    {  
        CH_info[ch_num].loss_cnt++;//如果当前信道质量不合格，次数+1
    }    
    else 
    { 
        CH_info[ch_num].loss_cnt = 0;//如果合格，次数清零
    }
    

    if(CH_info[ch_num].loss_cnt >= 3)//如果该频点连续三次不合格，优化该频点
    {
        CH_info[ch_num].status = 0;//标记信道状态：不可用
        if(mode == 1)//使用配置1：替换该频点
        {
            set<int>::iterator it = CH_list.begin();
            ch_num = *it;//在可用信道集中选择第一个信道（按通信质量排序）更新
            CH_info[ch_num].status = 2;//标记信道状态：已用
            CH_list.erase(ch_num);//在可用信道集中剔除已用信道
        }        
    }
    return ch_num;//返回更新后的信道编号
}
int main()
{
    srand(time(nullptr));
    /*for(int i=0;i<=79;i++)
    {
        cout<<i<<"-----"
            <<CH_info[i].loss_cnt<<" "
            <<CH_info[i].status<<" "
            <<CH_info[i].rssi<<" "
            <<CH_info[i].snr<<" "
            <<CH_info[i].prr<<" "
            <<endl;
    }
    */

    //利用mseq.h生成m序列
    mseq_main();

    //初始化相关参数
    AFH_ch_init(79);
    RSSI_stl = 50;
    SNR_stl = 50;
    PRR_stl = 50;
    CH_fa_stl = 50;
    k1 = 0.1;
    k2 = 0.3;
    k3 = 0.6;

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


    cout<<"CH_CNT:"<<CH_list.size()<<endl;
    for(set<int>::iterator it = CH_list.begin();it != CH_list.end();it++)
    {          
        cout<< *it<<" ";
    }
    cout<<endl;

    while (1)//模拟每次跳频更新
    {
        for(int i = 0 ; i < 12;i++)
        {
            cout<<PNSeq[i]<<" ";//更新前的信道编号
            PNSeq[i] = AFH_ch_update(1,PNSeq[i],79);
            cout<<PNSeq[i]<<" ";//更新后的信道编号
        }
        cout<<endl;
        system("pause");
    }
    
    
    //cout<<mseq_updata();
    system("pause");
    return 0;

}