#include<ctime>
#include<iostream> 
#include<cstdio>
#include<cstdlib> 
#include<chrono>
using namespace std;
using namespace std::chrono;
time_t a;
bool FH_flag,AFH_flag,ACK_flag;//???\???????\??????
int PNSeq[12],PN_CNT;//PN????
int SNR[80],RSSI[80],LOSS[80];
int hop_time;

/*	函 数 名: System_Init
*	功能说明: 通信功能初始化
*	形    参：无
*	返 回 值: 无*/
void System_Init()
{
	FH_flag = true; 
	AFH_flag = true;
	hop_time = 1000;
	
}

/*	函 数 名: AFH_PNSeq_Init
*	功能说明: PN序列初始化
*	形    参：无
*	返 回 值: 无*/
void AFH_PNSeq_Init(void)
{
	 
	for(int i=0;i<12;i++)PNSeq[i]=rand()%80+1;
	/*for(int i=0;i<12;i++)
		{
		printf("%d :%d\n",i+1,PNSeq[i]);
		}*/

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
	printf("rf_channel:%d hop_time:%d\n",rf_ch,1000-hop_time);
	printf("--SNR:%ddBm LOSS:%d% RSSI:-%d\n",SNR[rf_ch],LOSS[rf_ch],RSSI[rf_ch]);
	_sleep(50);
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

	if(AFH_stop > 913)
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
	System_Init();//通信系统初始化
	AFH_PNSeq_Init();//PN序列初始化
	
	while(hop_time)
	{
		chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
		hop_time--;
		if(FH_flag == true)//是否开启跳频
		{
			while(AFH_flag == true && ACK_flag == false)
			{
				AFH_LQA();
				ACK_flag = AFH_ACK_CHECK();//同步更新后的PN序列
			}
			ACK_flag = false;
			//信道选择
			if(PN_CNT<12)
			{
				AFH_Channelset(PNSeq[PN_CNT++]);	
			}
			else if(PN_CNT == 12)
			{
				AFH_PNSeq_Init();
				PN_CNT = 0;
				AFH_Channelset(PNSeq[PN_CNT]); 
			}
		}
		
		auto t2 = chrono::steady_clock::now();
		Data_transform();//数据包传输
		
		//cout << duration_cast<milliseconds>(t2 - t1).count() << " milliseconds." << endl;
		auto tt = std::chrono::duration_cast<microseconds>(t2 - t1);
		cout<<"time_cost:"<<tt.count()<<"us"<<endl;
		AFH_loop_refresh();//参数更新
	}
	system("pause");
	return 0;
}
