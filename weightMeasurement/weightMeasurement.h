#ifndef weightMeasurement_H
#define weightMeasurement_H


class weightMeasurement{
	public:
	
	int scaleID;
	int clkID;
	double* data;
	double weight;

	weightMeasurement(int);
	~weightMeasurement();	
	double measureWeight();
	double avgData();

};
 
#endif /* weightMeasurement_H */
