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

GLint selectedPointIndex=-1,repeat_number=1;
bool move=false, allow_new_point=false, first=true;

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

void draw_subdivision()
{
    std::vector<point2D> control_points_copy=control_points;

    for ( int counter=0; counter<repeat_number; counter++ )
    {
        std::vector<point2D> q_points;

        for ( int i=0; i<control_points_copy.size()-1; i++ )
            q_points.push_back ( ( control_points_copy[i]+control_points_copy[i+1] ) * ( 1.0/2.0 ) );

        for ( int i=1; i<control_points_copy.size()-1; i++ )
            control_points_copy[i]=control_points_copy[i]* ( 1.0/2.0 ) + ( q_points[i-1]+q_points[i] ) * ( 1.0/4.0 );

            for ( int i=control_points_copy.size()-1; i>0; i-- )
              control_points_copy.insert ( control_points_copy.begin() +i,q_points[i-1] );
    }

    glBegin ( GL_LINE_STRIP );
    for ( point2D point : control_points_copy ) //point2D point : control_points_copy
    {
        glVertex2d ( point.getX(),point.getY() );
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

    glBegin ( GL_LINE_STRIP );
    for ( point2D point : control_points )
    {
        glVertex2d ( point.getX(),point.getY() );
    }
    glEnd();

    if ( control_points.size() >=3 )
        draw_subdivision();

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
            repeat_number++;
            break;

        case GLFW_KEY_DOWN:
            if ( repeat_number>0 )
                repeat_number--;

        case GLFW_KEY_LEFT_CONTROL:
            allow_new_point = true;
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


