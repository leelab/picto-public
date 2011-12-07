#ifndef _SYNTAX_EDITOR_H_
#define _SYNTAX_EDITOR_H_

#include <QSyntaxHighlighter>
#include <QStack>

class QAction; 

/*! \brief A syntax highlighter used by the xmleditor.
 *
 *	This class extends the QSyntaxHighlighter to add syntax highlighting
 *	to the xmleditor.  Everytime a textblock is edited, QT calls highlightBlock
 *  with the QString that was highlighted (the line of text in which the change
 *	was made).  The class is setup to be fairly general, such that it will be
 *	easy to add new types of highlighting in the future.  The basic concept 
 *	is built on "Construct" objects.  A Construct contains an opener string, 
 *	a closer string, a format for the text between the opener and closer, and a 
 *	list of constructs that are allowed to exist between the opener and closer.
 *	Every text block maintains a stack of Constructs where the Construct at the 
 *	top of the stack is the one who's formatting is currently being applied.  
 *	text blocks always build up their stack based on that of the previous text
 *	block such that the whole document is consistant.  Constructs can also use
 *	empty openers to indicate that a Construct should be applied as soon as a 
 *	Construct that allows it as a subConstruct is added.  Empty Closers can be
 *	used to indicate that a Construct ends immediately after its opener.  Also,
 *	A special USE_NEXT_CONSTRUCT string can be used as a closer to indicate that
 *	a Construct does not end until the Construct beneath it in the stack ends.
 *
 *	For some quick examples:
 *	To highlight <NAME param="val"> with red: < and >, blue: NAME param=
 *	and green "val", a TAG construct would be set up with opener: < and
 *	closer: > and red formatting.  It would have a subConstruct TAGNAME with
 *	opener:	"" and closer: USE_NEXT_CONSTRUCT and blue formatting.  That would
 *	have a subConstruct STRING with opener: " and closer: " and green formatting.
 *	All these definitions are placed in the SyntaxHighlighter constructor, which 
 *	can easily be modified by copying and pasting existing Construct definitions 
 *	and editing their contents.
 */

 class SyntaxHighlighter : public QSyntaxHighlighter
    {
        Q_OBJECT
    
    public:
    
        SyntaxHighlighter(QObject *parent);
		virtual ~SyntaxHighlighter(){};
    protected:
    
        void highlightBlock(const QString &text);

		struct Construct
		{
			QString opener_;
			QString closer_;
			QTextCharFormat format_;
			QVector<int> subConstructs_;
		};
		struct ConstructFlag
		{
			int id_;
			int start_;
		};
		struct ConstructStack : public QTextBlockUserData, QStack<ConstructFlag>
		{
		};
    
    private:
		int FindEndOfState(const QString &text, int pos, int len, QString& closer);
        QMap<int, Construct> constructs_;
    };

#endif