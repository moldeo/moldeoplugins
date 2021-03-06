#include<time.h>
#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include "moldeo.h"
#include "moEffectGrid.h"
#include "supereg/pcx.h"
#include "supereg/sup3d.h"

/*agregado para OPENGL*/
// Full path for openGL(by Andres). Deberia ser path relativo!!!
/*LISTO OPENGL*/

#if !defined(__TIPOS_H__)
#define __TIPOS_H__

#define TIPO tdato  //Este es el tipo que vamos a usar para las alturas del relieve
#define TIPO2 tdato2
#define TIPOP tdatop
#define TIPO2P tdato2p

#if !defined(PI)
#define PI 3.1416
#endif

typedef struct dato
{
 int alt;//altitud(entero)
 char aspereza;//0..255 rugosidad
 char anim;//estado de animacion
 char colorido;
 char origen;
} tdato;

typedef struct dato2
{
 int alt;
 int altaux;
 char cuarto;//esto es para las texturas
 char color;
 int NP;
 float txa,txb;
 float tya,tyb;

 //triangulos |A/B|
 //char subdivideA;|/  A valores: s/n
 //char subdivideB;/|  B valores: s/n
} tdato2;

typedef tdato* tdatop;
typedef tdato2* tdato2p;
typedef int* TBITMAP;

//#define ANCHO(17+8+8)
//#define ANCHO(25+8+8)
#define ANCHO 45
#define ESCALAS 12
//#define ESCALAS 17
#define FORMATOCOLOR 8
//#define METRO 20
#define METRO 20
#define NTEX 16
#define NTEXd2 8

//Paleta
#define NALTS 8
//#define NTONOS 32
#define NTONOS 256
#define DIST 500000.0


/*
#define RESX 320
#define RESY 240
#define RESXD2 160.0
#define RESYD2 120.0
*/
#define RESX 800
#define RESY 600
#define RESXD2 400.0
#define RESYD2 300.0

typedef struct GridDefinition {

    double m_red;
    double m_green;
    double m_blue;
    double m_alpha;

    int m_escalas;
    int m_dimension;

    float m_planet_factor;
    float m_torus_factor;
    float m_wave_amplitude;
    float m_height_multiply;

    float map_position_x;
    float map_position_y;

    float m_minimum_surface_altitude;

    /*WIREFRAME*/
    bool  m_wireframe_mode;
    float m_wireframe_width;
    float m_wireframe_configuration;

    float m_lightx;
    float m_lighty;
    float m_lightz;
    float m_light_r;
    float m_light_g;
    float m_light_b;

} TGridDefinition;

void holamundo(void);

#endif


