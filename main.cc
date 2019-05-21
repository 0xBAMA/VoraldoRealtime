#include <iostream> //terminal I/O
#include <unistd.h> //sleep
#include <cstdlib>  //random number generation
#include <vector>   //container

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLUT
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

// Shader Compilation
#include "Shader.h"


// glsl-style Vector and Matrix Library
#include "glm/glm.hpp"

typedef glm::vec4  color4;
typedef glm::vec4  point4;

const int NumVertices = 70000;

point4 points[NumVertices];
color4 colors[NumVertices];

// // Vertices of a unit cube centered at origin, sides aligned with axes
// point4 vertices[8] = {
//     point4( -0.5, -0.5,  0.5, 1.0 ),
//     point4( -0.5,  0.5,  0.5, 1.0 ),
//     point4(  0.5,  0.5,  0.5, 1.0 ),
//     point4(  0.5, -0.5,  0.5, 1.0 ),
//     point4( -0.5, -0.5, -0.5, 1.0 ),
//     point4( -0.5,  0.5, -0.5, 1.0 ),
//     point4(  0.5,  0.5, -0.5, 1.0 ),
//     point4(  0.5, -0.5, -0.5, 1.0 )
// };
//
// // RGBA colors
// color4 vertex_colors[8] = {
//     color4( 0.0, 0.0, 0.0, 1.0 ),  // black
//     color4( 1.0, 0.0, 0.0, 1.0 ),  // red
//     color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
//     color4( 0.0, 1.0, 0.0, 1.0 ),  // green
//     color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
//     color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
//     color4( 1.0, 1.0, 1.0, 1.0 ),  // white
//     color4( 0.0, 1.0, 1.0, 1.0 )   // cyan
// };

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };

GLfloat pointsize = 1.0;

GLuint  theta;  // The location of the "theta" shader uniform variable

//----------------------------------------------------------------------------

// // quad generates two triangles for each face and assigns colors
// //    to the vertices
int Index = 0;
// void
// quad( int a, int b, int c, int d )
// {
//     colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
//     colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
//     colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
//     colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
//     colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
//     colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
// }
//
// //----------------------------------------------------------------------------
//
// // generate 12 triangles: 36 vertices and 36 colors
// void
// colorcube()
// {
//     quad( 1, 0, 3, 2 );
//     quad( 2, 3, 7, 6 );
//     quad( 3, 0, 4, 7 );
//     quad( 6, 5, 1, 2 );
//     quad( 4, 5, 6, 7 );
//     quad( 5, 4, 0, 1 );
// }

void gencube()
{
	GLfloat num;
	std::vector<GLfloat> range = {-0.5,-0.475,-0.45,-0.425,-0.4,-0.375,-0.35,-0.325,-0.3,-0.275,-0.25,-0.225,-0.2,-0.175,-0.15,-0.125,-0.1,-0.075,-0.05,-0.025,1.59595e-16,0.025,0.05,0.075,0.1,0.125,0.15,0.175,0.2,0.225,0.25,0.275,0.3,0.325,0.35,0.375,0.4,0.425,0.45,0.475,0.5};

	for(const auto& x : range){
		for(const auto& y : range){
			for(const auto& z : range){
				points[Index] = point4(x,y,z,1.0);

				//num = double(rand())/double(RAND_MAX);

				colors[Index] = color4(x+0.5,y+0.5,z+0.5,0.25);
				Index++;
			}
		}
	}

	std::cout << Index;
	//done
}

//----------------------------------------------------------------------------

// OpenGL initialization
void
init( Shader s )
{
    // colorcube();

		gencube();

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		  NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );

    // Use the shader program ( input from and compiled in main() )

		s.Use();


    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( s.Program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			  (GLvoid*) (0) );



    GLuint vColor = glGetAttribLocation( s.Program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
			   (GLvoid*) (sizeof(points)) );

    theta = glGetUniformLocation( s.Program, "theta" );

    glEnable( GL_DEPTH_TEST );

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

//----------------------------------------------------------------------------

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		// theShader.Use();

    glUniform3fv( theta, 1, Theta );
    glDrawArrays( GL_POINTS, 0, NumVertices );

    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
	case 033: // Escape Key
	case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
	    break;
	case 'a':
			pointsize += 1;
			glPointSize(pointsize);
			break;
  case 'z':
			pointsize -= 1;
			glPointSize(pointsize);
			break;
    }
}

//----------------------------------------------------------------------------

void
mouse( int button, int state, int x, int y )
{
    if ( state == GLUT_DOWN ) {
	switch( button ) {
	    case GLUT_LEFT_BUTTON:    Axis = Xaxis;  break;
	    case GLUT_MIDDLE_BUTTON:  Axis = Yaxis;  break;
	    case GLUT_RIGHT_BUTTON:   Axis = Zaxis;  break;
	}
    }
}

//----------------------------------------------------------------------------

void
idle( void )
{
    Theta[Axis] += 0.1;

    if ( Theta[Axis] > 360.0 ) {
				Theta[Axis] -= 360.0;
    }

    glutPostRedisplay();
}

//----------------------------------------------------------------------------




int main( int argc, char **argv )
{
	std::cout << "very first thing" << std::endl;
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize( 750, 750 );
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutCreateWindow( "Color Cube" );
	std::cout << "glut code done" << std::endl;

	// glm::vec4 vec;
	//
	// usleep(9000000);
	//
	// std::cout << "gottem" << std::endl;



	glewExperimental = GL_TRUE;
  glewInit();


	Shader theShader( "shaders/vertex.glsl", "shaders/fragment.glsl" );
	std::cout << "Shader compilation finished" << std::endl;

	std::cout << "Initializing" << std::endl;


  init( theShader );

  glutDisplayFunc( display );
  glutKeyboardFunc( keyboard );
  glutMouseFunc( mouse );
  glutIdleFunc( idle );

  glutMainLoop();

	std::cout << "Exiting" << std::endl;

	return 0;
}
