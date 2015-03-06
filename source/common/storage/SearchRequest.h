#ifndef SEARCHREQUEST_H
#define SEARCHREQUEST_H

#include <QString>

/*! \brief Defines a request for a search.
 *	\details Different types of search can be defined.  We can look for query strings, the existance of scripts, or the existance of elements of a certain type.  We can
 *	also set what type of elements should be searched, Experimental, Active Analysis, Inactive Analysis, etc.  Other parameters that can be set are case sensitivity for
 *	query string searches and an enable parameter that can be used to disable or enable a particular search query.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct SearchRequest
{
	/*! \brief The group of elements to search, ie. Experimental elements, Active Analysis elements, Both, Inactive Analysis elements, etc.*/
	enum SearchGroup {INVALID_GROUP = 0x01,EXPERIMENT = 0x02,ACTIVE_ANALYSES = 0x04,EXP_AND_ACTIVE_ANALYSES = 0x06,INACTIVE_ANALYSES = 0x08,ALL_ANALYSES = 0x0C,EVERYTHING = 0x0E};
	/*! \brief The type of search to perform
	 *	- SCRIPT - Search for existance of scripts in of the current SearchGroup type.
	 *	- STRING - Search for a matching text string in the current SearchGroup type.
	 *	- EXISTS - Search for the the simple existance of something in the current SearchGroup type.
	 */
	enum SearchType {INVALID_TYPE = 0x10,SCRIPT=0x20,STRING=0x40,EXISTS=0x80};

	SearchRequest(){query = "";group = INVALID_GROUP; type = INVALID_TYPE;caseSensitive = false;enabled = false;};
	/*! \brief Constructs a SearchRequest object.
	 *	@param sGroup The SearchGroup to search.
	 *	@param sType The SearchType to use for this search.
	 *	@param en The Search is currently enabled.
	 *	@param searchQuery In the case of SearchType::String, this is the search query string.
	 *	@param cSensitive In the case of SearchType::String, this is whether the search should be case sensitive.
	 */
	SearchRequest(SearchGroup sGroup,SearchType sType,bool en = true,QString searchQuery = QString(),bool cSensitive = false){type = sType;group=sGroup;enabled=en;query = searchQuery;caseSensitive = cSensitive;};
	QString query;	//!< The current search query string, for the case of SearchType::String
	SearchGroup group;	//!< The SearchGroup to search.
	SearchType type;	//!< The SearchType to use for this search.
	bool caseSensitive; //!< In the case of SearchType::String, this is whether the search should be case sensitive.
	bool enabled;	//!< If set to false, the current search should be disabled.
	/*! \brief This convenience function returns a unique index for every combination of group and type.*/
	int getGroupTypeIndex(){return type|group;};
	/*! \brief This convenience function can be used to calculate a group type index based on input SearchGroup / SearchType values. */
	static int getGroupTypeIndex(SearchGroup g,SearchType t){return t|g;};
};


#endif
