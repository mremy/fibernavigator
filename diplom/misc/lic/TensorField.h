/*
 * TensorField.h
 *
 *  Created on: 16.11.2008
 *      Author: ralph
 */

#ifndef TENSORFIELD_H_
#define TENSORFIELD_H_

#include "../../dataset/DatasetHelper.h"
#include "../Fantom/FTensor.h"

class TensorField {
public:
	TensorField(DatasetHelper* dh, float* tensorData, int order, int posDim);
	virtual ~TensorField();

	int getCells() 	{ return m_cells; };
	int getOrder() 	{ return m_order; };
	int getPosDim()	{ return m_posDim; };

	FTensor getTensorAtIndex(int index) {return theField[index];};
	FTensor getInterpolatedVector(const float x, const float y, const float z);

private:
	DatasetHelper* m_dh;
	std::vector<FTensor> theField;

	FMatrix createMatrix(FTensor lhs, FTensor rhs);


	int m_cells;
	int m_order;
	int m_posDim;
};

#endif /* TENSORFIELD_H_ */