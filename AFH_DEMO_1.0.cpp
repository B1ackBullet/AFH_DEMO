#include<ctime>
#include<iostream> 
#include<cstdio>
#include<cstdlib> 
#include<chrono>
#include<list>
#include<cmath>
#include<cstring>
using namespace std;
using namespace std::chrono;
time_t a;
bool FH_flag,AFH_flag,ACK_flag;//???\???????\??????
int PNSeq[12],PN_CNT;//PN????
int SNR[200],RSSI[200],LOSS[200];
int hop_time;

//PN序列相关变量
list<int> m_register;
list<int>::iterator it;
list<int>::reverse_iterator rit;
char coef[]="10000011";//设置反馈系数
char start[]="1000000";//设置初值

/*	函 数 名: System_Init
*	功能说明: 通信功能初始化
*	形    参：无
*	返 回 值: 无*/
void System_Init()
{
	FH_flag = true; //开启跳频功能
	AFH_flag = true;//开启LQA模块
	hop_time = 1000;//设置跳速
}

/*	函 数 名: Mseq_Updata
*	功能说明: 更新m序列相关参数
*	形    参：无
*	返 回 值: 对应的信道*/
int Mseq_Updata(void)
{
		//根据反馈系数相加并取模
		it=m_register.begin();
		int i=0;
		int top_temp=0;//新高位变量
		int rfch_temp;//信道对应值变量
		while(it!=m_register.end())
		{
			top_temp+=*it*(coef[i]-48);
			//cout<<top_temp<<" ";
			it++;
			i++;
		}
		top_temp=top_temp%2;
	
		//更新m序列状态
		m_register.pop_back();
		m_register.push_front(top_temp);

		//查询目前寄存器状态
		/*it=m_register.begin();
		while(it!=m_register.end())
		{
			cout<<*it<<" ";
			it++;
		}
		cout<<endl;*/

		//查询目前寄存器对应的值
		rit=m_register.rbegin();
		i=0;
		rfch_temp=0;
		while(rit!=m_register.rend())
		{
			rfch_temp+=*rit*pow(2,i++);
			//cout<<rfch_temp<<" ";
			rit++;			
		}
		//cout<<rfch_temp<<" ";
		//cout<<endl;		
		//system("pause");
		return rfch_temp;
}
/*	函 数 名: AFH_PNSeq_Init
*	功能说明: PN序列初始化
*	形    参：无
*	返 回 值: 无*/
void AFH_PNSeq_Init(int mode)
{
	 if(mode == 1)//模式1：m序列生成
	 {
		if(m_register.empty() == true)//寄存器为空，初始化并赋值
		{
			//随机反馈系数 随机范围0~63
			int x=rand()%63;
			int i=strlen(coef)-2;//从C2开始
			//cout<<x<<endl;
			while(x)//将x转换为二进制并修改反馈系数
			{
				if(x%2)coef[i]='1';
				else coef[i]='0';
				x/=2;
				i--;
			}
			//cout<<coef<<endl;
			for(int i = 0; i < strlen(start); i++)m_register.push_back(start[i]-48);
		}
		//进行12次更新填充
		for(int i = 0; i < 12; i++)
		{
			PNSeq[i]=Mseq_Updata();
			cout<<PNSeq[i]<<" ";
		}
		cout<<endl;
	 }
	

}

/*	函 数 名: AFH_LQA
*	功能说明: 信道质量评估
*	形    参：无
*	返 回 值: 无*/
void AFH_LQA(void)
{
	for(int i=0;i<12;i++)
	{
		SNR[PNSeq[i]]=rand()%100;
		LOSS[PNSeq[i]]=rand()%100;
		RSSI[PNSeq[i]]=rand()%100;
	}
}

/*	函 数 名: AFH_Channelset
*	功能说明: 信道切换/跳频操作
*	形    参：rf_ch：切换的信道编号
*	返 回 值: 无*/
void AFH_Channelset(int rf_ch)
{
	//切换到rf_ch编号信道
	//打印信道信息
	printf("rf_channel:%d \n",rf_ch);
	printf("--SNR:%ddBm LOSS:%d% RSSI:-%d\n",SNR[rf_ch],LOSS[rf_ch],RSSI[rf_ch]);
	//_sleep(50);
} 

/*	函 数 名: Data_transform
*	功能说明: 数据传输
*	形    参：无
*	返 回 值: 无*/
void Data_transform(void)
{
		a=time(NULL);
		char *time = ctime(&a);
		cout<<"Data:"<<time<<endl;
}

/*	函 数 名: AFH_loop_refresh
*	功能说明: 更新参数
*	形    参：无
*	返 回 值: 无*/
void AFH_loop_refresh(void)
{
	int AFH_stop= rand()%1000;

	if(FH_flag == true && AFH_stop == 913)//检测到停止跳频信号
	{
		FH_flag = false;
		printf("FH System has stopped\n");
		system("pause");
	}

}
/*	函 数 名: AFH_ACK_CHECK
*	功能说明: 同步信号确认
*	形    参：无 
*	返 回 值: 0：代表同步失败，1：同步成功*/
bool AFH_ACK_CHECK(void)
{
	return true;//??????
}


int main()
{	

	srand(time(nullptr));//生成rand()的随机数种子
	System_Init();//通信系统初始化
	AFH_PNSeq_Init(1);//PN序列初始化
	
	while(hop_time)
	{
		chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
		hop_time--;
		if(FH_flag == true)//是否开启跳频
		{
			while(AFH_flag == true && ACK_flag == false)
			{
				AFH_LQA();//信道质量评估，调整跳频图案
				ACK_flag = AFH_ACK_CHECK();//同步更新后的PN序列
			}
			ACK_flag = false;
			//信道选择，待修改
			if(PN_CNT<12)
			{
				AFH_Channelset(PNSeq[PN_CNT++]);	
			}
			else if(PN_CNT == 12)
			{
				AFH_PNSeq_Init(1);//重新生成新的PN序列
				AFH_Channelset(PNSeq[PN_CNT]); 
				PN_CNT = 0;
			}
		}
		
		auto t2 = chrono::steady_clock::now();
		Data_transform();//数据包传输
		
		//cout << duration_cast<milliseconds>(t2 - t1).count() << " milliseconds." << endl;
		auto tt = std::chrono::duration_cast<microseconds>(t2 - t1);
		cout<<"time_cost:"<<tt.count()<<"us"<<endl;
		
		cout<<"hop_time:"<<1000-hop_time<<endl;
		//system("pause");      
		AFH_loop_refresh();//参数更新
	}
	system("pause");
	return 0;
}
