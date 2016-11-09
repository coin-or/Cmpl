/***********************************************************************
 *  This code is part of CMPL
 *
 *  Copyright (C) 2007 - 2016
 *  Thomas Schleiff - Halle(Saale), Germany and
 *  Mike Steglich - Technical University of Applied Sciences
 *  Wildau, Germany
 *
 *  Coliop and CMPL are projects of the Technical University of
 *  Applied Sciences Wildau.
 *  Please visit the project homepage <www.coliop.org>
 *
 *  CMPL is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  CMPL is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include <QtGui>

#include "CmplHighlighter.hh"


CmplHighlighter::CmplHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    sectionFormat.setFontWeight(QFont::Bold);
    sectionFormat.setForeground(Qt::darkMagenta);
    QStringList sectionPatterns;
    sectionPatterns << "\\bparameters\\b" << "\\bvariables\\b" << "\\bobjectives\\b" << "\\bconstraints\\b";
    foreach (const QString &pattern, sectionPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = sectionFormat;
        highlightingRules.append(rule);
    }

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b[a-z]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);



    //numbers
    numbersFormat.setForeground(Qt::darkYellow);
    rule.pattern = QRegExp("\\b([+-]?[0-9]*\.?[0-9]+([eE][+-]?[0-9]+)?)");
    rule.format = numbersFormat;
    highlightingRules.append(rule);

    //non quoted string in cmplData
   /* cmplDataStringFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("<([\\D*\\d+\\s+]+)>");
    rule.format = cmplDataStringFormat;
    highlightingRules.append(rule);*/


    //Cmpl Header
    cmplHeaderFormat.setForeground(Qt::darkCyan);
    cmplHeaderFormat.setFontWeight(QFont::Bold);
    QStringList headerPatterns;
    headerPatterns << "%arg" << "%display" << "%data" << "%opt";
    foreach (const QString &pattern, headerPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = cmplHeaderFormat;
        highlightingRules.append(rule);
    }
    //rule.pattern = QRegExp("%[^\n]*");
    //rule.format = cmplHeaderFormat;
    //highlightingRules.append(rule);


    //keywords
    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bstring\\b" << "\\bbinary\\b" << "\\binteger\\b" << "\\breal\\b" << "\\bset\\b"
                    << "\\bmax\\b" << "\\bmin\\b"
                    << "\\belement\\b" << "\\bin\\b"
                    << "\\default\\b" << "\\bbreak\\b" << "\\cbontinue\\b" << "\\brepeat\\b"
                    << "\\becho\\b" << "\\berror\\b"
                    << "\\bsum\\b" << "\\bdiv\\b" << "\\bmod\\b" << "\\bT\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    //Strings
    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\"[^\"]*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);



    //Comments
    singleLineCommentFormat.setForeground(Qt::darkGray);
    rule.pattern = QRegExp("#[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    //Comments
    multiLineCommentFormat.setForeground(Qt::darkGray);
    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");

}


void CmplHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);


    while (startIndex >= 0) {

        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}

