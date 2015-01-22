#include"supereg/sup3d.h"

void get3dpoint640x350(float x1,float y1,float z1, TCAMARA3D *CAM)
 {
  float HM,DIVI,Hx,Hy,Hz;
  //CAM->constante=-(CAM->u.x*CAM->Ox+CAM->u.y*CAM->Oy+CAM->u.z*CAM->Oz);
    HM=(CAM->u.x*x1+CAM->u.y*y1+CAM->u.z*z1+CAM->constante);
    if(HM>0.1)
    {
    Hx=(x1-CAM->Ox)-HM*CAM->u.x;
    Hy=(y1-CAM->Oy)-HM*CAM->u.y;
    Hz=(z1-CAM->Oz)-HM*CAM->u.z;
    DIVI=CAM->foco/HM;
    CAM->get3d.y=(short)(175-(CAM->w.x*Hx+CAM->w.y*Hy+CAM->w.z*Hz)*DIVI);
    CAM->get3d.x=(short)(320-(CAM->v.x*Hx+CAM->v.y*Hy+CAM->v.z*Hz)*DIVI);
    }
}

void get3dpoint640x480(float x1,float y1,float z1, TCAMARA3D *CAM)
 {
  float HM,DIVI,Hx,Hy,Hz;
  //CAM->constante=-(CAM->u.x*CAM->Ox+CAM->u.y*CAM->Oy+CAM->u.z*CAM->Oz);
    HM=(CAM->u.x*x1+CAM->u.y*y1+CAM->u.z*z1+CAM->constante);
    if(HM>0.1)
    {
    Hx=(x1-CAM->Ox)-HM*CAM->u.x;
    Hy=(y1-CAM->Oy)-HM*CAM->u.y;
    Hz=(z1-CAM->Oz)-HM*CAM->u.z;
    DIVI=CAM->foco/HM;
    CAM->get3d.y=(short)(240-(CAM->w.x*Hx+CAM->w.y*Hy+CAM->w.z*Hz)*DIVI);
    CAM->get3d.x=(short)(320-(CAM->v.x*Hx+CAM->v.y*Hy+CAM->v.z*Hz)*DIVI);
    }
}


void get3dpoint640x480_fast_0(float x1,float y1,float z1, TCAMARA3D *CAM)
 {
  float HM,DIVI,Hx,Hy,Hz;
  //CAM->constante=-(CAM->u.x*CAM->Ox+CAM->u.y*CAM->Oy+CAM->u.z*CAM->Oz);
    HM=(CAM->u.x*x1+CAM->u.y*y1+CAM->u.z*z1+CAM->constante);
    if(HM>0.1)
    {
    Hx=x1-HM*CAM->u.x;
    Hy=y1-HM*CAM->u.y;
    Hz=(z1-CAM->Oz)-HM*CAM->u.z;
    DIVI=CAM->foco/HM;
    CAM->get3d.y=(short)(240-(CAM->w.x*Hx+CAM->w.y*Hy+CAM->w.z*Hz)*DIVI);
    CAM->get3d.x=(short)(320-(CAM->v.x*Hx+CAM->v.y*Hy+CAM->v.z*Hz)*DIVI);
    }
}


void get3dpoint320x200(float x1,float y1,float z1, TCAMARA3D *CAM)
 {
  float HM,DIVI,Hx,Hy,Hz;
  //CAM->constante=-(CAM->u.x*CAM->Ox+CAM->u.y*CAM->Oy+CAM->u.z*CAM->Oz);
    HM=(CAM->u.x*x1+CAM->u.y*y1+CAM->u.z*z1+CAM->constante);
    if(HM>0.1)
    {
    Hx=(x1-CAM->Ox)-HM*CAM->u.x;
    Hy=(y1-CAM->Oy)-HM*CAM->u.y;
    Hz=(z1-CAM->Oz)-HM*CAM->u.z;
    DIVI=160/HM;
    CAM->get3d.y=(short)(100-(CAM->w.x*Hx+CAM->w.y*Hy+CAM->w.z*Hz)*DIVI);
    CAM->get3d.x=(short)(160-(CAM->v.x*Hx+CAM->v.y*Hy+CAM->v.z*Hz)*DIVI);
    }
}


void get3dpoint320x240(float x1,float y1,float z1, TCAMARA3D *CAM)
 {
  float HM,DIVI,Hx,Hy,Hz;

  HM=(CAM->u.x*x1+CAM->u.y*y1+CAM->u.z*z1+CAM->constante);
    if(HM>0.1)
    {
    Hx=(x1-CAM->Ox)-HM*CAM->u.x;
    Hy=(y1-CAM->Oy)-HM*CAM->u.y;
    Hz=(z1-CAM->Oz)-HM*CAM->u.z;
    DIVI=160/HM;
    CAM->get3d.y=(short)(120-(CAM->w.x*Hx+CAM->w.y*Hy+CAM->w.z*Hz)*DIVI);
    CAM->get3d.x=(short)(160-(CAM->v.x*Hx+CAM->v.y*Hy+CAM->v.z*Hz)*DIVI);
    }
}



void inivectors(float xo, float yo, float zo, TCAMARA3D *CAM)
{
  CAM->Ox=xo; CAM->Oy=yo; CAM->Oz=zo;
  CAM->u.x=1.0; CAM->u.y=0.0; CAM->u.z=0.0;
  CAM->v.x=0.0; CAM->v.y=1.0; CAM->v.z=0.0;
  CAM->w.x=0.0; CAM->w.y=0.0; CAM->w.z=1.0;
  CAM->foco = 320;
}

void movevector(struct vecteurtype v1,struct vecteurtype v2, struct vecteurtype v3, float angulo, TCAMARA3D *CAM)
{
  struct vecteurtype R;
  float cosangle1,cosangle2,sinangle1,sinangle2,cossx,cossy;
  int D,alf;

   alf=1;
   R.x=-v3.y; R.y=v3.x;
   R.norme=sqrt(fabs(R.x*R.x+R.y*R.y));
   if(v1.z<0)  D=-1; else D=1;
   if(R.norme==0)
     {
     cosangle1=v1.x; sinangle1=v1.y;
     cossx=1; cossy=0;
     if(v3.z<0) alf=-1; else alf=1;
     v3.z=1;}
     else
     {
     cossx=R.x/R.norme; cossy=R.y/R.norme;
     cosangle1=(v1.x*R.x+v1.y*R.y)/R.norme;
     sinangle1=D*sqrt(1-cosangle1*cosangle1);
     }
  cosangle2=-sinangle1;
  sinangle2=cosangle1;
  cosangle2=cosangle2*cos(angulo)-sinangle2*sin(alf*angulo);
  cosangle1=cosangle1*cos(angulo)-sinangle1*sin(alf*angulo);
  sinangle1=-cosangle2;
  sinangle2=cosangle1;

  v1.x=cosangle1*cossx-sinangle1*v3.z*cossy;
  v1.y=cosangle1*cossy+sinangle1*v3.z*cossx;
  v1.z=sinangle1*R.norme;

  v2.x=cosangle2*cossx-sinangle2*v3.z*cossy;
  v2.y=cosangle2*cossy+sinangle2*v3.z*cossx;
  v2.z=sinangle2*R.norme;

  CAM->vecteur1=v1;
  CAM->vecteur2=v2;
}


void viewfrom(float ax, float ay, float az, TCAMARA3D *CAM)
{
  struct point3d C;

  C.x=ax; C.y=ay; C.z=az;
  CAM->u.x=-C.x/(sqrt(C.x*C.x+C.z*C.z+C.y*C.y));
  CAM->u.y=-C.y/(sqrt(C.x*C.x+C.z*C.z+C.y*C.y));
  CAM->u.z=-C.z/(sqrt(C.x*C.x+C.y*C.y+C.z*C.z));
  CAM->w.z=-(C.x*CAM->u.x+C.y*CAM->u.y)/sqrt(C.x*C.x+C.y*C.y);
  CAM->w.x=-CAM->u.z*CAM->u.x/sqrt(CAM->u.x*CAM->u.x+CAM->u.y*CAM->u.y);
  CAM->w.y=(-CAM->w.x*CAM->u.x-CAM->w.z*CAM->u.z)/CAM->u.y;
  movevector(CAM->w,CAM->v,CAM->u,PI,CAM);
  CAM->v = CAM->vecteur2;
  CAM->Ox=C.x; CAM->Oy=C.y; CAM->Oz=C.z;

}

void viewfromto(struct point3d A, struct point3d M, TCAMARA3D *CAM)

{ struct point3d C;
  C.x=A.x-M.x; C.y=A.y-M.y; C.z=A.z-M.z;
  CAM->u.x=-C.x/(sqrt(C.x*C.x+C.z*C.z+C.y*C.y));
  CAM->u.y=-C.y/(sqrt(C.x*C.x+C.z*C.z+C.y*C.y));
  CAM->u.z=-C.z/(sqrt(C.x*C.x+C.y*C.y+C.z*C.z));
  CAM->w.z=-(C.x*CAM->u.x+C.y*CAM->u.y)/sqrt(C.x*C.x+C.y*C.y);
  CAM->w.x=-CAM->u.z*CAM->u.x/sqrt(CAM->u.x*CAM->u.x+CAM->u.y*CAM->u.y);
  CAM->w.y=(-CAM->w.x*CAM->u.x-CAM->w.z*CAM->u.z)/CAM->u.y;
  movevector(CAM->w,CAM->v,CAM->u,PI,CAM);
  CAM->v = CAM->vecteur2;
  CAM->Ox=A.x; CAM->Oy=A.y; CAM->Oz=A.z;

}


void turnview(float angleuv, float anglevw, float anglewu, TCAMARA3D *CAM)
{
  if(angleuv!=0) {
    movevector(CAM->u,CAM->v,CAM->w,angleuv,CAM);
    CAM->u = CAM->vecteur1; CAM->v = CAM->vecteur2;}
  if(anglevw!=0) {
    movevector(CAM->v,CAM->w,CAM->u,anglevw,CAM);
    CAM->v = CAM->vecteur1; CAM->w = CAM->vecteur2;}
  if(anglewu!=0) {
    movevector(CAM->w,CAM->u,CAM->v,anglewu,CAM);
    CAM->w = CAM->vecteur1; CAM->u = CAM->vecteur2;}

}

/*hace que la camara se desplace sobre
un circulo plano paralelo al piso
de un angulo preciso, el centro
del dicho circulo es un
punto definido por el usuario
la camara enfocara directamente a ese punto*/

void girar(float ang, struct point3d M, TCAMARA3D *CAM)
{
  float dist;
  CAM->angle = CAM->angle+CAM->ang;
  dist = sqrt((M.x-CAM->Ox)*(M.x-CAM->Ox)+(M.y-CAM->Oy)*(M.y-CAM->Oy));
  CAM->Ox=cos(CAM->angle)*dist+M.x;
  CAM->Oy=sin(CAM->angle)*dist+M.y;

}

/*Desplaza la camara, sus coordenadas de
posicion sumandole o restandole valores
arbitrarios a su posicion actual*/

void movepointview(float valx, float valy, float valz, TCAMARA3D *CAM)

{
CAM->Ox = CAM->Ox+valx;
CAM->Oy = CAM->Oy+valy;
CAM->Oz = CAM->Oz+valz;
}


