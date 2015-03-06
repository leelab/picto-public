#ifndef _PSEUDOPseudorandomIntParameter_H_
#define _PSEUDOPseudorandomIntParameter_H_

#include "../common.h"

#include "parameter.h"
#include "../random/MersenneTwister.h"

namespace Picto {

/*!	\brief A Parameter for selecting Pseudo-Random Integers
 *	\details In computer science, the term Pseudo Random refers to the fact that
 *	randomization on a computer is never truly random but extremely random like, more
 *	or less so depending on which algorithm is used.  In our experiments, the idea of
 *	Pseudo-Randomness is different.  When we say Pseudo-Random here what we mean is that
 *	we have a list of possible values that can be returned by a randomization system and
 *	that they must all be returned once before any of them are returned twice.  We are
 *	therefore essentially creating a list and then selecting values out of that list until
 *	the list dissappears, the rebuilding the list and starting again.  This is useful for 
 *	maintaining a certain level of randomness while simultaneously assuring that all cases
 *	are occuring with the same overall frequency.
 *
 *	An example of Pseudo-Randomness could be the Pseudo-Random selection of values between 1 and 3.
 *	Possibilities are:
 *		- 1,2,3
 *		- 1,3,2
 *		- 2,1,3
 *		- 2,3,1
 *		- 3,1,2
 *		- 3,2,1
 *
 *	The following are some possiblities that are NOT allowed:
 *		- 1,1,3
 *		- 3,1,3
 *		- 2,2,2
 *
 *	Objects of the PseudorandomIntParameter class support this Pseudo-Random selection paradigm.  The range of 
 *	returnable values is integers from a set Min to a set Max.  Scripts call randomize()
 *	to select a new value from the list of allowable values and make it available on the
 *	"value" script property.  When all of the values from Min to Max have been used up, randomization
 *	will start over with a new list.
 *
 *	Additional options are also supported beyond what we have described above.  Values can be 
 *	reshuffled if scripts would like the latest value to appear again before random selection
 *	is finished for the latest range of allowed values.  A non-repetition Property can be 
 *	set to assure that the same value will never appear twice in a row unless it is
 *	the last value in the list of allowable values and it is being reshuffled.
 *
 *	For more detail on the algorithm used by the PseudorandomIntParameter, see randomize().
 *
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API PseudorandomIntParameter : public Parameter
#else
class PseudorandomIntParameter : public Parameter
#endif
{
	Q_OBJECT
	/*! \brief Returns the latest pseudorandom value that was selected when randomize() was called.
	 *	\details Setting this value is also possible and will cause the set value to be returned from the value
	 *	script property until it is manually changed, or randomize() is next called.
	*/
	Q_PROPERTY(int value READ getValue WRITE setValue)
	/*! \brief Sets/Gets the minimum random value that may be returned from this Parameter.
	 *	\details Changing this value will cause randomization to reset as if randomize() had not yet been called.
	 */
	Q_PROPERTY(int min READ getMin WRITE setMin)
	/*! \brief Sets/Gets the maximum random value that may be returned from this Parameter.
	 *	\details Changing this value will cause randomization to reset as if randomize() had not yet been called.
	 */
	Q_PROPERTY(int max READ getMax WRITE setMax)
public slots:
	void randomize();
	void reshuffleLastValue();
	void reset();

public:
	PseudorandomIntParameter();
	virtual ~PseudorandomIntParameter(){};

	static Parameter* NewParameter();
	static QSharedPointer<Asset> Create();
	virtual void enteredScope();
	virtual QString friendlyTypeName(){return "Pseudorandom Int";};
	virtual QString getUITemplate(){return "PseudorandomInt";};

	/*! \brief Returns the latest pseudorandom value that was selected when randomize() was called
	 *	or that was set using setValue()
	*/
	int getValue(){return propertyContainer_->getPropertyValue("Value").toInt();};
	/*! \brief Sets the value that will be returned from getValue() until this function or randomize() is 
	 *	next called.
	*/
	void setValue(int val){propertyContainer_->setPropertyValue("Value",val);};

	virtual bool valuesAreValid(QString& warning = QString());
	virtual void fixValues();

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	void checkForPropertyChanges();
	int getMin();
	//Note: changing the values of min or max cause this object to reset its data.
	void setMin(int min);
	int getMax();
	//Note: changing the values of min or max cause this object to reset its data.
	void setMax(int max);
	MTRand mtRand_;
	QVector<int> randomArray_;
	bool useSeed_;
	int seed_;
	int value_;
	int min_;
	int max_;
	QString units_;
	int currIndex_;
};


}; //namespace Picto

#endif
