#ifndef SCRIPTSYNTAXHIGHLIGHTER_H
#define SCRIPTSYNTAXHIGHLIGHTER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qsyntaxhighlighter>
#include <qtextformat>

namespace Picto {

/*! \brief A syntax highlighter for Picto Scripts.
 *	\details Since there is no Qt API ScriptSyntaxHighlighter, we used a file from the QtScriptTools source
 *	for our own local QScript highlighting this code comes almost entirely from their.  
 *	Presumably, in the future a standard ScriptSyntaxHighlighter will be available in the API at which point 
 *	we can migrate to that one.
 *
 *	We will not include detailed documentation of this class, since we basically never looked at its code,
 *	pretty much all we do is call its constructor with the document that we want to highlight and it takes
 *	care of the rest.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class ScriptSyntaxHighlighter : public QSyntaxHighlighter
{
public:
    ScriptSyntaxHighlighter(QTextDocument *document = 0);
    ~ScriptSyntaxHighlighter();
	QString asHtml();

protected:
    void highlightBlock(const QString &text);

private:
    void highlightWord(int currentPos, const QString &buffer);

    enum ScriptFormats {
        ScriptTextFormat, ScriptNumberFormat,
        ScriptStringFormat, ScriptTypeFormat,
        ScriptKeywordFormat, ScriptPreprocessorFormat,
        ScriptLabelFormat, ScriptCommentFormat,
        NumScriptFormats
    };
    QTextCharFormat m_formats[NumScriptFormats];

private:
    Q_DISABLE_COPY(ScriptSyntaxHighlighter)
};

}; //namespace Picto

#endif
