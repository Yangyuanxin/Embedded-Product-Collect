#ifndef _KALMAN_H_
#define _KALMAN_H_

extern	double KalmanGain;//  卡尔曼增益
extern	double EstimateCovariance;//估计协方差
extern	double MeasureCovariance;//测量协方差
extern	double  EstimateValue;//估计值

extern	void 	KalmanFilterInit(void);
extern	double KalmanFilter(double Measure);

#endif