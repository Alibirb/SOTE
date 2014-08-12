
/// License for the the original file:

/***************************************************************************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/


#include "MultiLineCodeEditbox.h"



#include <CEGUI/falagard/WidgetLookManager.h>
#include <CEGUI/falagard/WidgetLookFeel.h>
#include <CEGUI/WindowManager.h>
#include <CEGUI/widgets/Scrollbar.h>
#include <CEGUI/PropertyHelper.h>
#include <CEGUI/Image.h>
#include <CEGUI/TplWindowRendererProperty.h>


#include "CEGUI/LeftAlignedRenderedString.h"
#include "CEGUI/RightAlignedRenderedString.h"
#include "CEGUI/CentredRenderedString.h"
#include "CEGUI/JustifiedRenderedString.h"
#include <CEGUI/RenderedStringWordWrapper.h>

#include <iostream>


using namespace CEGUI;


const String MultiLineCodeEditboxWindowRenderer::TypeName("MultiLineCodeEditbox");



const String MultiLineCodeEditboxWindowRenderer::UnselectedTextColourPropertyName( "NormalTextColour" );
const String MultiLineCodeEditboxWindowRenderer::SelectedTextColourPropertyName( "SelectedTextColour" );
const String MultiLineCodeEditboxWindowRenderer::ActiveSelectionColourPropertyName( "ActiveSelectionColour" );
const String MultiLineCodeEditboxWindowRenderer::InactiveSelectionColourPropertyName( "InactiveSelectionColour" );
const float MultiLineCodeEditboxWindowRenderer::DefaultCaretBlinkTimeout(0.66f);

MultiLineCodeEditboxWindowRenderer::MultiLineCodeEditboxWindowRenderer(const String& type) : MultiLineEditboxWindowRenderer(type),
d_blinkCaret(false),
    d_caretBlinkTimeout(DefaultCaretBlinkTimeout),
    d_caretBlinkElapsed(0.0f),
    d_showCaret(true)
{

    CEGUI_DEFINE_WINDOW_RENDERER_PROPERTY(MultiLineCodeEditboxWindowRenderer,bool,
        "BlinkCaret", "Property to get/set whether the Editbox caret should blink.  "
        "Value is either \"True\" or \"False\".",
        &MultiLineCodeEditboxWindowRenderer::setCaretBlinkEnabled,&MultiLineCodeEditboxWindowRenderer::isCaretBlinkEnabled,
        false);
    CEGUI_DEFINE_WINDOW_RENDERER_PROPERTY(MultiLineCodeEditboxWindowRenderer,float,
        "BlinkCaretTimeout", "Property to get/set the caret blink timeout / speed.  "
        "Value is a float value indicating the timeout in seconds.",
        &MultiLineCodeEditboxWindowRenderer::setCaretBlinkTimeout,&MultiLineCodeEditboxWindowRenderer::getCaretBlinkTimeout,
        0.66f);

	_highlighter = new SyntaxHighlighter();
}


MultiLineCodeEditboxWindowRenderer::~MultiLineCodeEditboxWindowRenderer()
{
	//dtor
}


CEGUI::Colour MultiLineCodeEditboxWindowRenderer::soteColorToCeguiColour(SyntaxHighlighter::Color soteColor)
{
	return CEGUI::Colour(soteColor.red, soteColor.green, soteColor.blue, soteColor.alpha);
}

CEGUI::String MultiLineCodeEditboxWindowRenderer::addFormattingCodes(CEGUI::String unhighlightedText)
{
	MultiLineEditbox* w = (MultiLineEditbox*)d_window;

	SyntaxHighlighter::HighlightedTextList textList = _highlighter->highlight(unhighlightedText.c_str(), w->getSelectionStartIndex(), w->getSelectionEndIndex());
	CEGUI::String textWithCode = "";

	for(auto segment : textList)
	{
		/// Before we add formatting tags to the string, we need to escape any '[' characters so that CEGUI doesn't mistake those for formatting tags.
		int i = 0;
		while(i < segment.text.size())
		{
			i = segment.text.find("[", i);
			if(i >= segment.text.size())
				break;

			segment.text.insert(i, "\\");
			i += 2;	/// advance the iterator by two characters so we don't try to escape this occurance again.
		}

		/// Now we insert actual formatting tags if needed.

		CEGUI::Colour textColor = soteColorToCeguiColour(_highlighter->getTextColor(segment.styleName));

		std::stringstream stream;

		stream << "[colour='" << std::hex << textColor.getARGB() << "']";

		textWithCode += stream.str();
		textWithCode += segment.text;
	}
	return textWithCode;
}

Rectf MultiLineCodeEditboxWindowRenderer::getTextRenderArea(void) const
{
    MultiLineEditbox* w = (MultiLineEditbox*)d_window;
    const WidgetLookFeel& wlf = getLookNFeel();
    bool v_visible = w->getVertScrollbar()->isVisible();
    bool h_visible = w->getHorzScrollbar()->isVisible();

    // if either of the scrollbars are visible, we might want to use another text rendering area
    if (v_visible || h_visible)
    {
        String area_name("TextArea");

        if (h_visible)
        {
            area_name += "H";
        }
        if (v_visible)
        {
            area_name += "V";
        }
        area_name += "Scroll";

        if (wlf.isNamedAreaDefined(area_name))
        {
            return wlf.getNamedArea(area_name).getArea().getPixelRect(*w);
        }
    }

    // default to plain TextArea
    return wlf.getNamedArea("TextArea").getArea().getPixelRect(*w);
}

void MultiLineCodeEditboxWindowRenderer::cacheEditboxBaseImagery()
{
	MultiLineEditbox* w = (MultiLineEditbox*)d_window;
	const StateImagery* imagery;

	// get WidgetLookFeel for the assigned look.
	const WidgetLookFeel& wlf = getLookNFeel();
	// try and get imagery for our current state
	imagery = &wlf.getStateImagery(w->isEffectiveDisabled() ? "Disabled" : (w->isReadOnly() ? "ReadOnly" : "Enabled"));
	// peform the rendering operation.
	imagery->render(*w);
}

void MultiLineCodeEditboxWindowRenderer::cacheCaretImagery(const Rectf& textArea)
{
    MultiLineEditbox* w = (MultiLineEditbox*)d_window;
    const Font* fnt = w->getFont();

    // require a font so that we can calculate caret position.
    if (fnt)
    {
        // get line that caret is in
        size_t caretLine = w->getLineNumberFromIndex(w->getCaretIndex());

        const MultiLineEditbox::LineList& d_lines = w->getFormattedLines();

        // if caret line is valid.
        if (caretLine < d_lines.size())
        {
            // calculate pixel offsets to where caret should be drawn
            size_t caretLineIdx = w->getCaretIndex() - d_lines[caretLine].d_startIdx;
            float ypos = caretLine * fnt->getLineSpacing();
            float xpos = fnt->getTextAdvance(w->getText().substr(d_lines[caretLine].d_startIdx, caretLineIdx));

//             // get base offset to target layer for cursor.
//             Renderer* renderer = System::getSingleton().getRenderer();
//             float baseZ = renderer->getZLayer(7) - renderer->getCurrentZ();

            // get WidgetLookFeel for the assigned look.
            const WidgetLookFeel& wlf = getLookNFeel();
            // get caret imagery
            const ImagerySection& caretImagery = wlf.getImagerySection("Caret");

            // calculate finat destination area for caret
            Rectf caretArea;
            caretArea.left(textArea.left() + xpos);
            caretArea.top(textArea.top() + ypos);
            caretArea.setWidth(caretImagery.getBoundingRect(*w).getSize().d_width);
            caretArea.setHeight(fnt->getLineSpacing());
            caretArea.offset(Vector2f(-w->getHorzScrollbar()->getScrollPosition(), -w->getVertScrollbar()->getScrollPosition()));

            // cache the caret image for rendering.
            caretImagery.render(*w, caretArea, 0, &textArea);
        }
    }
}

void MultiLineCodeEditboxWindowRenderer::render()
{
    MultiLineEditbox* w = (MultiLineEditbox*)d_window;
    // render general frame and stuff before we handle the text itself
    cacheEditboxBaseImagery();

    // Render edit box text
    Rectf textarea(getTextRenderArea());
    cacheTextLines(textarea);

    // draw caret
    if ((w->hasInputFocus() && !w->isReadOnly()) &&
        (!d_blinkCaret || d_showCaret))
            cacheCaretImagery(textarea);
}


void MultiLineCodeEditboxWindowRenderer::cacheTextLines(const CEGUI::Rectf& dest_area)
{
	MultiLineEditbox* w = (MultiLineEditbox*)d_window;

	const Rectf clipper(getTextRenderArea());
	Rectf absarea(clipper);

	CEGUI::String highlightedString = addFormattingCodes(w->getText());

	BasicRenderedStringParser* renderedStringParser = new BasicRenderedStringParser();

	RenderedString renderedString = renderedStringParser->parse(highlightedString, w->getFont(), 0);

	d_formattedRenderedString = CEGUI_NEW_AO RenderedStringWordWrapper<LeftAlignedRenderedString>(renderedString);

	d_formattedRenderedString->format(d_window, dest_area.getSize());


	/// adjust y positioning according to formatting option
	float textHeight = d_formattedRenderedString->getVerticalExtent(d_window);
	const Scrollbar* const vertScrollbar = w->getVertScrollbar();
	const float vertScrollPosition = vertScrollbar->getScrollPosition();
	/// if scroll bar is in use, position according to that.
	if (vertScrollbar->isEffectiveVisible())
		absarea.d_min.d_y -= vertScrollPosition;

	/// calculate final colours
   	const float alpha = w->getEffectiveAlpha();
	ColourRect normalTextCol;
	setColourRectToUnselectedTextColour(normalTextCol);
	normalTextCol.modulateAlpha(alpha);
	ColourRect final_cols(normalTextCol);

	d_formattedRenderedString->draw(d_window, d_window->getGeometryBuffer(), absarea.getPosition(), &final_cols, &clipper);
}

void MultiLineCodeEditboxWindowRenderer::setColourRectToUnselectedTextColour(
                                                ColourRect& colour_rect) const
{
    setColourRectToOptionalPropertyColour(UnselectedTextColourPropertyName,
                                          colour_rect);
}

//----------------------------------------------------------------------------//
void MultiLineCodeEditboxWindowRenderer::setColourRectToSelectedTextColour(
                                                ColourRect& colour_rect) const
{
    setColourRectToOptionalPropertyColour(SelectedTextColourPropertyName,
                                          colour_rect);
}

//----------------------------------------------------------------------------//
void MultiLineCodeEditboxWindowRenderer::setColourRectToActiveSelectionColour(
                                                ColourRect& colour_rect) const
{
    setColourRectToOptionalPropertyColour(ActiveSelectionColourPropertyName,
                                          colour_rect);
}

//----------------------------------------------------------------------------//
void MultiLineCodeEditboxWindowRenderer::setColourRectToInactiveSelectionColour(
                                                ColourRect& colour_rect) const
{
    setColourRectToOptionalPropertyColour(InactiveSelectionColourPropertyName,
                                          colour_rect);
}

//----------------------------------------------------------------------------//
void MultiLineCodeEditboxWindowRenderer::setColourRectToOptionalPropertyColour(
                                                const String& propertyName,
                                                ColourRect& colour_rect) const
{
    if (d_window->isPropertyPresent(propertyName))
        colour_rect =
            d_window->getProperty<ColourRect>(propertyName);
    else
        colour_rect.setColours(0);
}

//----------------------------------------------------------------------------//
void MultiLineCodeEditboxWindowRenderer::update(float elapsed)
{
    // do base class stuff
    WindowRenderer::update(elapsed);

    // only do the update if we absolutely have to
    if (d_blinkCaret &&
        !static_cast<MultiLineEditbox*>(d_window)->isReadOnly() &&
        static_cast<MultiLineEditbox*>(d_window)->hasInputFocus())
    {
        d_caretBlinkElapsed += elapsed;

        if (d_caretBlinkElapsed > d_caretBlinkTimeout)
        {
            d_caretBlinkElapsed = 0.0f;
            d_showCaret ^= true;
            // state changed, so need a redraw
            d_window->invalidate();
        }
    }
}

//----------------------------------------------------------------------------//
bool MultiLineCodeEditboxWindowRenderer::isCaretBlinkEnabled() const
{
    return d_blinkCaret;
}

//----------------------------------------------------------------------------//
float MultiLineCodeEditboxWindowRenderer::getCaretBlinkTimeout() const
{
    return d_caretBlinkTimeout;
}

//----------------------------------------------------------------------------//
void MultiLineCodeEditboxWindowRenderer::setCaretBlinkEnabled(bool enable)
{
    d_blinkCaret = enable;
}

//----------------------------------------------------------------------------//
void MultiLineCodeEditboxWindowRenderer::setCaretBlinkTimeout(float seconds)
{
    d_caretBlinkTimeout = seconds;
}

//----------------------------------------------------------------------------//
bool MultiLineCodeEditboxWindowRenderer::handleFontRenderSizeChange(const Font* const font)
{
    const bool res = WindowRenderer::handleFontRenderSizeChange(font);

    if (d_window->getFont() == font)
    {
        d_window->invalidate();
        static_cast<MultiLineEditbox*>(d_window)->formatText(true);
        return true;
    }

    return res;
}





const String MultiLineCodeEditbox::EventNamespace("MultiLineCodeEditbox");
const String MultiLineCodeEditbox::WidgetTypeName("MultiLineCodeEditbox");

MultiLineCodeEditbox::MultiLineCodeEditbox(const CEGUI::String& type, const CEGUI::String& name) : MultiLineEditbox(type, name)
{
}


MultiLineCodeEditbox::~MultiLineCodeEditbox()
{
}

