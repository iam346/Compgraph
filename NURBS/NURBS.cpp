/**
 * Author: Badó Gergely József
 *
 * Használat:
 * a pontok alapértelmezetben mozgathatóak egérrel
 * ctrl+egér kattintás új pont lerakása
 * fel-le nyilak fokszám változtatás
 * w-s gombokkal kiválaszthatunk egy kívánt pontot aminek később az a-d gombokkal változtathatjuk a súlyát
 * k gomb megnyomásával megkapjuk a jelenlegi csomópont vektort és meg kell adnunk egy újat
 *
 **/
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <iostream>
#include <vector>
#include <math.h>

#include "../inc/point2D.h"

#define PI 3.141592653589793

typedef graph::point2D<double> point2D;

point2D size ( 800,600 );

std::vector<point2D> control_points;
std::vector<GLdouble> knots;
std::vector<GLdouble> weights;

int changeWeight=-1;//melyik pontnak akarlyuk a súlyát változtatni
int degree=3;//fokszám
GLint selectedPointIndex=-1;//kiválasztott pont mozgatásnál
bool move=false, allow_new_point=false;//mozgatás, új pont letételének engedélyezése

/**
 * Inicializáló függvény
 **/
void init()
{
    glClearColor ( 0, 0, 0, 1 );
    glMatrixMode ( GL_PROJECTION );
    glLoadIdentity();
    glOrtho ( 0, size.getX(), 0, size.getY(), 0.0, 1.0 );

    glEnable ( GL_POINT_SMOOTH );
    glPointSize ( 4 );
    glLineWidth ( 2.0 );
}

/**
 *A képletben szereplő lambda
 **/
double lambda ( int j, int k, int l, double u )
{
    //std::cout<<"l-begin"<<std::endl;
    if ( l==0 )
        return 1;
    //std::cout<<"l-end"<<std::endl;
    return ( u-knots[j] ) / ( knots[j+k-l]-knots[j] );
}

/**
 *A képletben szereplő súly rekurzív függvény
 **/
double w ( int j, int k, int l, double u )
{
    //std::cout<<"w-begin"<<std::endl;
    if ( l==0 )
        return weights.at ( j );
    //std::cout<<"w-end"<<std::endl;
    return lambda ( j,k,l,u ) *w ( j,k,l-1,u ) + ( 1.0-lambda ( j,k,l,u ) ) *w ( j-1,k,l-1,u );
}

/**
 *A képletben szereplő rekurzív kifejezés
 **/
point2D getNURBSPoint ( int j, int k, int l, double u )
{
    //std::cout<<"n-begin"<<std::endl;
    if ( l==0 )
        return control_points.at ( j );

    point2D temp= getNURBSPoint ( j,k,l-1,u ) *lambda ( j,k,l,u ) *w ( j,k,l-1,u )
                  + getNURBSPoint ( j-1,k,l-1,u ) * ( 1-lambda ( j,k,l,u ) ) *w ( j-1,k,l-1,u ) ;
    //std::cout<<"n-end"<<std::endl;
    return temp * ( 1.0/w ( j,k,l,u ) );
}

/**
 *Kreál egy a fokszámhoz és kontrolpontszámhoz tartozó lehetséges csomópontvektort
 **/
std::vector<GLdouble> getKnots ( int degree, int controlNum )
{
    int size=controlNum+degree+1;
    std::vector<GLdouble> temp;

    //degree darab "keret"  van az elején meg mivel maximum degree érték lehet egyforma
    for ( int i=0; i<degree; i++ )
        temp.push_back ( 0 );

    for ( int i=0; i<size-2*degree; i++ )
        temp.push_back ( i );

    // az utolsó érték ha nulla volt egyre kell váltani ha 1 vagy bármi más volt akkor pedig egyeln nagyobbra
    GLdouble last=1;
    if ( temp.at ( temp.size()-1 ) !=0 )
        last=temp.at ( temp.size()-1 );

    for ( int i=0; i<degree; i++ )
        temp.push_back ( last );

    return temp;
}

/**
 * Renderelést végző központi függvény
 **/
void render()
{
    glClear ( GL_COLOR_BUFFER_BIT );

    glColor3f ( 0,0,1 );
    glBegin ( GL_LINE_STRIP );
    for ( point2D point : control_points )
    {
        glVertex2d ( point.getX(),point.getY() );
    }
    glEnd();

    //pontok kirajzolása
    glColor3f ( 0,1,0 );
    for ( point2D point : control_points )
    {
        glBegin ( GL_POINTS );
        glVertex2d ( point.getX(),point.getY() );
        glEnd();
    }

    if ( changeWeight>=0 )
    {
        glColor3f ( 1,0,0 );
        glPointSize ( 10 );
        glBegin ( GL_POINTS );
        glVertex2d ( control_points.at ( changeWeight ).getX(),control_points.at ( changeWeight ).getY() );
        glEnd();
        glPointSize ( 4 );
        glColor3f ( 0,1,0 );
    }

    if ( control_points.size() >degree )
    {
        glBegin ( GL_LINE_STRIP );

        for ( int i=0; i<control_points.size(); i++ )
        {
            for ( double u=knots.at ( i ); u<knots.at ( i+1 ); u+=0.01 )
            {
                try
                {
                    point2D temp= getNURBSPoint ( i,degree+1,degree,u );
                    glVertex2f ( temp.getX(),temp.getY() );
                }
                catch ( ... )
                {
                    std::cout<<"ERROR"<<std::endl;
                }

            }
        }
        glVertex2f ( control_points.at ( control_points.size()-1 ).getX(),control_points.at ( control_points.size()-1 ).getY() );
        glEnd();
    }
}

//vagy -1 ha nincs aktív pont egyébként az aktív pont indexe
GLint getActivePoint2 ( const std::vector<point2D> &points, GLint sens, const point2D &mousePoint )
{
    for ( GLint i=0; i < points.size(); i++ )
        if ( points.at ( i ).dist2 ( mousePoint ) < sens*sens )
        {
            return i;
        }

    return -1;
}

void mouseMove ( GLFWwindow* window, GLdouble xMouse, GLdouble yMouse )
{
    if ( move )
    {
        if ( selectedPointIndex != -1 ) //hogyha jó helyre kattintottunk
        {
            control_points.at ( selectedPointIndex ).setX ( xMouse ); //mozgatunk
            control_points.at ( selectedPointIndex ).setY ( size.getY() - yMouse );
        }
        else
        {
            selectedPointIndex = getActivePoint2 ( control_points, 10, point2D ( xMouse, size.getY() - yMouse ) );
        }
    }
}

void mouseButton ( GLFWwindow* window, GLint button, GLint action, GLint mods )
{
    //ha lenyomtuk, és a ballt nyomtuk le és nincs elég pont
    if ( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && allow_new_point )
    {
        GLdouble xMouse,yMouse;
        glfwGetCursorPos ( window,&xMouse,&yMouse );

        control_points.push_back ( point2D ( xMouse, size.getY()-yMouse ) );
        weights.push_back ( 1.0 );

        if ( control_points.size() >degree )
            knots=getKnots ( degree,control_points.size() );

    }//ha lenyomtuk, és a ballt nyomtuk le és már megvan a kellõ mennyiségû pont
    else if ( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS )
    {
        move = true;
    }//ha felengedtük
    else if ( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE )
    {
        move = false;
        selectedPointIndex = -1;
    }
}

void keyListener ( GLFWwindow* windows, GLint key, GLint scanCode, GLint action, GLint mods )
{
    if ( action == GLFW_PRESS ) //le lett nyomva valami gomb akkor
    {
        switch ( key )
        {
        case GLFW_KEY_UP:
            if ( degree<control_points.size()-1 )
            {
                degree++;
                knots=getKnots ( degree,control_points.size() );
            }
            std::cout<<"Fokszám: "<<degree<<std::endl;
            break;
        case GLFW_KEY_DOWN:
            if ( degree>1 )
            {
                degree--;
                knots=getKnots ( degree,control_points.size() );
            }
            std::cout<<"Fokszám: "<<degree<<std::endl;
            break;
        case GLFW_KEY_LEFT_CONTROL:
            allow_new_point = true;
            break;
        case GLFW_KEY_W:
            if ( changeWeight< ( int ) control_points.size()-1 )
                changeWeight++;
            break;
        case GLFW_KEY_S:
            if ( changeWeight> 0 )
                changeWeight--;
            break;
        case GLFW_KEY_D:
            if ( changeWeight>=0 )
            {
                weights.at ( changeWeight ) +=0.1;
                std::cout<<changeWeight<<" pont súlya: "<<weights.at ( changeWeight ) <<std::endl;
            }
            break;
        case GLFW_KEY_A:
            if ( changeWeight>=0 )
            {
                weights.at ( changeWeight )-=0.1;
                std::cout<<changeWeight<<" pont súlya: "<<weights.at ( changeWeight ) <<std::endl;
            }
            break;
        case GLFW_KEY_K:
            std::cout<<"Jelenlegi csomópont értékek:"<<std::endl;
            for ( int i=0; i<knots.size(); i++ )
                std::cout<<knots.at ( i ) <<std::endl;
            std::cout<<std::endl;

            std::cout<<"Adjon meg "<<knots.size() <<" új csomópontot"<<std::endl;
            for ( int i=0; i<knots.size(); i++ )
                std::cin>>knots[ i ];

            for ( int i=0; i<knots.size(); i++ )
                std::cout<<knots.at ( i ) <<std::endl;
            std::cout<<std::endl;
            break;
        default:
            break;
        }
    }
    else if ( action==GLFW_RELEASE )
    {
        if ( key==GLFW_KEY_LEFT_CONTROL )
            allow_new_point=false;
    }
}

int main ( int argc, char** argv )
{
    GLFWwindow* window;

    if ( !glfwInit() ) //nincs betötlve
    {
        return -1;
    }

    window = glfwCreateWindow ( size.getX(), size.getY(), "B-Spline algoritmus", NULL, NULL );
    std::cout<<"ablak end"<<std::endl;
    if ( !window )
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent ( window );

    glfwSetCursorPosCallback ( window, mouseMove );
    glfwSetMouseButtonCallback ( window, mouseButton );
    glfwSetKeyCallback ( window, keyListener );

    init();

    while ( !glfwWindowShouldClose ( window ) )
    {
        render();
        glfwSwapBuffers ( window );
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

