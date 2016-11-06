#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <iostream>
#include <vector>
#include <math.h>

#include "../inc/point2D.h"

#define PI 3.141592653589793
typedef graph::point2D<GLdouble> point2D;
point2D size ( 800,600 );
std::vector<point2D> control_points;

int degree=3;
GLint selectedPointIndex=-1;
bool move=false, allow_new_point=true, first=true;

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

void curve_1_degree()
{
    glColor3f ( 0,1,0 );
    glBegin ( GL_LINE_STRIP );
    std::vector<point2D> temp_control_points;

    for ( int i=0; i<=control_points.size()-1; i++ )//-2
    {
        for ( GLdouble t=0.0; t<=1.0; t+=0.01 )
        {
            point2D newpoint ( control_points.at(i).getX(),control_points.at(i).getY());

            glVertex2d ( newpoint.getX(),newpoint.getY() );

        }
    }
    glEnd();

}

void curve_2_degree()
{
    glColor3f ( 0,1,0 );
    glBegin ( GL_LINE_STRIP );
    std::vector<point2D> temp_control_points;

    temp_control_points.insert ( temp_control_points.end(), control_points.begin(), control_points.end() );
   
    

    for ( int i=0; i<=temp_control_points.size()-3; i++ )
    {
        for ( GLdouble t=0.0; t<=1.0; t+=0.01 )
        {
           // point2D newpoint ();


            //glVertex2d ( newpoint.getX(),newpoint.getY() );

        }
    }
    glEnd();

}

void curve_3_degree()
{
    glColor3f ( 0,1,0 );
    //http://aries.ektf.hu/~hz/pdf-tamop/pdf-01/html/ch09.html#x1-65006r2
    std::vector<point2D> temp_control_points;
    temp_control_points.push_back ( control_points.at ( 0 ) );
    temp_control_points.push_back ( control_points.at ( 0 ) );
    temp_control_points.insert ( temp_control_points.end(), control_points.begin(), control_points.end() );
    temp_control_points.push_back ( control_points.back() );
    temp_control_points.push_back ( control_points.back() );
    
    glBegin ( GL_LINE_STRIP );
    for ( int i=0; i<=temp_control_points.size()-4; i++ )
    {
        for ( GLdouble t=0.0; t<=1.0; t+=0.01 )
        {
            point2D newpoint ( 1.0/6.0*pow ( 1-t,3 ) *temp_control_points.at ( i ).getX() +
                               ( 1.0/2.0*pow ( t,3.0 )-pow ( t,2 ) +2.0/3.0 ) *temp_control_points.at ( i+1 ).getX() +
                               ( -1.0/2.0*pow ( t,3 ) +1.0/2.0*pow ( t,2 ) +1.0/2.0*t+1.0/6.0 ) *temp_control_points.at ( i+2 ).getX() +
                               1.0/6.0*pow ( t,3 ) *temp_control_points.at ( i+3 ).getX(),
                               1.0/6.0*pow ( 1-t,3 ) *temp_control_points.at ( i+0 ).getY() +
                               ( 1.0/2.0*pow ( t,3.0 )-pow ( t,2 ) +2.0/3.0 ) *temp_control_points.at ( i+1 ).getY() +
                               ( -1.0/2.0*pow ( t,3 ) +1.0/2.0*pow ( t,2 ) +1.0/2.0*t+1.0/6.0 ) *temp_control_points.at ( i+2 ).getY() +
                               1.0/6.0*pow ( t,3 ) *temp_control_points.at ( i+3 ).getY() );


            glVertex2d ( newpoint.getX(),newpoint.getY() );
        }
    }
    glEnd();

}

void render()
{
    glClear ( GL_COLOR_BUFFER_BIT );
    glColor3f ( 0,1,0 );

    //pontok kirajzolása
    for ( point2D point : control_points )
    {
        glBegin ( GL_POINTS );
        glVertex2d ( point.getX(),point.getY() );
        glEnd();
    }

    if ( control_points.size() >3&&degree==3 )
        curve_3_degree();
    else if ( control_points.size() >2&& degree==2 )
        curve_2_degree();
    else if ( control_points.size() >1 && degree==1 )
        curve_1_degree();

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
            if ( degree<3 )
            {
                degree++;
            }
            break;
        case GLFW_KEY_DOWN:
            if ( degree>1 )
            {
                degree--;
            }
            break;
        case GLFW_KEY_P:
            allow_new_point = !allow_new_point;
            break;
        default:
            break;
        }
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


