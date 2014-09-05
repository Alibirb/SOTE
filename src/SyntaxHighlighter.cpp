#include "SyntaxHighlighter.h"


#include "globals.h"


/// Until GCC 4.9, the regex functions in libstdc++ were just stumps that returned false. Since we can't rely on everyone having a proper regex implementation in their standard library; we need to use boost instead.
#include <boost/regex.hpp>


/// Comparison function to sort HighlightingBlocks by start point
bool compareHighlightingBlocks(const SyntaxHighlighter::HighlightingBlock& first, const SyntaxHighlighter::HighlightingBlock& second)
{
	return (first.begin < second.begin);
}



SyntaxHighlighter::SyntaxHighlighter() : _needsParsing(true)
{
	Color red    	= Color(1, 0, 0, 1);
	Color green  	= Color(0, 1, 0, 1);
	Color blue   	= Color(0, 0, 1, 1);
	Color magenta	= Color(1, 0, 1, 1);
	Color yellow 	= Color(1, 1, 0, 1);

	Color white  	= Color(1, 1, 1, 1);
	Color black  	= Color(0, 0, 0, 1);
	Color grey   	= Color(.5, .5, .5, 1);

	_styles["default"].textColor = white;
	_styles["default"].backgroundColor = black;

	_styles["comment"].textColor = blue;
	_styles["comment"].backgroundColor = black;

	_styles["multiLineComment"].textColor = green;		/// This is only different from the "comment" style for testing purposes. Once the kinks are worked out, we'll just use one color for both single-line and multi-line comments.
	_styles["multiLineComment"].backgroundColor = black;

	_styles["default"].textColor = white;
	_styles["default"].backgroundColor = black;

	_styles["selected"].textColor = red;
	_styles["selected"].backgroundColor = white;

	_styles["literal"].textColor = magenta;
	_styles["literal"].backgroundColor = black;


	HighlightingRule singleLineCommentRule;
	singleLineCommentRule.regularExpression = "((//)|\\#).*?$";
	singleLineCommentRule.styleName = "comment";
	_highlightingRules.push_back(singleLineCommentRule);

	HighlightingRule multiLineCommentRule;
	multiLineCommentRule.regularExpression = "/\\*.*?\\*/";
	multiLineCommentRule.styleName = "multiLineComment";
	_highlightingRules.push_back(multiLineCommentRule);

	HighlightingRule numberRule;
	numberRule.regularExpression = "\\d";
	numberRule.styleName = "literal";
	_highlightingRules.push_back(numberRule);

	HighlightingRule stringLiteralRule;
	stringLiteralRule.regularExpression = "\".*?\"";
	stringLiteralRule.styleName = "literal";
	_highlightingRules.push_back(stringLiteralRule);


}

SyntaxHighlighter::~SyntaxHighlighter()
{
	//dtor
}

SyntaxHighlighter::Color SyntaxHighlighter::getTextColor(std::string styleName)
{
	if(!_styles.count(styleName.c_str()))
		logError("No syntax highlighting style named '" + styleName + "'\n");

	return _styles[styleName].textColor;
}
SyntaxHighlighter::Color SyntaxHighlighter::getBackgroundColor(std::string styleName)
{
	if(!_styles.count(styleName.c_str()))
		logError("No syntax highlighting style named '" + styleName + "'\n");

	return _styles[styleName].backgroundColor;
}

SyntaxHighlighter::HighlightedTextList SyntaxHighlighter::highlight(std::string unhighlightedText, int selectionStart, int selectionEnd)
{
	HighlightedTextList highlighting;


	std::list<HighlightingBlock> blocks;


	/// Search for sections matching each highlighting rule
	for(HighlightingRule rule : _highlightingRules)
	{

		boost::smatch match;
		boost::regex expression(rule.regularExpression);

		std::string::const_iterator searchStart, searchEnd;

		searchStart = unhighlightedText.begin();
		searchEnd = unhighlightedText.end();

		boost::match_results<std::string::const_iterator> results;
		boost::match_flag_type flags = boost::match_default;	/// Use the default regex format for boost (perl)

		while(boost::regex_search(searchStart, searchEnd, results, expression, flags))
		{
			HighlightingBlock block;
			block.begin = results[0].first - unhighlightedText.begin();
			block.end = results[0].second - unhighlightedText.begin();
			block.styleName = rule.styleName;

			blocks.push_back(block);

			searchStart = results[0].second;
		}

	}

	/// Sort the blocks based on starting point
	blocks.sort(compareHighlightingBlocks);

	/// Remove the blocks that start inside another block (comments do not count if they begin inside a string literal or another comment, et cetera)
	int endOfLastHighlight = 0;
	for(auto it = blocks.begin(); it != blocks.end(); ++it)
	{
		if(it->begin < endOfLastHighlight)
		{
			std::list<HighlightingBlock>::iterator it2 = it;	/// Removing the element will invalidate the iterator, so we need to make a copy that will point to the right place.
			--it2;
			blocks.erase(it);
			it = it2;
		}
			endOfLastHighlight = it->end;
	}




	/// Add a HighlightingOccurance for the currently-selected text if necessary.
	if(selectionStart >= 0)
	{

		HighlightingBlock selectedTextHighlighting;
		selectedTextHighlighting.begin = selectionStart;
		selectedTextHighlighting.end = selectionEnd;
		selectedTextHighlighting.styleName = "selected";

		std::list<HighlightingBlock>::iterator it = blocks.begin();

		/// Find the place in the list to put the highlighting for the selection.
		for( ; it != blocks.end(); ++it)
		{
			if(it->end > selectionStart)
			{
				/// The selection either overlaps with this highlighting block, or occurs between this one and the last one.
				break;
			}
		}

		/// Insert the highlighting for the selection, splitting existing highlights if needed.

		if( (it->begin < selectionStart) && (it->end > selectionEnd))
		{
			/// Our selection is completely inside this highlighting block. Must split the highlight into two parts.

			HighlightingBlock afterSelection;
			afterSelection.styleName = it->styleName;
			afterSelection.begin = selectionEnd;
			afterSelection.end = it->end;

			it->end = selectionStart;

			it++;
			blocks.insert(it, selectedTextHighlighting);
			blocks.insert(it, afterSelection);
		}
		else if( (it->begin < selectionStart) && (it->end > selectionStart))
		{
			/// Our selection starts inside this highlighting block, and continues to the end, or past it. So just shorten this highlighting block.
			it->end = selectionStart;

			++it;
			blocks.insert(it, selectedTextHighlighting);
		}
		else if( (it->begin < selectionEnd) && (it->end > selectionEnd))
		{
			/// Our selection overlaps with the beginning of this highlighting block. Trim the start of the highlighting block.
			it->begin = selectionEnd;

			blocks.insert(it, selectedTextHighlighting);
		}
		else
		{
			/// No overlap.
			blocks.insert(it, selectedTextHighlighting);
		}

	}


	/// Apply highlighting
	endOfLastHighlight = 0;
	for(auto block : blocks)
	{
		/// If this highlighting would occur inside another highlighting block, skip it.
		if(block.begin < endOfLastHighlight)
			continue;

		/// Add unhighlighted text between the last highlight and this highlight.
		HighlightedText beforeHighlighting;
		beforeHighlighting.styleName = "default";
		beforeHighlighting.text = unhighlightedText.substr(endOfLastHighlight, block.begin - endOfLastHighlight);
		highlighting.push_back(beforeHighlighting);

		/// Add the text for this highlight.
		HighlightedText section;
		section.styleName = block.styleName;
		section.text = unhighlightedText.substr(block.begin, block.end - block.begin);
		highlighting.push_back(section);

		endOfLastHighlight = block.end;
	}

	/// Add remaining text that doesn't get highlighted.
	HighlightedText afterHighlighting;
	afterHighlighting.styleName = "default";
	afterHighlighting.text = unhighlightedText.substr(endOfLastHighlight);
	highlighting.push_back(afterHighlighting);


	return highlighting;
}

