#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <list>
#include <string>
#include <unordered_map>



/// Class to perform syntax highlighting.
/**
	Known bugs:
		If a single-line comment openner ("//") is inside a string literal or a multi-line comment, and another single-line comment openner is on the same line, after the string literal or multi-line comment, the second single-line comment will not be highlighted. This is because after the first openner is found, the second one is ignored, even though it's the one that we actually want highlighted.
		The selected text is highlighted by changing the text color, not the background color. Apparently CEGUI doesn't have a format tag to change the background color.
 */
class SyntaxHighlighter
{
public:

	/// Struct containing a highlighted section of text and the name of the style to apply to it.
	struct HighlightedText
	{
		std::string text;
		std::string styleName;
	};
	typedef std::list<HighlightedText> HighlightedTextList;

	struct Color
	{
		float red, green, blue, alpha;
		Color() {}
		Color(float red, float green, float blue, float alpha=1.0) : red(red), green(green), blue(blue), alpha(alpha) {}
	};

	struct Style
	{
		Color textColor;
		Color backgroundColor;	/// NOTE: Background color is currently unimplemented. I thought CEGUI accounted for it in the formatting tags system, but apparently it doesn't.
	};

	struct HighlightingRule
	{
		std::string regularExpression;	/// Regular expression to match
		std::string styleName;
	};

	struct HighlightingBlock
	{
		int begin;
		int end;
		std::string styleName;
	};

protected:
	HighlightedTextList _highlightedText;

	std::unordered_map<std::string, Style> _styles;
	std::list<HighlightingRule> _highlightingRules;

	bool _needsParsing;	/// Whether we need to re-parse and re-highlight the text.


public:
	SyntaxHighlighter();
	virtual ~SyntaxHighlighter();

	/** Highlight the given text.
	 * Can optionally specify start and end points for the currently-selected text so that gets highlighted as well.
	 */
	HighlightedTextList highlight(std::string unhighlightedText, int selectionStart = -1, int selectionEnd = -1);

	Color getTextColor(std::string styleName);
	Color getBackgroundColor(std::string styleName);



protected:

};

#endif // SYNTAXHIGHLIGHTER_H
