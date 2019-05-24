// basic libraries
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


// CImg Image library
#include "resources/CImg.h"


// glsl-style Vector and Matrix Library - separate includes for different functionality
#include "resources/glm/glm.hpp" //general types
#include "resources/glm/gtc/matrix_transform.hpp" //orthographic view matrix (glm::ortho( left, right, bottom, top, zNear, zFar ))
#include "resources/glm/gtc/type_ptr.hpp" //allows the sending of a matrix (weird workaround for glUniform...)


// Perlin noise - simple implementation from https://github.com/sol-prog/Perlin_Noise
#include "resources/perlin.h"



// don't really need this, it's really just for convenience
typedef glm::vec4  color4;
typedef glm::vec4  point4;



// image dimensions, based on laptop screen resolution
const int image_height = 768;
const int image_width = 1366;


// verticies
const int NumVertices = 7000000;

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

void gencube()
{
	GLfloat num;

	std::vector<GLfloat> range = {-0.5,-0.48,-0.46,-0.44,-0.42,-0.4,-0.38,-0.36,-0.34,-0.32,-0.3,-0.28,-0.26,-0.24,-0.22,-0.2,-0.18,-0.16,-0.14,-0.12,-0.1,-0.08,-0.06,-0.04,-0.02,0,0.02,0.04,0.06,0.08,0.1,0.12,0.14,0.16,0.18,0.2,0.22,0.24,0.26,0.28,0.3,0.32,0.34,0.36,0.38,0.4,0.42,0.44,0.46,0.48,0.5};


	// std::vector<GLfloat> range = {-0.5,-0.49,-0.48,-0.47,-0.46,-0.45,-0.44,-0.43,-0.42,-0.41,-0.4,-0.39,-0.38,-0.37,-0.36,-0.35,-0.34,-0.33,-0.32,-0.31,-0.3,-0.29,-0.28,-0.27,-0.26,-0.25,-0.24,-0.23,-0.22,-0.21,-0.2,-0.19,-0.18,-0.17,-0.16,-0.15,-0.14,-0.13,-0.12,-0.11,-0.1,-0.09,-0.08,-0.07,-0.06,-0.05,-0.04,-0.03,-0.02,-0.01,0.0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.1,0.11,0.12,0.13,0.14,0.15,0.16,0.17,0.18,0.19,0.2,0.21,0.22,0.23,0.24,0.25,0.26,0.27,0.28,0.29,0.3,0.31,0.32,0.33,0.34,0.35,0.36,0.37,0.38,0.39,0.4,0.41,0.42,0.43,0.44,0.45,0.46,0.47,0.48,0.49,0.5};


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


					if( val < 0.3 )
					{
						colors[Index] = color4( p.noise( x, y, z ), 0.0, 1.0, 3.0 );
					}
					else if( val > 0.8 )
					{
						colors[Index] = color4( 1.0, p.noise( x, y, z ), 0.0, 3.0 );
					}
					else
					{
						// colors[Index] = color4( val, val, val, 0.5 - pow( distance_to_zero, 2 ) );
						colors[Index] = color4( val, val, val, 0.5 - distance_to_zero );

					}


				}

				Index++;
			}
		}
	}

	//done
}



// OpenGL initialization
void init( Shader s )
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


		// uniform value for rotation
    theta = glGetUniformLocation( s.Program, "theta" );
		ortho_matrix = glGetUniformLocation( s.Program, "view" ); //allows the scaling to the screen dimensions`


		Projection = glm::ortho(-1.366f, 1.366f, -0.768f, 0.768f, -1.0f, 1.0f);

		glUniformMatrix4fv( ortho_matrix, 1, GL_FALSE,  glm::value_ptr( Projection ) );

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


void screenshot( )
{
	int num_pixels = image_width * image_height;

	// unsigned char buffer, holds framebuffer contents
	unsigned char * r_pixel_buffer = new unsigned char[num_pixels];
	unsigned char * g_pixel_buffer = new unsigned char[num_pixels];
	unsigned char * b_pixel_buffer = new unsigned char[num_pixels];

	// get the image data from OpenGL
	glReadPixels( 0, 0, image_width, image_height, 	 GL_RED, GL_UNSIGNED_BYTE, r_pixel_buffer );
	glReadPixels( 0, 0, image_width, image_height, GL_GREEN, GL_UNSIGNED_BYTE, g_pixel_buffer );
	glReadPixels( 0, 0, image_width, image_height,  GL_BLUE, GL_UNSIGNED_BYTE, b_pixel_buffer );

	// From https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glReadPixels.xhtml :
	// "Pixels are returned in row order from the lowest to the highest row, left to right in each row."

	// create an image
	cimg_library::CImg<unsigned char> img( image_width, image_height, 1, 3, 0 );

	// load pixel data into the images, and draw
	unsigned char image_color[3]; // holds the value of the current pixel's color

	int x = 0;
	int y = image_height;

	for( int index = 0; index < num_pixels; index++ )
	{
		image_color[0] = r_pixel_buffer[index];
		image_color[1] = g_pixel_buffer[index];
		image_color[2] = b_pixel_buffer[index];

		//std::cout << (unsigned int) (image_color[0]) << " " 					// r value
		//					<< (unsigned int) (image_color[1]) << " " 					// g value
		//					<< (unsigned int) (image_color[2]) << " " 					// b value
		//					<< x << " " << y << " " 														// x and y values (output pixel)
		//					<< image_width*image_height - index << std::endl;		// how many entries left?

		img.draw_point( x, y, image_color);

		x++;

		if(x == image_width + 2){
			x = 0;
			y--;

			std::cout << index << std::endl;
		}
	}

	// save the image as screenshot.png, don't worry about overwriting right now
	img.save_png("screenshot.png");


	// done

	delete[] r_pixel_buffer;
	delete[] g_pixel_buffer;
	delete[] b_pixel_buffer;

}


//   ________.____     ____ ______________
//  /  _____/|    |   |    |   \__    ___/
// /   \  ___|    |   |    |   / |    |
// \    \_\  \    |___|    |  /  |    |
//  \______  /_______ \______/   |____|
// 	    	 \/        \/

//----------------------------------------------------------------------------

void display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// update the value of theta in the shader
    glUniform3fv( theta, 1, Theta );

		// the draw call
    glDrawArrays( GL_POINTS, 0, NumVertices );


    glutSwapBuffers();
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

		  // take screnshot
			case ' ':
					screenshot( );
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

	Shader theShader( "resources/shaders/vertex.glsl", "resources/shaders/fragment.glsl" );
	std::cout << "\rShader Compilation Complete.  " << std::endl;

	std::cout << "Generating Geometry";

  init( theShader );

	std::cout << "\rInitialization done." << std::endl;
	std::cout << "gencube() produced " << Index << " points" << std::endl;

  glutDisplayFunc( display );
  glutKeyboardFunc( keyboard );
  glutMouseFunc( mouse );
  glutIdleFunc( idle );

  glutMainLoop();

	std::cout << "Exiting" << std::endl;

	return 0;
}
