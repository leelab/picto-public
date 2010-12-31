#include "SyntaxHighlighter.h"

#define USE_NEXT_CONSTRUCT "**"
enum{eBASE = -1, eENTITY, eSTARTTAG, eENDTAG, eSELFENDSLASH, eTAGNAME, eTAGPROPERTY, eCOMMENT, eCOMMENTTEXT, eSTRING, 
	eOPENPARENTHESIS, eCLOSEPARENTHESIS, eNUMCONSTRUCTS};
SyntaxHighlighter::SyntaxHighlighter(QObject *parent)
        : QSyntaxHighlighter(parent)
{
		Construct myConstruct;
		//Define base construct - don't touch this.  It is the lowest level construct.  It should never be added to anything.
		myConstruct.format_.setForeground(QColor(0, 0, 0));
		myConstruct.opener_ = "";
		myConstruct.closer_ = "";
		constructs_[eBASE] = myConstruct;

		// Define Constructs - Put any new constructs here
		// Note that an empty opener or closer ("") indicates that it starts or ends immediately (ie. If the current string
		// of size 0 equals nothing, which is always true). A closer of USE_NEXT_CONSTRUCT indicates that this construct goes on forever
		// until the next construct down in the stack closes.
        myConstruct.format_.setForeground(QColor(206, 17, 0));
        myConstruct.format_.setFontWeight(QFont::Bold);
		myConstruct.opener_ = "&";
		myConstruct.closer_ = ";";
		constructs_[eENTITY] = myConstruct;
		
        myConstruct.format_.setForeground(QColor(0, 60, 135));
        myConstruct.format_.setFontWeight(QFont::Bold);
		myConstruct.opener_ = "<";
		myConstruct.closer_ = ">";
		constructs_[eSTARTTAG] = myConstruct;

		myConstruct.format_.setForeground(QColor(0, 60, 135));
        myConstruct.format_.setFontWeight(QFont::Bold);
		myConstruct.opener_ = "</";
		myConstruct.closer_ = ">";
		constructs_[eENDTAG] = myConstruct;

		myConstruct.format_.setForeground(QColor(0, 60, 135));
        myConstruct.format_.setFontWeight(QFont::Bold);
		myConstruct.opener_ = "/";
		myConstruct.closer_ = "";
		constructs_[eSELFENDSLASH] = myConstruct;

		myConstruct.format_.setForeground(QColor(132, 33, 0));
        myConstruct.format_.setFontWeight(QFont::Bold);
		myConstruct.opener_ = "";
		myConstruct.closer_ = USE_NEXT_CONSTRUCT;
		constructs_[eTAGNAME] = myConstruct;

		myConstruct.format_.setForeground(QColor(206, 17, 0));
        myConstruct.format_.setFontItalic(true);
		myConstruct.opener_ = " ";
		myConstruct.closer_ = USE_NEXT_CONSTRUCT;
		constructs_[eTAGPROPERTY] = myConstruct;

		myConstruct.format_.setForeground(QColor(0, 60, 135));
        myConstruct.format_.setFontItalic(true);
		myConstruct.opener_ = "<!--";
		myConstruct.closer_ = "-->";
		constructs_[eCOMMENT] = myConstruct;

		myConstruct.format_.setForeground(QColor(0, 79, 21));
        myConstruct.format_.setFontItalic(true);
		myConstruct.opener_ = "";
		myConstruct.closer_ = USE_NEXT_CONSTRUCT;
		constructs_[eCOMMENTTEXT] = myConstruct;

		myConstruct.format_.setForeground(QColor(0, 60, 135));
        myConstruct.format_.setFontItalic(true);
		myConstruct.opener_ = "\"";
		myConstruct.closer_ = "\"";
		constructs_[eSTRING] = myConstruct;

		myConstruct.format_.setForeground(QColor(0, 0, 0));
        myConstruct.format_.setFontWeight(QFont::Bold);
		myConstruct.opener_ = "(";
		myConstruct.closer_ = "";
		constructs_[eOPENPARENTHESIS] = myConstruct;

		myConstruct.format_.setForeground(QColor(0, 0, 0));
        myConstruct.format_.setFontWeight(QFont::Bold);
		myConstruct.opener_ = ")";
		myConstruct.closer_ = "";
		constructs_[eCLOSEPARENTHESIS] = myConstruct;

		//Setup subconstructs - define which constructs may appear within which constructs here
		constructs_[eSTARTTAG].subConstructs_.push_back(eTAGNAME);
		constructs_[eENDTAG].subConstructs_.push_back(eTAGNAME);
		constructs_[eTAGNAME].subConstructs_.push_back(eTAGPROPERTY);
		constructs_[eTAGNAME].subConstructs_.push_back(eSELFENDSLASH);
		constructs_[eTAGPROPERTY].subConstructs_.push_back(eSTRING);
		constructs_[eTAGPROPERTY].subConstructs_.push_back(eSELFENDSLASH);
		constructs_[eSELFENDSLASH].subConstructs_.push_back(eSTRING);
		constructs_[eCOMMENT].subConstructs_.push_back(eCOMMENTTEXT);
		constructs_[eBASE].subConstructs_.push_back(eENTITY);
		constructs_[eBASE].subConstructs_.push_back(eSTARTTAG);
		constructs_[eBASE].subConstructs_.push_back(eENDTAG);
		constructs_[eBASE].subConstructs_.push_back(eCOMMENT);
		constructs_[eBASE].subConstructs_.push_back(eOPENPARENTHESIS);
		constructs_[eBASE].subConstructs_.push_back(eCLOSEPARENTHESIS);



		rehighlight();
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
	// Copy construct stack from previous text block to the current text block
	ConstructStack* prevData = (ConstructStack*)currentBlock().previous().userData();
	ConstructStack* myData = new ConstructStack();	// We setCurrentBlockUserData() with this an QT takes care of deleting it.
	if(prevData)
		(*myData) = *prevData;

	// If the construct stack is empty, put the BASE construct in it.
	if(!myData->size())
	{
		ConstructFlag baseFlag;
		baseFlag.id_ = eBASE;
		baseFlag.start_ = 0;
		myData->push(baseFlag);
	}

	// Set up constants and variables
    int len = text.length();
    int pos = 0;
	QString matchStr;
	ConstructStack::iterator it;
	// All stacked constructs from the previous text block effectively start at pos = 0 for this text block
	for(it = myData->begin();it!=myData->end();it++)
	{
		it->start_ = pos;
	}
	while (pos < len) 
	{	// Loop through text block.  In the context of the construct on top of the stack there are permissable 
		// start strings that start new constructs and stop strings that stop this construct.  Search for these
		// strings and format accordingly.

		ConstructFlag flag;
		flag.start_ = -2;	// -2 indicates no match
		foreach(int constructID,constructs_[myData->top().id_].subConstructs_)
		{	// Loop through all constructs that can be within the top construct, looking for their start string. Keep the longest one that is allowed.
			matchStr = constructs_[constructID].opener_;
			if(text.mid(pos,matchStr.length()) == matchStr)
			{	// We found this match string.  Check if this works as a new construct 
				if((flag.start_<0) || (matchStr.length() > constructs_[flag.id_].opener_.length()))
				{	
					flag.id_ = constructID;
					flag.start_ = pos;
				}
			}
		}
		if(myData->size() > 1)
		{	// If the top construct in the stack isn't the BASE construct, look for its end string
			matchStr = constructs_[myData->top().id_].closer_;
			for(it = myData->end()-1;it != myData->begin();it--) 
			{	// Move down stack until we find construct who closer can be used
				matchStr = constructs_[it->id_].closer_;
				if(matchStr != USE_NEXT_CONSTRUCT)
					break;
			}
			if((matchStr != USE_NEXT_CONSTRUCT) && (text.mid(pos,matchStr.length()) == matchStr))
			{	//We found the end of the top construct without empty end string.  Check that it works better than anything else we found
				if((flag.start_<0) || (matchStr.length() >= constructs_[flag.id_].opener_.length()))
				{
					flag.start_ = -1; //-1 indicates that we should pop the top construct off the stack
					flag.id_ = it->id_;											
				}
			}
		}
		
		if(flag.start_>-2)
		{	// We found a matched opener or closer
			if(flag.start_>-1)
			{	// It was a matched opener
				
				// Set the previous top constructs format up until here
				setFormat(myData->top().start_, pos - myData->top().start_, constructs_[myData->top().id_].format_);
				myData->push(flag);
				pos += constructs_[flag.id_].opener_.length();
			}
			else
			{	// It was a matched closer.  Move down stack, setting formats accordingly until the one who'se closer string was matched is found.
				do{
					it = myData->end()-1;
					pos += (constructs_[it->id_].closer_ == USE_NEXT_CONSTRUCT)?0:constructs_[it->id_].closer_.length();
					setFormat(it->start_, pos - it->start_, constructs_[it->id_].format_);
					myData->pop();
					myData->top().start_=pos;
				}while(it->id_ != flag.id_);
			}
		}
		else
		{	// We didn't match any opener or closers.  Keep moving through text block
			pos++;			
		}
	}
	// We reached the end of the text block. Use the current construct to format everything to the end of the block
	setFormat(myData->top().start_, pos - myData->top().start_, constructs_[myData->top().id_].format_);

	setCurrentBlockUserData(myData);
}