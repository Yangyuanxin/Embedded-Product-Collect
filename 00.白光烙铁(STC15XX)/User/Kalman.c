#include "config.h"
#include "math.h"

double KalmanGain;//  卡尔曼增益
double EstimateCovariance;//估计协方差
double MeasureCovariance;//测量协方差
double EstimateValue;//估计值

void 	KalmanFilterInit(void)
{
	EstimateValue=0;
	EstimateCovariance=0.1;
	MeasureCovariance=0.02;
}

double KalmanFilter(double Measure)
{
	//计算卡尔曼增益
	KalmanGain = EstimateCovariance * sqrt(1/(EstimateCovariance*EstimateCovariance+MeasureCovariance*MeasureCovariance));

	//计算本次滤波估计值
	EstimateValue = (EstimateValue + KalmanGain*(Measure-EstimateValue));
	//更新估计协方差

	EstimateCovariance=sqrt(1-KalmanGain)*EstimateCovariance;
	//更新测量方差
	MeasureCovariance=sqrt(1-KalmanGain)*MeasureCovariance;
	
	//返回估计值
	return EstimateValue;
}