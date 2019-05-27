// basic libraries
#include <iostream> //terminal I/O
#include <unistd.h> //sleep
#include <cstdlib>  //random number generation
#include <cstdio>
#include <vector>   //container
#include <string>

#include <math.h>   //sqrt, pow




//stream class shit
using std::cout;
using std::endl;
using std::cin;



//  ___________.___   _____  _____________________
//  \__    ___/|   | /     \ \_   _____/\______   \
//    |    |   |   |/  \ /  \ |    __)_  |       _/
//    |    |   |   /    Y    \|        \ |    |   \
//    |____|   |___\____|__  /_______  / |____|_  /
//                         \/        \/         \/

#include <chrono>
//Chrono class aliases

using Clock=std::chrono::high_resolution_clock;
using milliseconds=std::chrono::milliseconds;

Clock Interval_clock;

auto tick = Clock::now(); //variable to hold start of the timekeeping
auto tock = Clock::now(); //variable to hold end of timekeeping




//    ________.____        ________________________ _________________________
//   /  _____/|    |      /   _____/\__    ___/    |   \_   _____/\_   _____/
//  /   \  ___|    |      \_____  \   |    |  |    |   /|    __)   |    __)
//  \    \_\  \    |___   /        \  |    |  |    |  / |     \    |     \
//   \______  /_______ \ /_______  /  |____|  |______/  \___  /    \___  /
//          \/        \/         \/                         \/         \/


// GLEW
#define GLEW_STATIC
#include <GL/glew.h>


// GLUT
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>


// Shader Compilation
#include "../resources/shaders/Shader.h"


// CImg Image library
#include "../resources/CImg.h"


// Perlin noise - simple-to-use implementation from https://github.com/sol-prog/Perlin_Noise
#include "../resources/perlin.h"


// glsl-style Vector and Matrix Library - separate includes for different functionality
#include "../resources/glm/glm.hpp" //general types
#include "../resources/glm/gtc/matrix_transform.hpp" //orthographic view matrix (glm::ortho( left, right, bottom, top, zNear, zFar ))
#include "../resources/glm/gtc/type_ptr.hpp" //allows the sending of a matrix (weird workaround for glUniform...)


// this namespace contains, for either platform:
//		ortho() matrix variables
//		vector types, vector arrays for points and colors
//    image dimensions
// 		definition of point size
//		definition of rotation and rotation_direction toggles
//
//

namespace jb_OpenGL_Laptop{

	GLfloat left = -1.366f;
	GLfloat right = 1.366f;
	GLfloat top = -0.768f;
	GLfloat bottom = 0.768f;
	GLfloat zNear = -1.0f;
	GLfloat zFar = 1.0f;

	// don't really need this, it's really just for convenience
	typedef glm::vec4  color4;
	typedef glm::vec4  point4;



	// image dimensions, based on laptop screen resolution
	const int image_height = 768;
	const int image_width = 1366;


	// verticies - THINKPAD
	const int NumVertices = 3000000;

	point4 points[NumVertices];
	color4 colors[NumVertices];




	// Array of rotation angles (in degrees) for each coordinate axis
	enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
	int      Axis = Xaxis;
	GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };

	GLuint  theta;  // The location of the "theta" shader uniform variable
	GLuint  ortho_matrix;

	glm::mat4 Projection;

	bool rotate = true;
	bool rotation_direction = true;


	//initial value of point size
	GLfloat pointsize = 1.0;

}

namespace jb_OpenGL_Radeon{

	GLfloat left = -1.366f;
	GLfloat right = 1.366f;
	GLfloat top = -0.768f;
	GLfloat bottom = 0.768f;
	GLfloat zNear = -1.0f;
	GLfloat zFar = 1.0f;

	// don't really need this, it's really just for convenience
	typedef glm::vec4  color4;
	typedef glm::vec4  point4;



	// image dimensions, based on laptop screen resolution
	const int image_height = 768;
	const int image_width = 1366;


	// verticies - RADEON
	const int NumVertices = 64000000;

	point4 points[NumVertices];
	color4 colors[NumVertices];




	// Array of rotation angles (in degrees) for each coordinate axis
	enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
	int      Axis = Xaxis;
	GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };

	GLuint  theta;  // The location of the "theta" shader uniform variable
	GLuint  ortho_matrix;

	glm::mat4 Projection;

	bool rotate = true;
	bool rotation_direction = true;


	//initial value of point size
	GLfloat pointsize = 1.0;

}


using namespace jb_OpenGL_Laptop;

//using namespace jb_OpenGL_Radeon;


//----------------------------------------------------------------------------

// // quad generates two triangles for each face and assigns colors
// //    to the vertices

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


// .___ _______  .______________
// |   |\      \ |   \__    ___/
// |   |/   |   \|   | |    |
// |   /    |    \   | |    |
// |___\____|__  /___| |____|
//             \/

//----------------------------------------------------------------------------

int Index = 0; //tracks the number of points generated

void generate_points()
{

	PerlinNoise p;

	for( GLfloat x = -0.5; x <= 0.5 ; x += 0.01 )
	{
		for ( GLfloat y = -0.5; y <= 0.5; y += 0.01)
		{
			if( p.noise( 10*x, 10*y, 0.0 ) > 0.5 && p.noise( 10*x, 10*y, 0.0 ) < 0.75)
			{

				// the x and y values here will be used as texture coordinates, taking advantage of wrapping
				points[Index] = point4( x, y, 0.0, 1.0);


				colors[Index] = color4( p.noise( 10*x, 10*y, 0.0 ), 0.7*p.noise( 10*x, 10*y, 0.0 ), 0.3*p.noise( 10*x, 10*y, 0.0 ), p.noise( 10*x, 10*y, 0.0 ));


				cout << Index << endl;

				Index++;

			}
		}
	}






	//done
}



// OpenGL initialization
void init( Shader s )
{

		#define STB_IMAGE_IMPLEMENTATION
		#include "../resources/stb_image.h"

		generate_points();

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW );
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


		cout << sizeof( points );


		// texture handling


		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);



		// uniform value for rotation
    theta = glGetUniformLocation( s.Program, "theta" );
		ortho_matrix = glGetUniformLocation( s.Program, "view" ); //allows the scaling to the screen dimensions`


		Projection = glm::ortho(left, right, top, bottom, zNear, zFar);

		glUniformMatrix4fv( ortho_matrix, 1, GL_FALSE,  glm::value_ptr( Projection ) );

		// enable z buffer for occlusion
    glEnable( GL_DEPTH_TEST );
		glEnable( GL_TEXTURE_3D );


		// alpha blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		// what color background?
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
}



//   ________.____     ____ ______________
//  /  _____/|    |   |    |   \__    ___/
// /   \  ___|    |   |    |   / |    |
// \    \_\  \    |___|    |  /  |    |
//  \______  /_______ \______/   |____|
// 	    	 \/        \/

//----------------------------------------------------------------------------

int frame_timer_index = 0;
bool show_frame_time = false;


const int num_frames_history = 180;
int frame_times[num_frames_history];


void display( void )
{

		tick = Clock::now();



    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// update the value of theta in the shader
    glUniform3fv( theta, 1, Theta );

		// the draw call
    glDrawArrays( GL_POINTS, 0, NumVertices );


    glutSwapBuffers();



		tock = Clock::now();

		if( frame_timer_index = num_frames_history ){ frame_timer_index = 0; }else{ frame_timer_index++; }

		frame_times[frame_timer_index] = ( int ) std::chrono::duration_cast<milliseconds>( tock - tick ).count( );

		if ( show_frame_time )
			std::cout << frame_times[frame_timer_index] << " milliseconds" << endl;


}

//----------------------------------------------------------------------------

void keyboard( unsigned char key, int x, int y )
{
    switch( key )
		{
			// quit
			case 033: // Escape Key
			case 'q': case 'Q':
			    exit( EXIT_SUCCESS );
			    break;

			// toggle rotation
			case 'r':
					//condition ? result_if_true : result_if_false
					rotate = rotate ? false : true;
					break;

			// change direction of rotation
			case 't':
					rotation_direction = rotation_direction ? false : true;
					break;

			// resize points
			case 'a':
					pointsize += 1;
					glPointSize( pointsize );
					break;
		  case 'z':
					pointsize -= 1;
					glPointSize( pointsize );
					break;


			// toggle frame counter
			case 'f':
			//condition ? result_if_true : result_if_false
			show_frame_time = show_frame_time ? false : true;
			break;

		  case 's':
				 left *= 1.1;
				 right *= 1.1;
				 top *= 1.1;
				 bottom *= 1.1;
				 zNear *= 1.1;
				 zFar *= 1.1;

				 Projection = glm::ortho(left, right, top, bottom, zNear, zFar);

		 			glUniformMatrix4fv( ortho_matrix, 1, GL_FALSE,  glm::value_ptr( Projection ) );
				 break;

		  case 'x':
					left *= 0.9;
					right *= 0.9;
					top *= 0.9;
					bottom *= 0.9;
					zNear *= 0.9;
					zFar *= 0.9;

					Projection = glm::ortho(left, right, top, bottom, zNear, zFar);

				 glUniformMatrix4fv( ortho_matrix, 1, GL_FALSE,  glm::value_ptr( Projection ) );
				break;
    }
}

//----------------------------------------------------------------------------

void mouse( int button, int state, int x, int y )
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

void idle( void )
{

		if( rotate ){
			if( rotation_direction ){
				Theta[Axis] += 0.1;

				if ( Theta[Axis] > 360.0 ) {
						Theta[Axis] -= 360.0;
				}
			}else{ //rotate the other way
				Theta[Axis] -= 0.1;

				if ( Theta[Axis] < 0.0 ) {
						Theta[Axis] += 360.0;
				}
			}
		}

    glutPostRedisplay();
}

//----------------------------------------------------------------------------



//    _____      _____  .___ _______
//   /     \    /  _  \ |   |\      \
//  /  \ /  \  /  /_\  \|   |/   |   \
// /    Y    \/    |    \   /    |    \
// \____|__  /\____|__  /___\____|__  /
// 		     \/         \/            \/


int main( int argc, char **argv )
{

	std::cout << "GLUT Initializing...";

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize( image_width, image_height );
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutCreateWindow( "Color Cube" );
	glutFullScreen();

	std::cout << "\rGLUT Initialization Complete." << std::endl;


	glewExperimental = GL_TRUE;
  glewInit();




	std::cout << "Shader Compilation Starting...";

	Shader theShader( "../resources/shaders/vertex_textures.glsl", "../resources/shaders/fragment_textures.glsl" );

	std::cout << "\rShader Compilation Complete.  " << std::endl;




	std::cout << "Generating Geometry";

  init( theShader );

	std::cout << "\rInitialization done." << std::endl;
	std::cout << "generate_points() produced " << Index << " points" << std::endl;



  glutDisplayFunc( display );
  glutKeyboardFunc( keyboard );
  glutMouseFunc( mouse );
  glutIdleFunc( idle );

  glutMainLoop();



	std::cout << "Exiting" << std::endl;

	return 0;
}
