/*
 * Copyright 2008, The Android Open Source Project
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "pvplugin.h"

#ifndef formPlugin__DEFINED
#define formPlugin__DEFINED

struct TextInput
{
    ANPRectF    rect;
    char        text[30];
    uint32_t    charPtr;
};

class FormPlugin : public SubPlugin
{
public:
    FormPlugin(NPP inst);
    virtual ~FormPlugin();
    virtual bool supportsDrawingModel(ANPDrawingModel);
    virtual int16_t handleEvent(const ANPEvent *evt);
private:
    void draw(ANPCanvas *);
    void drawPlugin(const ANPBitmap &bitmap, const ANPRectI &clip);

    bool        m_hasFocus;

    TextInput  *m_activeInput;
    TextInput   m_usernameInput;
    TextInput   m_passwordInput;

    ANPPaint   *m_paintInput;
    ANPPaint   *m_paintActive;
    ANPPaint   *m_paintText;

    ANPRectI    m_visibleRect;

    void drawText(ANPCanvas *, TextInput);
    void drawPassword(ANPCanvas *, TextInput);

    bool handleNavigation(ANPKeyCode keyCode);
    void handleTextInput(TextInput *input, ANPKeyCode keyCode, int32_t unichar);
    void scrollIntoView(TextInput *input);
    void switchActiveInput(TextInput *input);

    ANPPaint *getPaint(TextInput *);
    TextInput *validTap(int x, int y);

};

#endif // formPlugin__DEFINED
