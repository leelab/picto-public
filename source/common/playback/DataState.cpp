#include "DataState.h"
using namespace Picto;

template <> char DataState<char>::getPostMaxIndex()
{
	return maxIndex_+1;
}

template <> double DataState<double>::getPostMaxIndex()
{
	return maxIndex_;
}

template <> qulonglong DataState<qulonglong>::getPreMinIndex()
{
	return minIndex_ - 1;	
}

template <> double DataState<double>::getPreMinIndex()
{
	return minIndex_;
}