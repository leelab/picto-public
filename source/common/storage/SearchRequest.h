#ifndef SEARCHREQUEST_H
#define SEARCHREQUEST_H

#include <QString>

//! [0]
struct SearchRequest
{
	//The group of elements to search, ie. Experimental elements, Analysis elements, Both.
	enum SearchGroup {INVALID_GROUP = 0x01,EXPERIMENT = 0x02,ANALYSIS = 0x04,ALL = 0x06,};
	//SCRIPT - Search for existance of scripts
	//STRING - Search for text string
	enum SearchType {INVALID_TYPE = 0x10,SCRIPT=0x20,STRING=0x40};

	SearchRequest(){query = "";group = INVALID_GROUP; type = INVALID_TYPE;caseSensitive = false;enabled = false;};
	SearchRequest(SearchGroup sGroup,SearchType sType,bool en = true,QString searchQuery = QString(),bool cSensitive = false){type = sType;group=sGroup;enabled=en;query = searchQuery;caseSensitive = cSensitive;};
	QString query;
	SearchGroup group;
	SearchType type;
	bool caseSensitive;
	//If set to false, the current search of type, searchType should be disabled.
	bool enabled;
	//This convenience function returns a unique index for depending on the combination of group and type.
	int getGroupTypeIndex(){return type|group;};
	//This convenience function can be used to calculate a group type index based on input values
	static int getGroupTypeIndex(SearchGroup g,SearchType t){return t|g;};
};
//! [0]

#endif
