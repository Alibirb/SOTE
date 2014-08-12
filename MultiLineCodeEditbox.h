/// License for the the original files:

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

#ifndef MULTILINECODEEDITBOX_H
#define MULTILINECODEEDITBOX_H


#include <CEGUI/WindowRendererSets/Core/MultiLineEditbox.h>

#include "SyntaxHighlighter.h"

using namespace CEGUI;




/// Improved version of FalagardMultiLineEditbox to allow for syntax highlighting and other neat features.
class MultiLineCodeEditboxWindowRenderer : public MultiLineEditboxWindowRenderer
{
protected:
	SyntaxHighlighter* _highlighter;

public:
	static const String TypeName;     //! type name for this widget.

	//! Name of property to use to obtain unselected text rendering colour.
    static const String UnselectedTextColourPropertyName;
    //! Name of property to use to obtain selected text rendering colour.
    static const String SelectedTextColourPropertyName;
    //! Name of property to use to obtain active selection rendering colour.
    static const String ActiveSelectionColourPropertyName;
    //! Name of property to use to obtain inactive selection rendering colour.
    static const String InactiveSelectionColourPropertyName;
    //! The default timeout (in seconds) used when blinking the caret.
    static const float DefaultCaretBlinkTimeout;

public:
	MultiLineCodeEditboxWindowRenderer(const CEGUI::String& type);
	virtual ~MultiLineCodeEditboxWindowRenderer();

	CEGUI::Rectf getTextRenderArea() const;
	void render();
	void update(float elapsed);

    //! return whether the blinking caret is enabled.
    bool isCaretBlinkEnabled() const;
    //! return the caret blink timeout period (only used if blink is enabled).
    float getCaretBlinkTimeout() const;
    //! set whether the blinking caret is enabled.
    void setCaretBlinkEnabled(bool enable);
    //! set the caret blink timeout period (only used if blink is enabled).
    void setCaretBlinkTimeout(float seconds);

    /// overridden from base class
    bool handleFontRenderSizeChange(const Font* const font);

protected:

	CEGUI::Colour soteColorToCeguiColour(SyntaxHighlighter::Color soteColor);

	CEGUI::String addFormattingCodes(CEGUI::String unhighlightedText);

	/*!
    \brief
        Perform rendering of the widget control frame and other 'static' areas.  This
        method should not render the actual text.  Note that the text will be rendered
        to layer 4 and the selection brush to layer 3, other layers can be used for
        rendering imagery behind and infront of the text & selection..

    \return
        Nothing.
    */
    void cacheEditboxBaseImagery();

    /*!
    \brief
        Render the caret.

    \return
        Nothing
    */
    void cacheCaretImagery(const Rectf& textArea);

    /*!
    \brief
        Render text lines.
    */
    void cacheTextLines(const Rectf& dest_area);

    /*!
    \brief
        Set the given ColourRect to the colour to be used for rendering Editbox
        text oustside of the selected region.
    */
    void setColourRectToUnselectedTextColour(ColourRect& colour_rect) const;

    /*!
    \brief
        Set the given ColourRect to the colour to be used for rendering Editbox
        text falling within the selected region.
    */
    void setColourRectToSelectedTextColour(ColourRect& colour_rect) const;

    /*!
    \brief
        Set the given ColouRect to the colours to be used for rendering the
        selection highlight when the editbox is active.
    */
    void setColourRectToActiveSelectionColour(ColourRect& colour_rect) const;

    /*!
    \brief
        set the given ColourRect to the colours to be used for rendering the
        selection highlight when the editbox is inactive.
    */
    void setColourRectToInactiveSelectionColour(ColourRect& colour_rect) const;

    /*!
    \brief
        Set the given ColourRect to the colour(s) fetched from the named
        property if it exists, else the default colour of black.

    \param propertyName
        String object holding the name of the property to be accessed if it
        exists.

    \param colour_rect
        Reference to a ColourRect that will be set.
    */
    void setColourRectToOptionalPropertyColour(const String& propertyName,
                                               ColourRect& colour_rect) const;

    //! true if the caret imagery should blink.
    bool d_blinkCaret;
    //! time-out in seconds used for blinking the caret.
    float d_caretBlinkTimeout;
    //! current time elapsed since last caret blink state change.
    float d_caretBlinkElapsed;
    //! true if caret should be shown.
    bool d_showCaret;

    //! Class that renders RenderedString with some formatting.
	mutable FormattedRenderedString* d_formattedRenderedString;

};





/// Improved version of a CEGUI editbox that allows for syntax highlighting.
class MultiLineCodeEditbox : public MultiLineEditbox
{
public:
	//! 'Namespace' string used for global events on this class.
	static const CEGUI::String EventNamespace;
    //! String holding the type name of this widget.
    static const CEGUI::String WidgetTypeName;

public:
	MultiLineCodeEditbox(const CEGUI::String& type, const CEGUI::String& name);
	virtual ~MultiLineCodeEditbox();

protected:

};

#endif // MULTILINECODEEDITBOX_H
