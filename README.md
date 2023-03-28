# AFH_DEMO更新日志

## [v1.0] -2023-3-17
1. 跳频框架搭建完成
	- main( )
	- System_Init( )
	- AFH_PNSeq_Init( )
	- AFH_LQA( )
	- AFH_Channelset( )
	- Data_transform( )
	- AFH_ACK_CHECK( )
2. 添加运行时间测试代码time_cost
3. 添加AFH_loop_refresh( )函数，用于完成参数更新及中断处理

## [v1.1]-2023-3-24
1. pn序列生成算法编写完成
	- AFH_PNSeq_Init( )
	- Mseq_Updata( )
2. 增加随机反馈系数功能