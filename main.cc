#include <iostream> //terminal I/O
#include <unistd.h> //sleep
#include <cstdlib>  //random number generation
#include <vector>   //container

#include <math.h>   //sqrt, pow


// GLEW
#define GLEW_STATIC
#include <GL/glew.h>


// GLUT
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>


// Shader Compilation
#include "resources/shaders/Shader.h"


// glsl-style Vector and Matrix Library
#include "resources/glm/glm.hpp"


// Perlin noise - simple implementation from https://github.com/sol-prog/Perlin_Noise
#include "resources/perlin.h"


typedef glm::vec4  color4;
typedef glm::vec4  point4;

const int NumVertices = 700000;

point4 points[NumVertices];
color4 colors[NumVertices];

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

	std::vector<GLfloat> range = {-0.5,-0.48,-0.46,-0.44,-0.42,-0.4,-0.38,-0.36,-0.34,-0.32,-0.3,-0.28,-0.26,-0.24,-0.22,-0.2,-0.18,-0.16,-0.14,-0.12,-0.1,-0.08,-0.06,-0.04,-0.02,0,0.02,0.04,0.06,0.08,0.1,0.12,0.14,0.16,0.18,0.2,0.22,0.24,0.26,0.28,0.3,0.32,0.34,0.36,0.38,0.4,0.42,0.44,0.46,0.48,0.5};

	PerlinNoise p;

	for(const auto& x : range){
		for(const auto& y : range){
			for(const auto& z : range){
				points[Index] = point4(x,y,z,1.0);

				//num = double(rand())/double(RAND_MAX);

				if ( x==0 || y==0 || z==0 ){
					colors[Index] = color4(0.0,0.0,0.0,1.0);
				}
				else
				{
					// colors[Index] = color4(x+0.5,y+0.5,z+0.5,0.01);
					// colors[Index] = color4(0.0,0.0,0.3,0.1);

					float val = p.noise( 10*x, 10*y, 10*z );
					//float distance_to_zero = sqrt(pow(x2 - x1, 2) +  pow(y2 - y1, 2) +  pow(z2 - z1, 2) * 1.0);

					float distance_to_zero = sqrt( pow(x - 0, 2) +  pow(y - 0, 2) +  pow(z - 0, 2) * 1.0 );


					if( val < 0.3 ){
						colors[Index] = color4( 0.0, 0.0, 1.0, 1.0 );

					}else{
						colors[Index] = color4( val, val, val, 0.5 - pow( distance_to_zero, 2 ) );

					}


				}

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

		// vertex locations
    GLuint vPosition = glGetAttribLocation( s.Program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*) (0) );


		// vertex colors
    GLuint vColor = glGetAttribLocation( s.Program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*) (sizeof(points)) );


		// uniform value for rotation
    theta = glGetUniformLocation( s.Program, "theta" );


		// enable z buffer for occlusion
    glEnable( GL_DEPTH_TEST );


		// alpha blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		// glBlendFunc(sfactor, dfactor); ?? this is going to require more research

		// The enum values that are available for sfactor are as follows -

		// GL_ZERO,
    // GL_ONE,
    // GL_SRC_COLOR,
    // GL_ONE_MINUS_SRC_COLOR,
    // GL_DST_COLOR,
    // GL_ONE_MINUS_DST_COLOR,
    // GL_SRC_ALPHA,
    // GL_ONE_MINUS_SRC_ALPHA,
    // GL_DST_ALPHA,
    // GL_ONE_MINUS_DST_ALPHA,
    // GL_CONSTANT_COLOR,
    // GL_ONE_MINUS_CONSTANT_COLOR,
    // GL_CONSTANT_ALPHA,
    // GL_ONE_MINUS_CONSTANT_ALPHA,
    // GL_SRC_ALPHA_SATURATE

		// The enum values that are available for dfactor are as follows -

		// GL_ZERO,
    // GL_ONE,
    // GL_SRC_COLOR,
    // GL_ONE_MINUS_SRC_COLOR,
    // GL_DST_COLOR,
    // GL_ONE_MINUS_DST_COLOR,
    // GL_SRC_ALPHA,
    // GL_ONE_MINUS_SRC_ALPHA,
    // GL_DST_ALPHA,
    // GL_ONE_MINUS_DST_ALPHA,
    // GL_CONSTANT_COLOR,
    // GL_ONE_MINUS_CONSTANT_COLOR,
    // GL_CONSTANT_ALPHA,
    // GL_ONE_MINUS_CONSTANT_ALPHA


		// what color background?
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

//----------------------------------------------------------------------------

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// update the value of theta in the shader
    glUniform3fv( theta, 1, Theta );

		// the draw call
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
			glPointSize( pointsize );
			break;
  case 'z':
			pointsize -= 1;
			glPointSize( pointsize );
			break;
    }
}

//----------------------------------------------------------------------------

void
mouse( int button, int state, int x, int y )
{
    if ( state == GLUT_DOWN )
		{
			switch( button )
			{
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


	Shader theShader( "resources/shaders/vertex.glsl", "resources/shaders/fragment.glsl" );
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
