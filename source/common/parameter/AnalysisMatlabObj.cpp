#include "AnalysisMatlabObj.h"
#include "../memleakdetect.h"

#include <QLabel>
#include <QTimer>
#include <QFile>

//Matlab
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <sstream>
#include <iostream>
#include <vector>


// Matlab Coder
#include "binocdfCoderWrap.h"
#include "binocdfCoderWrap_emxAPI.h"
#include "binopdfCoderWrap.h"
#include "randCoderWrap.h"
#include "normrndCoderWrap.h"


using namespace std;

namespace Picto {

AnalysisMatlabObj::AnalysisMatlabObj()
	: AnalysisVariable()
{
	doneInit_ = false;
}
AnalysisMatlabObj::~AnalysisMatlabObj()
{
}

QSharedPointer<Asset> AnalysisMatlabObj::Create()
{
	return QSharedPointer<Asset>(new AnalysisMatlabObj());
}
void AnalysisMatlabObj::postDeserialize()
{
	AnalysisVariable::postDeserialize();
}

bool AnalysisMatlabObj::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if (!AnalysisVariable::validateObject(xmlStreamReader))
		return false;
	return true;
}
void AnalysisMatlabObj::enteredScope()
{
	AnalysisVariable::enteredScope();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////MATLAB CODER////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
QVariantList AnalysisMatlabObj::binocdfCoder(QVariantList in1, QVariantList in2, QVariantList in3)
{
	// Create input data	
	vector<double> in1data;
	for (int i = 0; i < in1.size(); i++)
	{
		in1data.push_back(double(in1.at(i).toDouble()));
	}
	emxArray_real_T *in1Arr;
	in1Arr = emxCreateWrapper_real_T(&in1data[0], 1, in1data.size());

	// Create input data	
	vector<double> in2data;
	for (int i = 0; i < in2.size(); i++)
	{
		in2data.push_back(double(in2.at(i).toDouble()));
	}
	emxArray_real_T *in2Arr;
	in2Arr = emxCreateWrapper_real_T(&in2data[0], 1, in2data.size());

	// Create input data	
	vector<double> in3data;
	for (int i = 0; i < in3.size(); i++)
	{
		in3data.push_back(double(in3.at(i).toDouble()));
	}
	emxArray_real_T *in3Arr;
	in3Arr = emxCreateWrapper_real_T(&in3data[0], 1, in3data.size());

	// Create output array
	emxArray_real_T* outArr;	
	outArr = emxCreate_real_T(1, in1data.size());

	// Call the library function
	binocdfCoderWrap(in1Arr, in2Arr, in3Arr, outArr);

	QVariantList resultVar;
	for (int i = 0; i < in1data.size(); i++)
	{
		resultVar.append(outArr->data[i]);
	}

	return resultVar;
}
QVariantList AnalysisMatlabObj::binopdfCoder(QVariantList in1, QVariantList in2, QVariantList in3)
{
	// Create input data	
	vector<double> in1data;
	for (int i = 0; i < in1.size(); i++)
	{
		in1data.push_back(double(in1.at(i).toDouble()));
	}
	emxArray_real_T *in1Arr;
	in1Arr = emxCreateWrapper_real_T(&in1data[0], 1, in1data.size());

	// Create input data	
	vector<double> in2data;
	for (int i = 0; i < in2.size(); i++)
	{
		in2data.push_back(double(in2.at(i).toDouble()));
	}
	emxArray_real_T *in2Arr;
	in2Arr = emxCreateWrapper_real_T(&in2data[0], 1, in2data.size());

	// Create input data	
	vector<double> in3data;
	for (int i = 0; i < in3.size(); i++)
	{
		in3data.push_back(double(in3.at(i).toDouble()));
	}
	emxArray_real_T *in3Arr;
	in3Arr = emxCreateWrapper_real_T(&in3data[0], 1, in3data.size());

	// Create output array
	emxArray_real_T* outArr;
	outArr = emxCreate_real_T(1, in1data.size());

	// Call the library function
	binopdfCoderWrap(in1Arr, in2Arr, in3Arr, outArr);

	QVariantList resultVar;
	for (int i = 0; i < in1data.size(); i++)
	{
		resultVar.append(outArr->data[i]);
	}

	return resultVar;
}
QVariantList AnalysisMatlabObj::randCoder(double in)
{
	// Create output array
	emxArray_real_T* outArr;
	int outSize = in*in;
	outArr = emxCreate_real_T(1, outSize); //1*16

	// Call the library function
	randCoderWrap(in, outArr);

	QVariantList resultVar;
	for (int i = 0; i < in; i++) //rows
	{
		QVariantList colVar;
		for (int j = 0; j < in; j++) //cols
		{			
			int index = j + i*in;
			colVar.append(outArr->data[index]);
		}
		resultVar.push_back(colVar);
	}

	return resultVar;
}
QVariantList AnalysisMatlabObj::normrndCoder(QVariantList mu, QVariantList sigma)
{
	// Create input data		
	vector<double> in1data;
	for (int i = 0; i < mu.size(); i++)
	{
		in1data.push_back(double(mu.at(i).toDouble()));
	}
	emxArray_real_T *in1Arr;
	in1Arr = emxCreateWrapper_real_T(&in1data[0], 1, in1data.size());

	// Create input data	
	vector<double> in2data;
	for (int i = 0; i < sigma.size(); i++)
	{
		in2data.push_back(double(sigma.at(i).toDouble()));
	}
	emxArray_real_T *in2Arr;
	in2Arr = emxCreateWrapper_real_T(&in2data[0], 1, in2data.size());

	// Create output array	
	emxArray_real_T* outArr;
	outArr = emxCreate_real_T(1, mu.size());

	// Call the library function
	normrndCoderWrap(in1Arr,in2Arr, outArr);
	
	QVariantList resultVar;
	for (int i = 0; i < mu.size(); i++)
	{
		resultVar.append(outArr->data[i]);
	}

	return resultVar;
}
}; //namespace Picto


