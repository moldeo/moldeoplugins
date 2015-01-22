#include "supereg/tmotor.h"


//****************************************************************************************

TEngine_Utility::TEngine_Utility( moConfig &p_config, moResourceManager* p_pResourceManager ) : m_Config(p_config), m_pResourceManager(p_pResourceManager) {

    //Aca tendria que haber una funcion del tipo cargar el supereg.ini
    Luzx = 8.0 / sqrt(129.0);Luzy = - 8.0 / sqrt(129.0);Luzz = -1.0 / sqrt(129.0);
    Inhibir = 0;
    SafeMode = 0;
    Memoria_Utilizada = 0;
    Rapidez = 20000;
    RapidezF = (float) Rapidez;
    //RapidezMaxima = 100*100000;
    RapidezMaxima = 10*20000;


    TG_RELIEVE = NULL;
    TG_NUBES = NULL;


    int escalas = m_Config.Int(moR(GRID_SCALES));
    int dimension = m_Config.Int(moR(GRID_SIZE));

    if (!( 0<escalas && escalas<30)) {
      MODebug2->Error("moEffectGrid > TEngine_Utility() > DANGER! escalas out of bound! escalas=["+
                        IntToStr(escalas)
                      +"]" );
      return;
    }
    m_GridDefinition.m_height_multiply = 1.0f;
    m_GridDefinition.m_planet_factor = 1.0f;
    m_GridDefinition.m_torus_factor = 0.0f;
    m_GridDefinition.m_wave_amplitude = 0.0f;

    m_GridDefinition.m_wireframe_mode = false;
    m_GridDefinition.m_escalas = escalas;
    m_GridDefinition.m_dimension = dimension;

    moText mapt = m_Config.Text(moR(GRID_MAP));
    char* mapa = mapt;

    moText complete_map_path("");
    complete_map_path = m_pResourceManager->GetDataMan()->GetDataPath() + moSlash + mapt;

    moFile MapFile(complete_map_path);
    if (!MapFile.Exists()) {
      MODebug2->Error("moEffectGrid > TEngine_Utility constructor > Map file missing or misconfigured! [" + complete_map_path + "]");
      return;
    }

    TG_RELIEVE = new TGrid_Utility( escalas, dimension, complete_map_path );

//	TG_NUBES = new TGrid_Utility(7,(9+8+8));



    if(TG_RELIEVE==NULL) exit(0);
    TG_RELIEVE->m_GridDefinition = m_GridDefinition;
//	if(TG_NUBES==NULL) exit(0);

}


//****************************************************************************************


TEngine_Utility::~TEngine_Utility()
{
 delete TG_RELIEVE;
}



//****************************************************************************************

//****************************************************************************************


void TEngine_Utility::Start_Engine(void)
{
  int FIN;
	FIN = 0;
	if (TG_RELIEVE==NULL) {
    MODebug2->Error("moEffectGrid TEngine_Utility::Start_Engine > Couldn't start engine. Check grid configuration.");
    return;
	}
  TG_RELIEVE->Posicionar((TG_RELIEVE->Mapa->MapaAncho / 2),(TG_RELIEVE->Mapa->MapaAlto / 2),(float) 100000.0,&CAM3D);
  //TG_RELIEVE->Posicionar(10,20,(float) 5000.0,&CAM3D);
  //TG_NUBES->Posicionar(10,20,(float) 5000.0,&CAM3D);
}

//******************

void TEngine_Utility::Loop_Engine(void)
{
  if (TG_RELIEVE==NULL) {
    return;
  }

  TG_RELIEVE->m_GridDefinition = m_GridDefinition;

  TG_RELIEVE->ActualizarGrillas(Rapidez,&CAM3D);
  TG_RELIEVE->TiempoCorriendo();
  //TG_RELIEVE->ProyectaryDibujar(pagina_virtual,&CAM3D);

  if ( m_GridDefinition.m_wireframe_mode) {
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glLineWidth( m_GridDefinition.m_wireframe_width );
  } else {
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  }

  TG_RELIEVE->glProyectaryDibujar(pagina_virtual,&CAM3D);

  InertialMotion();
//nota:
	 //si la camara se encuentra entre las nubes  el relieve, entonces primero
	 //se dibuja primero las nubes y luego el relieve
	 //si se esta por encima de las nubes es alreves
	 //aunque lo mejor es tirar todo junto


	 //TG_NUBES->ActualizarGrillas(Rapidez,&CAM3D);
     //TG_NUBES->TiempoCorriendo();
     //TG_RELIEVE->ProyectaryDibujar(pagina_virtual,&CAM3D);
	 //TG_NUBES->glProyectaryDibujar(pagina_virtual,&CAM3D);

}

//******************


//****************************************************************************************

void TEngine_Utility::Stop_Engine(void)
{
//nada
	Rapidez = 0;
}



//****************************************************************************************

void
TEngine_Utility::ImprimirTodaLaInfo(void)
{
//nada
}

//****************************************************************************************

void
TEngine_Utility::Trim(int val)
{
	float vf;
	vf = 15 *(float) val /(float) 256.0;
	turnview(-vf * PI/150,0.0,0.0,&CAM3D);

}

//****************************************************************************************

void
TEngine_Utility::Pitch(int val)
{
	float vf;
	vf = 15 *(float) val /(float) 256.0;
	turnview(0.0,0.0, vf * PI/150,&CAM3D);

}

//****************************************************************************************

int additive_code0 = 1000;
int additive_code1 = 1000;
int additive_code2 = 1000;
int additive_code3 = 1000;
int additive_code4 = 1000;
int additive_code5 = 1000;
int add_add = 50;
int add_ret = 20;
int add_min = 150;
int add_max = 1000;

void
TEngine_Utility::InertialMotion() {
  //0
  if (additive_code0<1000) turnview(PI/additive_code0,0.0,0.0,&CAM3D);
  //1
  if (additive_code1<1000) turnview(-PI/additive_code1,0.0,0.0,&CAM3D);
  //2
  if (additive_code2<1000) turnview(0.0,0.0,PI/additive_code2,&CAM3D);
  //3
  if (additive_code3<1000) turnview(0.0,0.0,-PI/additive_code3,&CAM3D);

  //4
  if (additive_code4<1000) turnview(0.0,PI/additive_code4,0.0,&CAM3D);
  //5
  if (additive_code5<1000) turnview(0.0,-PI/additive_code5,0.0,&CAM3D);

  additive_code0+= add_ret;
  if (additive_code0>add_max) additive_code0 = add_max;

  additive_code1+= add_ret;
  if (additive_code1>add_max) additive_code1 = add_max;

  additive_code2+= add_ret;
  if (additive_code2>add_max) additive_code2 = add_max;

  additive_code3+= add_ret;
  if (additive_code3>add_max) additive_code3 = add_max;

  additive_code4+= add_ret;
  if (additive_code4>add_max) additive_code4 = add_max;

  additive_code5+= add_ret;
  if (additive_code5>add_max) additive_code5 = add_max;

}

int
TEngine_Utility::Check_Keyb(int code)
{
//  TGrid *Temp;
//reescribir
//aca hay que usar SDL
       if(code==0) {
           additive_code0-= add_add;
           if (additive_code0<add_min) additive_code0 = add_min;
       }

       if(code==1) {
          additive_code1-= add_add;
          if (additive_code1<add_min) additive_code1 = add_min;
       }

       if(code==2) {
           additive_code2-= add_add;
           if (additive_code2<add_min) additive_code2 = add_min;

       }

       if(code==3) {
           additive_code3-= add_add;
           if (additive_code3<add_min) additive_code3 = add_min;

       }

       if(code==4) {
           additive_code4-= add_add;
           if (additive_code4<add_min) additive_code4 = add_min;
       }

       if(code==5) {
           additive_code5-= add_add;
           if (additive_code5<add_min) additive_code5 = add_min;
       }

       if(code==6)
       {
        if(RapidezF <= 0) RapidezF = 1;
        else RapidezF = RapidezF * 1.2;
        if(RapidezF>RapidezMaxima) RapidezF = RapidezMaxima;
        Rapidez = (int) RapidezF;
        MODebug2->Push(moText("moEffectGrid::TMotor::CheckKeyb() > Rapidez:") + IntToStr(Rapidez));
       }

       if(code==7)
       {
         if(RapidezF<1) RapidezF = 0;
         else RapidezF = RapidezF * 0.8;
         Rapidez = (int) RapidezF;
         MODebug2->Push(moText("moEffectGrid::TMotor::CheckKeyb() > Rapidez:") + IntToStr(Rapidez));
       }
/*
       if(key[KEY_ESC])
           return(-1);

       if(key[KEY_I]) CAM3D.foco-=7;
       if(key[KEY_O]) CAM3D.foco+=7;

       if(key[KEY_Q]) CAM3D.foco=-CAM3D.foco;

       if(key[KEY_Y]) {
                        Luzx = CAM3D.u.x; Luzy = CAM3D.u.y; Luzz = CAM3D.u.z;
                        //aca deberia actualizar el vector deluz en todas las grillas
                        Temp = TG_RELIEVE->Inicial;
                        while(Temp!=NULL) {
                           Temp->Luzx = Luzx;
                           Temp->Luzy = Luzy;
                           Temp->Luzz = Luzz;
                           Temp = Temp->Hijo;
                           }
                        }
       if(key[KEY_1]) {if(Inhibir<ESCALAS) Inhibir++;}
       if(key[KEY_2]) {if(Inhibir>0) Inhibir--;}
*/

        if (code==GRID_RESET) {
          if (TG_RELIEVE) TG_RELIEVE->Posicionar( 20,20, 1000, &CAM3D );
          Rapidez = 20000;
        }
return(0);

}

void
TEngine_Utility::Wireframe( int force ) {
    if (force!=-1) {
        m_GridDefinition.m_wireframe_mode = (force==1);
    } else {
        m_GridDefinition.m_wireframe_mode = !this->m_GridDefinition.m_wireframe_mode;
    }
}






