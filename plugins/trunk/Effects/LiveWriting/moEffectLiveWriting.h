/*******************************************************************************

                              moEffectLiveWriting.h

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *  (at your option) any later version.                                    *
  *                                                                          *
  *   This code is distributed in the hope that it will be useful, but       *
  *   WITHOUT ANY WARRANTY; without even the implied warranty of             *
  *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
  *   General Public License for more details.                               *
  *                                                                          *
  *   A copy of the GNU General Public License is available on the World     *
  *   Wide Web at <http://www.gnu.org/copyleft/gpl.html>. You can also       *
  *   obtain it by writing to the Free Software Foundation,                  *
  *   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.         *
  *                                                                          *
  ****************************************************************************

  Copyright(C) 2006 Fabricio Costa

  Authors:
  Fabricio Costa
  Andrés Colubri

*******************************************************************************/

#ifndef __MO_EFFECT_LIVEWRITING_H__
#define __MO_EFFECT_LIVEWRITING_H__

#include "moPlugin.h"



enum moTextParamIndex {
	LIVEWRITING_ALPHA,
	LIVEWRITING_COLOR,
	LIVEWRITING_SYNC,
	LIVEWRITING_PHASE,
	LIVEWRITING_MODE,
	LIVEWRITING_LAYOUTS,
	LIVEWRITING_FONT,
	LIVEWRITING_TEXT,
	LIVEWRITING_BLENDING,
	LIVEWRITING_WIDTH,
	LIVEWRITING_HEIGHT,
	LIVEWRITING_TRANSLATEX,
	LIVEWRITING_TRANSLATEY,
  LIVEWRITING_SCALEX,
	LIVEWRITING_SCALEY,
	LIVEWRITING_ROTATE,
  LIVEWRITING_INLET,
	LIVEWRITING_OUTLET
};


typedef char moLiveChar;


moDeclareDynamicArray( moLiveChar, moLiveChars );

class moLivePhrase : public moAbstract {

    public:

        moLivePhrase();
        virtual ~moLivePhrase();
        moLivePhrase &operator = (const moLivePhrase &src);

        void Add( moLiveChar c ); ///< add a char
        void Delete(); ///< remove last
        void LeftCursor();
        void RightCursor();

    protected:

        int         m_CursorPosition;
        moLiveChars m_Chars;

};

moDeclareDynamicArray( moLivePhrase*, moLivePhrases );

class moLiveParagraph : public moAbstract {

    public:

        moLiveParagraph();
        virtual ~moLiveParagraph();
        moLiveParagraph &operator = (const moLiveParagraph &src);

    protected:

        moLivePhrases m_Phrases;

};

moDeclareDynamicArray( moLiveParagraph*, moLiveParagraphs );



typedef moVector4f  moLiveColor;

class moLiveFrameZone : public moAbstract {

    public:

        enum borders {
          LEFT=0,
          RIGHT=1,
          TOP=2,
          BOTTOM=3,
          BORDERS=4
        };

        moLiveFrameZone();
        virtual ~moLiveFrameZone();
        moLiveFrameZone &operator = (const moLiveFrameZone &src);

    protected:

        ///moLiveFrames

        ///layer size and position
        float       position[BORDERS];
        float       margin[BORDERS];

        ///paddings
        float       padding[BORDERS];

        ///sizes
        float       width;
        float       height;


        ///layer borders
        float       border_type[BORDERS];
        float       border_size[BORDERS];


        ///fonts and colors
        moFont      m_Font; ///style, weight, size, ...

        ///Colors
        moLiveColor     m_FontColor; ///
        moLiveColor     m_BorderColor[BORDERS]; ///for each border




};

moDeclareDynamicArray( moLiveFrameZone*, moLiveFrameZones );


class moLiveFrame : public moLiveParagraph {

    public:

        moLiveFrame();
        virtual ~moLiveFrame();
        moLiveFrame &operator = (const moLiveFrame &src);

    protected:

        moLiveFrameZone   m_Zone;

};

moDeclareDynamicArray( moLiveFrame*, moLiveFrames );


class moLiveLayout : public moAbstract {

  public:

        moLiveLayout();
        virtual ~moLiveLayout();
        moLiveLayout &operator = (const moLiveLayout &src);

  protected:

        moLiveFrameZones    m_FrameZones;

};


moDeclareDynamicArray( moLiveLayout*, moLiveLayouts );


class moLiveComicPage : public moAbstract {

    public:

        moLiveComicPage();
        virtual ~moLiveComicPage();
        moLiveComicPage &operator = (const moLiveComicPage &src);

        void NextPosition(); ///va hacia el proximo frame
        void PrevPosition(); ///va hacia el frame anterior

        void LeftPosition(); ///va hacia el frame mas surdo
        void UpPosition(); ///va hacia el frame mas arriba
        void DownPosition(); ///va hacia el frame mas abajo
        void RightPosition(); ///va hacia el frame mas diestro

        moLiveFrame&       GetActualFrame();

    protected:

        int               m_FramePosition;
        moLiveFrames      m_Frames;

};

moDeclareDynamicArray( moLiveComicPage*, moLiveComicBook );

class moLivePage : public moAbstract {

    public:

        moLivePage();
        virtual ~moLivePage();
        moLivePage &operator = (const moLivePage &src);

    protected:

        moLiveParagraphs m_Paragraphs;


};


moDeclareDynamicArray( moLivePage*, moLiveChapter );

moDeclareDynamicArray( moLiveChapter*, moLiveBook );



class moEffectLiveWriting : public moEffect
{
    public:


        moEffectLiveWriting();
        virtual ~moEffectLiveWriting();

        MOboolean Init();
        void Draw(moTempo*, moEffectState* parentstate = NULL);
        MOboolean Finish();

        void Interaction( moIODeviceManager * );
        moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

    protected:

        moLivePhrase        m_ActualPhrase;

        moLiveParagraph     m_ActualParagraph;

        moLiveFrame         m_ActualFrame;

        moLiveComicPage     m_ActualComicPage;

        moLiveComicBook     m_LiveComicBook;

};

class moEffectLiveWritingFactory : public moEffectFactory
{
public:
    moEffectLiveWritingFactory() {}
    virtual ~moEffectLiveWritingFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
