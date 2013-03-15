#include "SearchableTextEdit.h"
#include "../../common/memleakdetect.h"

SearchableTextEdit::SearchableTextEdit(QWidget *parent) :
QTextEdit(parent)
{
}

void SearchableTextEdit::search(SearchRequest searchRequest, QColor highlightColor)
{
    QList<ExtraSelection> extraSelections;
	if(!searchRequest.enabled)
	{
		return setExtraSelections(extraSelections);
	}
	QTextCursor startingPos = textCursor();
    moveCursor(QTextCursor::Start);
    QColor color = highlightColor;
 
	ExtraSelection extra;
	extra.format.setBackground(QBrush(color));
    while	(
				(searchRequest.caseSensitive && find(searchRequest.query,QTextDocument::FindCaseSensitively)) ||
				(!searchRequest.caseSensitive && find(searchRequest.query))
			
			)
    {
        extra.cursor = textCursor();
        extraSelections.append(extra);
    }
	setExtraSelections(extraSelections);
	setTextCursor(startingPos);
}