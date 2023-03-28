#include<iostream>
#include<cmath>
#include<cstring>
#include<list>
#include<ctime>
using namespace std;

list<int> m_register;
list<int>::iterator it;
list<int>::reverse_iterator rit;
char coef[]="10000011";//设置反馈系数
char start[]="1000000";//设置初值

int mseq_updata(void)
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
		cout<<rfch_temp<<" ";
		//cout<<endl;		
		//system("pause");
		return rfch_temp;
}
int main()
{
	
	int i=0;
	//随机反馈系数 随机范围0~63
	int x=rand()%63;
	i=strlen(coef)-2;//从C2开始
	cout<<x<<endl;
	while(x)
	{
		if(x%2)coef[i]='1';
		else coef[i]='0';
		x/=2;
		i--;
	}
	cout<<coef<<endl;
	for(i=0;i<strlen(start);i++)
	{
		m_register.push_back(start[i]-48);
	}



	i=pow(2,strlen(start));
	while (i)
	{
		mseq_updata();/* code */
		i--;
	}
	
	
	
	
	/*Mseq m1(coef,start);//采用有初值的构造函数
	m1.displaycoef();//显示反馈系数
	m1.displayregist();//显示寄存器里的值
	m1.creat_mseq();//生成m序列
	m1.displayresualt();//显示结果*/
    system("pause");
    return 0;
}