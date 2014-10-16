/*******************************************************************************

                              moNetTUIOIn.cpp

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

*******************************************************************************/

#include "moNetTUIOIn.h"
#ifndef MO_WIN32
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif
//========================
//  Factory
//========================

moNetTUIOInFactory *m_NetTUIOInFactory = NULL;

MO_PLG_API moIODeviceFactory* CreateIODeviceFactory(){
	if (m_NetTUIOInFactory==NULL)
		m_NetTUIOInFactory = new moNetTUIOInFactory();
	return (moIODeviceFactory*) m_NetTUIOInFactory;
}

MO_PLG_API void DestroyIODeviceFactory(){
	delete m_NetTUIOInFactory;
	m_NetTUIOInFactory = NULL;
}

moIODevice*  moNetTUIOInFactory::Create() {
	return new moNetTUIOIn();
}

void moNetTUIOInFactory::Destroy(moIODevice* fx) {
	delete fx;
}


void moNetTUIOListener::addTuioObject(TuioObject *tobj) {
    cout << "addTuioObject cursor" << endl;

    if (verbose)
        MODebug2->Push(moText("Add Object"));
/*	if (verbose)
		std::cout << "add obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") "<< tobj->getX() << " " << tobj->getY() << " " << tobj->getAngle() << std::endl;
*/

}

void moNetTUIOListener::updateTuioObject(TuioObject *tobj) {
  if (verbose)
    MODebug2->Push(moText("Update Object"));
    /*
	if (verbose)
		std::cout << "set obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") "<< tobj->getX() << " " << tobj->getY() << " " << tobj->getAngle()
		<< " " << tobj->getMotionSpeed() << " " << tobj->getRotationSpeed() << " " << tobj->getMotionAccel() << " " << tobj->getRotationAccel() << std::endl;
*/
}

void moNetTUIOListener::removeTuioObject(TuioObject *tobj) {
    if (verbose)
        MODebug2->Push(moText("Remove Object"));

	/*if (verbose)
		std::cout << "del obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ")" << std::endl;
		*/
}

void moNetTUIOListener::addTuioCursor(TuioCursor *tcur) {

    //here this is a mouse down event!!!
    //if (tcur->getCursorID()==0) {
		//m_CachedEvents.Add( MO_IODEVICE_MOUSE, SDL_MOUSEMOTION, tcur->getX()*1280, tcur->getY()*800,  tcur->getX()*1280, tcur->getY()*800 );
        //m_CachedEvents.Add( MO_IODEVICE_MOUSE, SDL_MOUSEMOTION, tcur->getX()*1280, tcur->getY()*800, tcur->getX()*1280, tcur->getY()*800  );
		//m_CachedEvents.Add( MO_IODEVICE_MOUSE, SDL_MOUSEBUTTONDOWN, SDL_BUTTON_LEFT, tcur->getX()*1280, tcur->getY()*800 );
        m_CachedEvents.Add( MO_IODEVICE_TOUCH, MO_TOUCH_START, tcur->getCursorID(), tcur->getSessionID(), tcur->getX()*1280, tcur->getY()*800 );
    //}



    if (verbose)
    MODebug2->Message(
        moText("Add Cursor: id: ")
        + IntToStr(tcur->getCursorID())
        + moText(" x: ")
        + FloatToStr(tcur->getX())
        + moText(" y: ")
        + FloatToStr(tcur->getY())
    );
/*	if (verbose)
		std::cout << "add cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() << std::endl;
*/
}

void moNetTUIOListener::updateTuioCursor(TuioCursor *tcur) {
    //here this is a mouse motion event
    //if (tcur->getCursorID()==0) {
        //m_CachedEvents.Add( MO_IODEVICE_MOUSE, SDL_MOUSEMOTION, tcur->getX()*1280, tcur->getY()*800, tcur->getX()*1280, tcur->getY()*800  );
        m_CachedEvents.Add( MO_IODEVICE_TOUCH, MO_TOUCH_MOVE, tcur->getCursorID(), tcur->getSessionID(), tcur->getX()*1280, tcur->getY()*800 );
    //}
    //m_CachedEvents.Add( MO_IODEVICE_MOUSE, SDL_MOUSEMOTION, x-m_MouseX, y-m_MouseY, x, y );
    /*if (verbose)
      MODebug2->Message(
          moText("Update Cursor: id: ")
          + IntToStr(tcur->getCursorID())
          + moText(" x: ")
          + FloatToStr(tcur->getX())
          + moText(" y: ")
          + FloatToStr(tcur->getY())
      );
      */
/*
	if (verbose)
		std::cout << "set cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY()
					<< " " << tcur->getMotionSpeed() << " " << tcur->getMotionAccel() << " " << std::endl;
*/

}

void moNetTUIOListener::removeTuioCursor(TuioCursor *tcur) {
    //here this is a mouse up event
    //if (tcur->getCursorID()==0) {
        //m_CachedEvents.Add( MO_IODEVICE_MOUSE, SDL_MOUSEBUTTONUP, SDL_BUTTON_LEFT, tcur->getX(), tcur->getY() );
        m_CachedEvents.Add( MO_IODEVICE_TOUCH, MO_TOUCH_END, tcur->getCursorID(), tcur->getSessionID(), tcur->getX()*1280, tcur->getY()*800 );
    //}

    if (verbose)
    MODebug2->Message(
        moText("Remove Cursor id: ")
        + IntToStr(tcur->getCursorID())
        + moText(" session id: ")
        + IntToStr(tcur->getSessionID())
    );
/*
	if (verbose)
		std::cout << "del cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ")" << std::endl;
    */
}

void moNetTUIOListener::refresh(TuioTime frameTime) {
	//drawObjects();
}


void moNetTUIOListener::drawObjects() {

	char id[3];

	// draw the cursors
	std::list<TuioCursor*> cursorList = tuioClient->getTuioCursors();
	tuioClient->lockCursorList();

	//MODebug2->Push(moText("Draw Cursors"));

	for (std::list<TuioCursor*>::iterator iter = cursorList.begin(); iter!=cursorList.end(); iter++) {

		TuioCursor *tuioCursor = (*iter);

		std::list<TuioPoint> path = tuioCursor->getPath();

		if (path.size()>0) {

            TuioPoint last_point = path.front();


            for (std::list<TuioPoint>::iterator point = path.begin(); point!=path.end(); point++) {
                //glVertex3f(last_point.getX()*width, last_point.getY()*height, 0.0f);
                //glVertex3f(point->getX()*width, point->getY()*height, 0.0f);
                last_point.update(point->getX(),point->getY());

                MODebug2->Push(moText("cursor: x:") + FloatToStr(point->getX()) );

            }

		}
	}
	tuioClient->unlockCursorList();

	// draw the objects

	//MODebug2->Push(moText("Draw ObjectsS"));

	std::list<TuioObject*> objectList = tuioClient->getTuioObjects();
	tuioClient->lockObjectList();
	for (std::list<TuioObject*>::iterator iter = objectList.begin(); iter!=objectList.end(); iter++) {
		TuioObject* tuioObject = (*iter);
		int pos_size = height/20.0f;
		int neg_size = -1*pos_size;
		float xpos  = tuioObject->getX()*width;
		float ypos  = tuioObject->getY()*height;
		float angle = tuioObject->getAngle()/M_PI*180;

        //MODebug2->Push(moText("angle:") + FloatToStr(angle) );
		/*


		glColor3f(0.0, 0.0, 0.0);
		glPushMatrix();
		glTranslatef(xpos, ypos, 0.0);
		glRotatef(angle, 0.0, 0.0, 1.0);
		glBegin(GL_QUADS);
			glVertex2f(neg_size, neg_size);
			glVertex2f(neg_size, pos_size);
			glVertex2f(pos_size, pos_size);
			glVertex2f(pos_size, neg_size);
		glEnd();
		glPopMatrix();

		glColor3f(1.0, 1.0, 1.0);
		glRasterPos2f(xpos,ypos+5);
		sprintf(id,"%d",tuioObject->getSymbolID());
		drawString(id);
		*/
	}



	tuioClient->unlockObjectList();

}

void moNetTUIOListener::updateOutlets(  ) {

  if (!m_pOutlets || !tuioClient) return;

///RESET outlets...to default value

    if (m_pOutletCursor1TouchOn && m_pOutletCursor1TouchOn->GetData()->Int()!=0) {
      m_pOutletCursor1TouchOn->GetData()->SetInt( 0 );
      m_pOutletCursor1TouchOn->Update( true );
    }
    if (m_pOutletCursor1TouchOff && m_pOutletCursor1TouchOff->GetData()->Int()!=1) {
      m_pOutletCursor1TouchOff->GetData()->SetInt( 1 );
      m_pOutletCursor1TouchOff->Update( true );
    }
    if (m_pOutletCursor2TouchOn && m_pOutletCursor2TouchOn->GetData()->Int()!=0) {
      m_pOutletCursor2TouchOn->GetData()->SetInt( 0 );
      m_pOutletCursor2TouchOn->Update( true );
    }
    if (m_pOutletCursor2TouchOff && m_pOutletCursor2TouchOff->GetData()->Int()!=1) {
      m_pOutletCursor2TouchOff->GetData()->SetInt( 1 );
      m_pOutletCursor2TouchOff->Update( true );
    }
    if (m_pOutletCursor3TouchOn && m_pOutletCursor3TouchOn->GetData()->Int()!=0) {
      m_pOutletCursor3TouchOn->GetData()->SetInt( 0 );
      m_pOutletCursor3TouchOn->Update( true );
    }
    if (m_pOutletCursor3TouchOff && m_pOutletCursor3TouchOff->GetData()->Int()!=1) {
      m_pOutletCursor3TouchOff->GetData()->SetInt( 1 );
      m_pOutletCursor3TouchOff->Update( true );
    }
    if (m_pOutletCursor4TouchOn && m_pOutletCursor4TouchOn->GetData()->Int()!=0) {
      m_pOutletCursor4TouchOn->GetData()->SetInt( 0 );
      m_pOutletCursor4TouchOn->Update( true );
    }
    if (m_pOutletCursor4TouchOff && m_pOutletCursor4TouchOff->GetData()->Int()!=1) {
      m_pOutletCursor4TouchOff->GetData()->SetInt( 1 );
      m_pOutletCursor4TouchOff->Update( true );
    }


  char id[3];

  // draw the cursors
  std::list<TuioCursor*> cursorList = tuioClient->getTuioCursors();
  tuioClient->lockCursorList();

  //MODebug2->Push(moText("Draw Cursors"));
  int ncursors = 0;
  for (std::list<TuioCursor*>::iterator iter = cursorList.begin(); iter!=cursorList.end(); iter++) {

    TuioCursor *tuioCursor = (*iter);

    std::list<TuioPoint> path = tuioCursor->getPath();



    ncursors++;
    if (tuioCursor) {

      switch(ncursors) {

          case 1:
			if (m_pOutletCursor1TouchStart) {
            m_pOutletCursor1TouchStart->GetData()->SetInt( tuioCursor->getTuioState()==TUIO_ADDED );
                m_pOutletCursor1TouchStart->Update( true );
            }
			if (m_pOutletCursor1TouchOn) {

          int ison = (int)  (tuioCursor->getTuioState()==TUIO_ADDED)
                        || (tuioCursor->getTuioState()==TUIO_ACCELERATING)
                        || (tuioCursor->getTuioState()==TUIO_DECELERATING)
                        || (tuioCursor->getTuioState()==TUIO_ROTATING)
                        || (tuioCursor->getTuioState()==TUIO_STOPPED);
          //MODebug2->Message( moText("CURSOR1TUIOON On:") + IntToStr(ison) );
            m_pOutletCursor1TouchOn->GetData()->SetInt(ison);
                m_pOutletCursor1TouchOn->Update( true );
            }
			if (m_pOutletCursor1TouchEnd) {
            m_pOutletCursor1TouchEnd->GetData()->SetInt( tuioCursor->getTuioState()==TUIO_REMOVED );
                m_pOutletCursor1TouchEnd->Update( true );
            }
			if (m_pOutletCursor1TouchOff) {
            m_pOutletCursor1TouchOff->GetData()->SetInt( (tuioCursor->getTuioState()==TUIO_REMOVED));
            m_pOutletCursor1TouchOff->Update( true );
        }
			if (m_pOutletCursor1TouchMove) {
          m_pOutletCursor1TouchMove->GetData()->SetInt( tuioCursor->getTuioState()==TUIO_ACCELERATING
                                                       || tuioCursor->getTuioState()==TUIO_DECELERATING );
                m_pOutletCursor1TouchMove->Update( true );
            }
            if (m_pOutletCursor1X) {
                m_pOutletCursor1X->GetData()->SetFloat( tuioCursor->getX() );
                m_pOutletCursor1X->Update( true );
            }
            if (m_pOutletCursor1Y) {
                m_pOutletCursor1Y->GetData()->SetFloat( tuioCursor->getY() );
                m_pOutletCursor1Y->Update( true );
            }
            break;

          case 2:

            if (m_pOutletCursor2TouchStart) {
                m_pOutletCursor2TouchStart->GetData()->SetInt( tuioCursor->getTuioState()==TUIO_ADDED );
                m_pOutletCursor2TouchStart->Update( true );
            }
            if (m_pOutletCursor2TouchOn) {
                m_pOutletCursor2TouchOn->GetData()->SetInt( (tuioCursor->getTuioState()==TUIO_ADDED)
                                                        || (tuioCursor->getTuioState()==TUIO_ACCELERATING)
                                                        || (tuioCursor->getTuioState()==TUIO_DECELERATING)
                                                        || (tuioCursor->getTuioState()==TUIO_ROTATING)
                                                        || (tuioCursor->getTuioState()==TUIO_STOPPED)
                                                           );
                m_pOutletCursor2TouchOn->Update( true );
            }
            if (m_pOutletCursor2TouchEnd) {
                m_pOutletCursor2TouchEnd->GetData()->SetInt( tuioCursor->getTuioState()==TUIO_REMOVED );
                m_pOutletCursor2TouchEnd->Update( true );
            }
            if (m_pOutletCursor2TouchOff) {
                m_pOutletCursor2TouchOff->GetData()->SetInt( (tuioCursor->getTuioState()==TUIO_REMOVED));
                m_pOutletCursor2TouchOff->Update( true );
            }
            if (m_pOutletCursor2TouchMove) {
                m_pOutletCursor2TouchMove->GetData()->SetInt( tuioCursor->getTuioState()==TUIO_ACCELERATING
                                                             || tuioCursor->getTuioState()==TUIO_DECELERATING );
                m_pOutletCursor2TouchMove->Update( true );
            }

            if (m_pOutletCursor2X) {
                m_pOutletCursor2X->GetData()->SetFloat( tuioCursor->getX() );
                m_pOutletCursor2X->Update( true );
            }
            if (m_pOutletCursor2Y) {
                m_pOutletCursor2Y->GetData()->SetFloat( tuioCursor->getY() );
                m_pOutletCursor2Y->Update( true );
            }
            break;

        case 3:

            if (m_pOutletCursor3TouchStart) {
                m_pOutletCursor3TouchStart->GetData()->SetInt( tuioCursor->getTuioState()==TUIO_ADDED );
                m_pOutletCursor3TouchStart->Update( true );
            }
            if (m_pOutletCursor3TouchOn) {
                m_pOutletCursor3TouchOn->GetData()->SetInt( (tuioCursor->getTuioState()==TUIO_ADDED)
                                                        || (tuioCursor->getTuioState()==TUIO_ACCELERATING)
                                                        || (tuioCursor->getTuioState()==TUIO_DECELERATING)
                                                        || (tuioCursor->getTuioState()==TUIO_ROTATING)
                                                        || (tuioCursor->getTuioState()==TUIO_STOPPED)
                                                           );
                m_pOutletCursor3TouchOn->Update( true );
            }
            if (m_pOutletCursor3TouchEnd) {
                m_pOutletCursor3TouchEnd->GetData()->SetInt( tuioCursor->getTuioState()==TUIO_REMOVED );
                m_pOutletCursor3TouchEnd->Update( true );
            }
            if (m_pOutletCursor3TouchOff) {
                m_pOutletCursor3TouchOff->GetData()->SetInt( (tuioCursor->getTuioState()==TUIO_REMOVED));
                m_pOutletCursor3TouchOff->Update( true );
            }
            if (m_pOutletCursor3TouchMove) {
                m_pOutletCursor3TouchMove->GetData()->SetInt( tuioCursor->getTuioState()==TUIO_ACCELERATING
                                                             || tuioCursor->getTuioState()==TUIO_DECELERATING );
                m_pOutletCursor3TouchMove->Update( true );
            }

            if (m_pOutletCursor3X) {
                m_pOutletCursor3X->GetData()->SetFloat( tuioCursor->getX() );
                m_pOutletCursor3X->Update( true );
            }
            if (m_pOutletCursor3Y) {
                m_pOutletCursor3Y->GetData()->SetFloat( tuioCursor->getY() );
                m_pOutletCursor3Y->Update( true );
            }
            break;

        case 4:

            if (m_pOutletCursor4TouchStart) {
                m_pOutletCursor4TouchStart->GetData()->SetInt( tuioCursor->getTuioState()==TUIO_ADDED );
                m_pOutletCursor4TouchStart->Update( true );
            }
            if (m_pOutletCursor4TouchOn) {
                m_pOutletCursor4TouchOn->GetData()->SetInt( (tuioCursor->getTuioState()==TUIO_ADDED)
                                                        || (tuioCursor->getTuioState()==TUIO_ACCELERATING)
                                                        || (tuioCursor->getTuioState()==TUIO_DECELERATING)
                                                        || (tuioCursor->getTuioState()==TUIO_ROTATING)
                                                        || (tuioCursor->getTuioState()==TUIO_STOPPED)
                                                           );
                m_pOutletCursor4TouchOn->Update( true );
            }
            if (m_pOutletCursor4TouchEnd) {
                m_pOutletCursor4TouchEnd->GetData()->SetInt( tuioCursor->getTuioState()==TUIO_REMOVED );
                m_pOutletCursor4TouchEnd->Update( true );
            }
            if (m_pOutletCursor4TouchOff) {
                m_pOutletCursor4TouchOff->GetData()->SetInt( (tuioCursor->getTuioState()==TUIO_REMOVED));
                m_pOutletCursor4TouchOff->Update( true );
            }
            if (m_pOutletCursor4TouchMove) {
                m_pOutletCursor4TouchMove->GetData()->SetInt( tuioCursor->getTuioState()==TUIO_ACCELERATING
                                                             || tuioCursor->getTuioState()==TUIO_DECELERATING );
                m_pOutletCursor4TouchMove->Update( true );
            }

            if (m_pOutletCursor4X) {
                m_pOutletCursor4X->GetData()->SetFloat( tuioCursor->getX() );
                m_pOutletCursor4X->Update( true );
            }
            if (m_pOutletCursor4Y) {
                m_pOutletCursor4Y->GetData()->SetFloat( tuioCursor->getY() );
                m_pOutletCursor4Y->Update( true );
            }
            break;

      }

    }

    if (path.size()>0) {

            TuioPoint last_point = path.front();

            for (std::list<TuioPoint>::iterator point = path.begin(); point!=path.end(); point++) {
                //glVertex3f(last_point.getX()*width, last_point.getY()*height, 0.0f);
                //glVertex3f(point->getX()*width, point->getY()*height, 0.0f);
                last_point.update(point->getX(),point->getY());
                //MODebug2->Push(moText("cursor: x:") + FloatToStr(point->getX()) );

            }
    }
  }
  tuioClient->unlockCursorList();

}

  void moNetTUIOListener::updateEvents( moEventList* pEvents ) {
        moEvent* tmp;
        moEvent* actual;
        actual = m_CachedEvents.First;

        while(actual!=NULL) {
          //cout << "PollEvents: Adding new event " << actual << endl;
          if (pEvents) {
            //cout << "PollEvents: GetEvents(): First: " << GetEvents()->First << " Last:" << GetEvents()->Last << endl;
            //pEvents->Add( (moEvent*)actual );
            pEvents->Add( actual->deviceid,
                          actual->devicecode,
                          actual->reservedvalue0,
                          actual->reservedvalue1,
                          actual->reservedvalue2,
                          actual->reservedvalue3,
                          actual->pointer );
            //cout << "PollEvents: GetEvents(): First: " << GetEvents()->First << " Last:" << GetEvents()->Last << endl;
            //cout << "PollEvents: Added OK!!!" << endl;
          }

          tmp = actual;
          actual = tmp->next;
        }
        m_CachedEvents.Finish();
  }

/*
void moNetTUIOListener::drawString(char *str) {
	if (str && strlen(str)) {
		while (*str) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *str);
			str++;
		}
	}
}
*/

moNetTUIOListener::moNetTUIOListener( moOutlets* p_pOutlets, int port ) : tuioClient(NULL),verbose(true),m_pOutlets(NULL) {

  m_pOutletCursors = NULL;

  m_pTrackerNetTUIOIn = NULL;

  m_pOutletCursor1 = NULL;
  m_pOutletCursor1X = NULL;
  m_pOutletCursor1Y = NULL;

  m_pOutletCursor2 = NULL;
  m_pOutletCursor2X = NULL;
  m_pOutletCursor2Y = NULL;

  m_pOutletCursor3 = NULL;
  m_pOutletCursor3X = NULL;
  m_pOutletCursor3Y = NULL;

  m_pOutletCursor4 = NULL;
  m_pOutletCursor4X = NULL;
  m_pOutletCursor4Y = NULL;

  m_pOutletCursor1TouchStart = NULL;
  m_pOutletCursor1TouchOn = NULL;
  m_pOutletCursor1TouchEnd = NULL;
  m_pOutletCursor1TouchOff = NULL;
  m_pOutletCursor1TouchMove = NULL;

  m_pOutletCursor2TouchStart = NULL;
  m_pOutletCursor2TouchOn = NULL;
  m_pOutletCursor2TouchEnd = NULL;
  m_pOutletCursor2TouchOff = NULL;
  m_pOutletCursor2TouchMove = NULL;

  m_pOutletCursor3TouchStart = NULL;
  m_pOutletCursor3TouchOn = NULL;
  m_pOutletCursor3TouchEnd = NULL;
  m_pOutletCursor3TouchOff = NULL;
  m_pOutletCursor3TouchMove = NULL;

  m_pOutletCursor4TouchStart = NULL;
  m_pOutletCursor4TouchOn = NULL;
  m_pOutletCursor4TouchEnd = NULL;
  m_pOutletCursor4TouchOff = NULL;
  m_pOutletCursor4TouchMove = NULL;

  Init( p_pOutlets, port );
}

bool
moNetTUIOListener::Init( moOutlets* p_pOutlets, int port ) {

  m_pOutlets = p_pOutlets;

  moOutlet* pOutlet = NULL;

  for( int i=0; i<m_pOutlets->Count(); i++) {

    pOutlet = m_pOutlets->Get(i);

    if (pOutlet) {

        if (pOutlet->GetConnectorLabelName() == moText("TRACKERNETTUIOIN")) {
            m_pTrackerNetTUIOIn = pOutlet;
            //MODebug2->Message( moText("CURSOR1TUIOSTART outlet ready.") );
        }
//=====================================
        if (pOutlet->GetConnectorLabelName() == moText("CURSORS")) {
            m_pOutletCursors = pOutlet;
            MODebug2->Push( moText("CURSORS outlet ready.") );
        }
//=====================================
        if (pOutlet->GetConnectorLabelName() == moText("CURSOR1")) {
            m_pOutletCursor1 = pOutlet;
            MODebug2->Push( moText("CURSOR1 outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR1TOUCHSTART")) {
            m_pOutletCursor1TouchStart = pOutlet;
            //MODebug2->Message( moText("CURSOR1TUIOSTART outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR1TOUCHON")) {
            m_pOutletCursor1TouchOn = pOutlet;
            //MODebug2->Message( moText("CURSOR1TUIOON outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR1TOUCHEND")) {
            m_pOutletCursor1TouchEnd = pOutlet;
            //MODebug2->Message( moText("CURSOR1TOUCHEND outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR1TOUCHOFF")) {
            m_pOutletCursor1TouchOff = pOutlet;
            //MODebug2->Message( moText("CURSOR1TUIOOFF outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR1TOUCHMOVE")) {
            m_pOutletCursor1TouchMove = pOutlet;
            MODebug2->Push( moText("CURSOR1TOUCHMOVE outlet ready.") );
        }


        if (pOutlet->GetConnectorLabelName() == moText("CURSOR1X")) {
            m_pOutletCursor1X = pOutlet;
            MODebug2->Push( moText("CURSOR1X outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR1Y")) {
            m_pOutletCursor1Y = pOutlet;
            MODebug2->Push( moText("CURSOR1Y outlet ready.") );
        }
//=====================================
        if (pOutlet->GetConnectorLabelName() == moText("CURSOR2")) {
            m_pOutletCursor2 = pOutlet;
            MODebug2->Push( moText("CURSOR2 outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR2TOUCHSTART")) {
            m_pOutletCursor2TouchStart = pOutlet;
            MODebug2->Push( moText("CURSOR2TUIOSTART outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR2TOUCHON")) {
            m_pOutletCursor2TouchOn = pOutlet;
            //MODebug2->Message( moText("CURSOR2TUIOON outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR2TOUCHEND")) {
            m_pOutletCursor2TouchEnd = pOutlet;
            MODebug2->Push( moText("CURSOR2TOUCHEND outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR2TOUCHOFF")) {
            m_pOutletCursor2TouchOff = pOutlet;
            //MODebug2->Message( moText("CURSOR2TUIOOFF outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR2TOUCHMOVE")) {
            m_pOutletCursor2TouchMove = pOutlet;
            MODebug2->Push( moText("CURSOR2TOUCHMOVE outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR2X")) {
            m_pOutletCursor2X = pOutlet;
            MODebug2->Push( moText("CURSOR2X outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR2Y")) {
            m_pOutletCursor2Y = pOutlet;
            MODebug2->Push( moText("CURSOR2Y outlet ready.") );
        }
//=====================================
        if (pOutlet->GetConnectorLabelName() == moText("CURSOR3")) {
            m_pOutletCursor3 = pOutlet;
            MODebug2->Push( moText("CURSOR3 outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR3TOUCHSTART")) {
            m_pOutletCursor3TouchStart = pOutlet;
            //MODebug2->Message( moText("CURSOR1TUIOSTART outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR3TOUCHON")) {
            m_pOutletCursor3TouchOn = pOutlet;
            //MODebug2->Message( moText("CURSOR1TUIOON outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR3TOUCHEND")) {
            m_pOutletCursor3TouchEnd = pOutlet;
            //MODebug2->Message( moText("CURSOR1TOUCHEND outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR3TOUCHOFF")) {
            m_pOutletCursor3TouchOff = pOutlet;
            //MODebug2->Message( moText("CURSOR1TUIOOFF outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR3TOUCHMOVE")) {
            m_pOutletCursor3TouchMove = pOutlet;
            MODebug2->Push( moText("CURSOR3TOUCHMOVE outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR3X")) {
            m_pOutletCursor3X = pOutlet;
            MODebug2->Push( moText("CURSOR3X outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR3Y")) {
            m_pOutletCursor3Y = pOutlet;
            MODebug2->Push( moText("CURSOR3Y outlet ready.") );
        }


//=====================================
        if (pOutlet->GetConnectorLabelName() == moText("CURSOR4")) {
            m_pOutletCursor4 = pOutlet;
            MODebug2->Push( moText("CURSOR4 outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR4TOUCHSTART")) {
            m_pOutletCursor4TouchStart = pOutlet;
            //MODebug2->Message( moText("CURSOR1TUIOSTART outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR4TOUCHON")) {
            m_pOutletCursor4TouchOn = pOutlet;
            //MODebug2->Message( moText("CURSOR1TUIOON outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR4TOUCHEND")) {
            m_pOutletCursor4TouchEnd = pOutlet;
            //MODebug2->Message( moText("CURSOR1TOUCHEND outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR4TOUCHOFF")) {
            m_pOutletCursor4TouchOff = pOutlet;
            //MODebug2->Message( moText("CURSOR1TUIOOFF outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR4TOUCHMOVE")) {
            m_pOutletCursor4TouchMove = pOutlet;
            MODebug2->Push( moText("CURSOR4TOUCHMOVE outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR4X")) {
            m_pOutletCursor4X = pOutlet;
            MODebug2->Push( moText("CURSOR4X outlet ready.") );
        }

        if (pOutlet->GetConnectorLabelName() == moText("CURSOR4Y")) {
            m_pOutletCursor4Y = pOutlet;
            MODebug2->Push( moText("CURSOR4Y outlet ready.") );
        }

    }
  }


  if (tuioClient) {
		tuioClient->disconnect();
		delete tuioClient;
		tuioClient = NULL;
  }

	tuioClient = new TuioClient(port);
	tuioClient->addTuioListener(this);
	tuioClient->connect();

	if (!tuioClient->isConnected()) {
	    MODebug2->Error(moText("TUIO Listener Error : unable to connect to port: ") + IntToStr(port));
	} else {
      MODebug2->Message(moText("TUIO Listener Connected to port: ") + IntToStr(port));
  }


  return true;
}

void
moNetTUIOListener::SetVerbose ( bool p_verbose ) {
    verbose = p_verbose;
}


// moNetTUIOIn class **************************************************

moNetTUIOIn::moNetTUIOIn()
{
    SetName("nettuioin");
    m_pListener = NULL;
    m_pTrackerSystemData = NULL;
}

moNetTUIOIn::~moNetTUIOIn()
{
    Finish();
}

MOboolean moNetTUIOIn::Init()
{
    int dev;
    int port;
    MOuint i, n, n_dev, n_hosts;
    moText conf, dev_name;

    //==========================
    // CORRESPONDE A UN PREINIT
    //==========================

    if (moMoldeoObject::Init()) {
      moMoldeoObject::CreateConnectors();
    } else return false;

    //==========================
    // INIT
    //==========================

    moDefineParamIndex( NETTUIOIN_INLET, moText("inlet") );
    moDefineParamIndex( NETTUIOIN_OUTLET, moText("outlet") );
    moDefineParamIndex( NETTUIOIN_PORT , moText("port") );
    moDefineParamIndex( NETTUIOIN_HOST , moText("host") );
    moDefineParamIndex( NETTUIOIN_VERBOSE , moText("verbose") );

    port = m_Config.Int( moR(NETTUIOIN_PORT) );

    MODebug2->Message(moText("Initializing listener on port: ") + IntToStr(port));

    m_pListener = new moNetTUIOListener( GetOutlets(), port );

    return true;
}


moConfigDefinition *
moNetTUIOIn::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moMoldeoObject::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("port"), MO_PARAM_NUMERIC, NETTUIOIN_PORT, moValue( moText("3333"), moText("NUM")).Ref() );
	p_configdefinition->Add( moText("host"), MO_PARAM_TEXT, NETTUIOIN_HOST );
	p_configdefinition->Add( moText("verbose"), MO_PARAM_NUMERIC, NETTUIOIN_VERBOSE, moValue( moText("1"), moText("NUM")).Ref() );
	return p_configdefinition;
}

MOswitch moNetTUIOIn::SetStatus(MOdevcode codisp, MOswitch state)
{
    return true;
}

MOswitch moNetTUIOIn::GetStatus(MOdevcode codisp)
{
    return(-1);
}

MOint moNetTUIOIn::GetValue(MOdevcode codisp)
{
    return(-1);
}

MOdevcode moNetTUIOIn::GetCode(moText strcod)
{
    return(-1);
}

void moNetTUIOIn::Update(moEventList *Eventos)
{

    MOuint i;

    //clean every outlet!!! important
    for(i=0; i<GetOutlets()->Count();i++) {
        moOutlet* pOutlet = GetOutlets()->Get(i);
        if (pOutlet) {
            pOutlet->GetMessages().Empty();
            pOutlet->Update(false);
        }
    }


    if (!m_pTrackerSystemData) {
        MODebug2->Push("moNetTUIOIn::UpdateParameters > creating moTrackerSystemData() Object.");
        m_pTrackerSystemData = new moTrackerSystemData();
        if (m_pTrackerSystemData) {
            MODebug2->Message( "moNetTUIOIn::UpdateParameters > moTrackerSystemData() Object OK.");
        }
    }

    if (m_pTrackerSystemData) {

        for(int i=0; i<m_pTrackerSystemData->GetFeatures().Count(); i++ ) {
            if (m_pTrackerSystemData->GetFeatures().GetRef(i)!=NULL)
            delete m_pTrackerSystemData->GetFeatures().GetRef(i);
        }

        ///RESET DATA !!!!
        m_pTrackerSystemData->GetFeatures().Empty();
        m_pTrackerSystemData->ResetMatrix();

        ///GET NEW DATA!!!!
        moTrackerFeature* TF = NULL;
        MOubyte* pBuf = NULL;

        int validfeatures = 0;

        float sumX = 0.0f,sumY = 0.0f;
        float sumN = 0.0f;
        float varX = 0.0f, varY = 0.0f;
        float minX = 1.0f, minY = 1.0;
        float maxX = 0.0f, maxY = 0.0;

        float vel=0.0,acc=0.0,tor=0.0;
        float velAverage = 0.0, accAverage =0.0, torAverage=0.0;
        moVector2f velAverage_v(0,0);



      if (m_pListener && m_pListener->tuioClient ) {

        std::list<TuioCursor*> cursorList = m_pListener->tuioClient->getTuioCursors();
         m_pListener->tuioClient->lockCursorList();

        //MODebug2->Push(moText("Draw Cursors"));

        for (std::list<TuioCursor*>::iterator iter = cursorList.begin(); iter!=cursorList.end(); iter++) {

          TuioCursor *tuioCursor = (*iter);

          std::list<TuioPoint> path = tuioCursor->getPath();

          if (path.size()>0) {

                  TuioPoint last_point = path.front();


                  for (std::list<TuioPoint>::iterator point = path.begin(); point!=path.end(); point++) {
                      //glVertex3f(last_point.getX()*width, last_point.getY()*height, 0.0f);
                      //glVertex3f(point->getX()*width, point->getY()*height, 0.0f);
                      last_point.update(point->getX(),point->getY());

                      //MODebug2->Push(moText("cursor: x:") + FloatToStr(point->getX()) + moText("cursor: y:") + FloatToStr(point->getY()) );

                      {///NEW TRACKER FEATURE
                      TF = new moTrackerFeature();
                        if (TF) {
                            TF->x = point->getX();
                            TF->y =  point->getY();
                            TF->val = 0;
                            TF->valid = 1;
                            TF->tr_x = TF->x;
                            TF->tr_y = TF->y;

                            ///CALCULATE AVERAGE FOR BARYCENTER AND VARIANCE
                            sumX+= TF->x;
                            sumY+= TF->y;

                            sumN+= 1.0f;

                            if (sumN==1.0f) {
                                /*
                                MODebug2->Push( moText("moOpenCV::UpdateParameters > TF > TF->x:")
                                            + FloatToStr(TF->x)
                                            + moText(" TF->y:")
                                            + FloatToStr(TF->y)
                                           );
                                */
                            }

                            ///maximos
                            if (TF->x>maxX) maxX = TF->x;
                            if (TF->y>maxY) maxY = TF->y;

                            ///minimos
                            if (TF->x<minX) minX = TF->x;
                            if (TF->y<minY) minY = TF->y;

                            ///esta es simplemente una matriz que cuenta la cantidad de....
                            m_pTrackerSystemData->SetPositionMatrix( TF->x, TF->y, 1 );
                            ///genera la matrix de referencia rapida por zonas
                            ///m_pTrackerSystemData->SetPositionMatrix( TF );

                            ///CALCULATE VELOCITY AND ACCELERATION
                            TF->ap_x = TF->a_x;
                            TF->ap_y = TF->a_x;
                            TF->vp_x = TF->v_x;
                            TF->vp_y = TF->v_x;
                            TF->v_x = TF->x - TF->tr_x;
                            TF->v_y = TF->y - TF->tr_y;
                            TF->a_x = TF->v_x - TF->vp_x;
                            TF->a_y = TF->v_y - TF->vp_y;
                            TF->t_x = TF->a_x - TF->ap_x;
                            TF->t_y = TF->a_y - TF->ap_y;

                            vel = moVector2f( TF->v_x, TF->v_y ).Length();
                            acc = moVector2f( TF->a_x, TF->a_y ).Length();
                            tor = moVector2f( TF->t_x, TF->t_y ).Length();
                            velAverage+= vel;
                            accAverage+= acc;
                            torAverage+= tor;
                            velAverage_v+= moVector2f( fabs(TF->v_x), fabs(TF->v_y) );


                            if ( vel >= 0.001 && vel <=0.05 ) m_pTrackerSystemData->SetMotionMatrix( TF->x, TF->y, 1 );
                            if ( acc >= 0.001 ) m_pTrackerSystemData->SetAccelerationMatrix( TF->x, TF->y, 1 );

                        }

                        m_pTrackerSystemData->GetFeatures().Add(TF);
                      }


                      } ///FIN NEW TRACKER FEATURE



                  }

          }
        }

         m_pListener->tuioClient->unlockCursorList();

        /** BARYCENTER VARIANCE AND OTHERS*/


        moVector2f previous_B = m_pTrackerSystemData->GetBarycenter();
        moVector2f previous_BM = m_pTrackerSystemData->GetBarycenterMotion();

        moVector2f BarPos;
        moVector2f BarMot;
        moVector2f BarAcc;

        m_pTrackerSystemData->SetBarycenter( 0, 0 );
        m_pTrackerSystemData->SetBarycenterMotion( 0, 0);
        m_pTrackerSystemData->SetBarycenterAcceleration( 0, 0 );

        m_pTrackerSystemData->SetMax( 0, 0 );
        m_pTrackerSystemData->SetMin( 0, 0 );

        m_pTrackerSystemData->SetDeltaValidFeatures( m_pTrackerSystemData->GetValidFeatures() - (int)sumN );
        m_pTrackerSystemData->SetValidFeatures( (int)sumN );

        if (sumN>=1.0f) {

            BarPos = moVector2f( sumX/sumN, sumY/sumN);

            BarMot = BarPos - previous_B;
            BarAcc = BarMot - previous_BM;

            m_pTrackerSystemData->SetBarycenter( BarPos.X(), BarPos.Y() );
            m_pTrackerSystemData->SetBarycenterMotion( BarMot.X(), BarMot.Y() );
            m_pTrackerSystemData->SetBarycenterAcceleration( BarAcc.X(), BarAcc.Y() );

            velAverage = velAverage / (float)sumN;
            accAverage = accAverage / (float)sumN;
            torAverage = torAverage / (float)sumN;
            velAverage_v = velAverage_v * 1.0f / (float)sumN;

            m_pTrackerSystemData->SetAbsoluteSpeedAverage( velAverage );
            m_pTrackerSystemData->SetAbsoluteAccelerationAverage( accAverage );
            m_pTrackerSystemData->SetAbsoluteTorqueAverage( torAverage );

            m_pTrackerSystemData->SetMax( maxX, maxY );
            m_pTrackerSystemData->SetMin( minX, minY );

            ///CALCULATE VARIANCE FOR EACH COMPONENT

            moVector2f Bar = m_pTrackerSystemData->GetBarycenter();
            for(int i=0; i<m_pTrackerSystemData->GetFeatures().Count(); i++ ) {
                TF = m_pTrackerSystemData->GetFeatures().GetRef(i);
                if (TF) {
                    if (TF->val>=0) {
                        varX+= moMathf::Sqr( TF->x - Bar.X() );
                        varY+= moMathf::Sqr( TF->y - Bar.Y() );
                    }
                }
            }
            m_pTrackerSystemData->SetVariance( varX/sumN, varY/sumN );
            //m_pTrackerSystemData->SetVariance( velAverage_v.X(), velAverage_v.Y() );

/*
            MODebug2->Push( moText("TrackerKLT: varX: ") + FloatToStr( m_pTrackerSystemData->GetVariance().X())
                           + moText(" varY: ") + FloatToStr(m_pTrackerSystemData->GetVariance().Y()) );
*/


            ///CALCULATE CIRCULAR MATRIX
            for(int i=0; i<m_pTrackerSystemData->GetFeatures().Count(); i++ ) {
                TF = m_pTrackerSystemData->GetFeatures().GetRef(i);
                if (TF) {
                    if (TF->val>=0) {
                        m_pTrackerSystemData->SetPositionMatrixC( TF->x, TF->y, 1 );
                        vel = moVector2f( TF->v_x, TF->v_y ).Length();
                        //acc = moVector2f( TF->a_x, TF->a_y ).Length();
                        if (vel>=0.01) m_pTrackerSystemData->SetMotionMatrixC( TF->x, TF->y, 1 );
                    }
                }
            }
            /*

            MODebug2->Push( moText("moOpenCV::UpdateParameters > ValidFeatures: ")
                                            + FloatToStr(sumN)
                                            + moText(" GetValidFeatures:")
                                            + IntToStr( m_pTrackerSystemData->GetValidFeatures() )
                                           );*/



        }///END BARYCENTERE AND OTHERS

    }


    if ( m_pListener) {
      if (m_pListener->m_pTrackerNetTUIOIn) {
        if (m_pTrackerSystemData) {
          m_pListener->m_pTrackerNetTUIOIn->GetData()->SetPointer( (MOpointer) m_pTrackerSystemData, sizeof(moTrackerSystemData) );
          m_pListener->m_pTrackerNetTUIOIn->Update(); ///to notify Inlets!!
          if (m_pTrackerSystemData->GetFeaturesCount()) {
              //MODebug2->Push("moNetTUIOIn::Update > Sending tuio signals count: " + IntToStr(m_pTrackerSystemData->GetFeaturesCount()) );
          }
        }
      }
    }

    bool res;
    moEvent *tmp;
    moEvent *actual;


    if (m_pListener) {
        //m_pListener->drawObjects();
        m_pListener->SetVerbose( ( 1 == m_Config.Int( moR(NETTUIOIN_VERBOSE) )) );
        m_pListener->updateOutlets();
        m_pListener->updateEvents(Eventos);
    }

    moMoldeoObject::Update(Eventos);

    for(i=0; i<GetOutlets()->Count();i++) {
        moOutlet* pOutlet = GetOutlets()->Get(i);
        if (pOutlet) {
            if (pOutlet->Updated()) {
                /*
                MODebug2->Push(moText("NetTuioIn messages: ")+IntToStr(pOutlet->GetMessages().Count() )
                             +moText(" [1s int]: ")+IntToStr(pOutlet->GetMessages().GetRef(0).Ref(0).Int())
                             +moText(" [2d int]: ")+IntToStr(pOutlet->GetMessages().GetRef(0).Ref(1).Int())
                             +moText(" [3rd int]: ")+IntToStr(pOutlet->GetMessages().GetRef(0).Ref(2).Int())
                             );
                             */
            }
        }
    }

}

MOboolean moNetTUIOIn::Finish()
{
  if (m_pListener) {
    delete m_pListener;
    m_pListener = NULL;
  }
	return true;
}

void moNetTUIOIn::SendEvent(int i)
{

}
