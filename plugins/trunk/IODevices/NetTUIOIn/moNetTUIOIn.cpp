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
#include <SDL/SDL.h>
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
    if (tcur->getCursorID()==0) m_CachedEvents.Add( MO_IODEVICE_MOUSE, SDL_MOUSEBUTTONDOWN, SDL_BUTTON_LEFT, tcur->getX()*1280, tcur->getY()*800 );

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
    if (tcur->getCursorID()==0) m_CachedEvents.Add( MO_IODEVICE_MOUSE, SDL_MOUSEMOTION, tcur->getX()*1280, tcur->getY()*800, tcur->getX()*1280, tcur->getY()*800  );
    //m_CachedEvents.Add( MO_IODEVICE_MOUSE, SDL_MOUSEMOTION, x-m_MouseX, y-m_MouseY, x, y );
    if (verbose)
    MODebug2->Message(
        moText("Update Cursor: id: ")
        + IntToStr(tcur->getCursorID())
        + moText(" x: ")
        + FloatToStr(tcur->getX())
        + moText(" y: ")
        + FloatToStr(tcur->getY())
    );
/*
	if (verbose)
		std::cout << "set cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY()
					<< " " << tcur->getMotionSpeed() << " " << tcur->getMotionAccel() << " " << std::endl;
*/

}

void moNetTUIOListener::removeTuioCursor(TuioCursor *tcur) {
    //here this is a mouse up event
    if (tcur->getCursorID()==0) m_CachedEvents.Add( MO_IODEVICE_MOUSE, SDL_MOUSEBUTTONUP, SDL_BUTTON_LEFT, tcur->getX(), tcur->getY() );

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
            if (m_pOutletCursor2X) {
                m_pOutletCursor2X->GetData()->SetFloat( tuioCursor->getX() );
                m_pOutletCursor2X->Update( true );
            }
            if (m_pOutletCursor2Y) {
                m_pOutletCursor2Y->GetData()->SetFloat( tuioCursor->getY() );
                m_pOutletCursor2Y->Update( true );
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


  Init( p_pOutlets, port );
}

bool
moNetTUIOListener::Init( moOutlets* p_pOutlets, int port ) {

  m_pOutlets = p_pOutlets;

  moOutlet* pOutlet = NULL;

  for( int i=0; i<m_pOutlets->Count(); i++) {

    pOutlet = m_pOutlets->Get(i);

    if (pOutlet) {
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

    bool res;
    moEvent *tmp;
    moEvent *actual;


    //clean every outlet!!! important
    for(i=0; i<GetOutlets()->Count();i++) {
        moOutlet* pOutlet = GetOutlets()->Get(i);
        if (pOutlet) {
            pOutlet->GetMessages().Empty();
            pOutlet->Update(false);
        }
    }


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
