/*
 * SDL OpenGL Tutorial.
 * (c) Michael Vance, 2000
 * briareos@lokigames.com
 *
 * Distributed under terms of the LGPL. 
 */

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include "engine/shaderprogram.h"

#include <stdio.h>
#include <stdlib.h>
//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

ShaderProgram* shader;

//Starts up SDL, creates window, and initializes OpenGL
bool init();

//Initializes rendering program and clear color
bool initGL();
void initShaders();

//Input handler
void handleKeys( unsigned char key, int x, int y );

//Per frame update
void update();

//Renders quad to the screen
void render();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//OpenGL context
SDL_GLContext gContext;

//Render flag
bool gRenderQuad = true;

//Graphics program
GLuint gProgramID = 0;
GLint gVertexPos2DLocation = -1;
GLint timeLoc=-1;
GLint scWidthLoc=-1;
GLint scHeightLoc=-1;

GLuint VertexArrayID;
GLuint gVBO = 0;
GLuint gIBO = 0;

float time=0.f;

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Use OpenGL 3.3 core
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create context
			gContext = SDL_GL_CreateContext( gWindow );
			if( gContext == NULL )
			{
				printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize GLEW
				glewExperimental = GL_TRUE; 
				GLenum glewError = glewInit();
				if( glewError != GLEW_OK )
				{
					printf( "Error initializing GLEW! %s\n", glewGetErrorString( glewError ) );
				}

				//Use Vsync
				if( SDL_GL_SetSwapInterval( 1 ) < 0 )
				{
					printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
				}

			}
		}
	}

	return success;
}

void initShaders(char* frag)
{
    if(frag==NULL)
        shader = new ShaderProgram("shaders/basic_vertex.vshader",NULL,"shaders/basic_fragment.fshader");
    else
        shader = new ShaderProgram("shaders/basic_vertex.vshader",NULL,frag);
    
    gVertexPos2DLocation = shader->getAttribLocation("LVertexPos2D");
    timeLoc = shader->getUniformLocation("time");
    scWidthLoc = shader->getUniformLocation("screenWidth");
    scHeightLoc = shader->getUniformLocation("screenHeight");
    
    glViewport(0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    //Initialize clear color
	glClearColor( 0.f, 0.f, 0.f, 1.f );
    
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
	//VBO data
	GLfloat vertexData[] =
	{
		-0.5f, -1.f,
		0.5f, -1.f,
		-0.5f, 1.f,
		-0.5f, 1.f,
		0.5f, -1.f,
		0.5f, 1.f, 
	};

	//Create VBO
	glGenBuffers( 1, &gVBO );
	glBindBuffer( GL_ARRAY_BUFFER, gVBO );
	glBufferData( GL_ARRAY_BUFFER, 2 * 6 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW );
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,NULL);
	glBindVertexArray(0);
}

void handleKeys( unsigned char key, int x, int y )
{
	//Toggle quad
	if( key == 'q' )
	{
		gRenderQuad = !gRenderQuad;
	}
}

void update()
{
    time = SDL_GetTicks()/1000.f;
}

void render()
{
	//Clear color buffer
	glClear( GL_COLOR_BUFFER_BIT );
	
	//Render quad
	if( gRenderQuad )
	{
		//Bind program
		//glUseProgram( gProgramID );
        shader->use();

		//Set vertex data
		glUniform1f(timeLoc, time); 
		glUniform1f(scWidthLoc, SCREEN_WIDTH);
		glUniform1f(scHeightLoc, SCREEN_HEIGHT);
		glBindVertexArray(VertexArrayID);
		glDrawArrays( GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);

		//Unbind program
		glUseProgram( NULL );
	}
}

void close()
{
	//Deallocate program
	delete shader;
	glDeleteVertexArrays(1, &VertexArrayID);
	//glDeleteProgram( gProgramID );

	//Destroy window	
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}


int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Main loop flag
		bool quit = false;
		if(argc==2)
		    initShaders(args[1]);
		else
		    initShaders(NULL);

		//Event handler
		SDL_Event e;
		
		//Enable text input
		SDL_StartTextInput();

		//While application is running
		while( !quit )
		{
			//Handle events on queue
			while( SDL_PollEvent( &e ) != 0 )
			{
				//User requests quit
				if( e.type == SDL_QUIT )
				{
					quit = true;
				}
				//Handle keypress with current mouse position
				else if( e.type == SDL_TEXTINPUT )
				{
					int x = 0, y = 0;
					SDL_GetMouseState( &x, &y );
					handleKeys( e.text.text[ 0 ], x, y );
				}
			}
            update();

			render();
			
			//Update screen
			SDL_GL_SwapWindow( gWindow );
		}
		
		//Disable text input
		SDL_StopTextInput();
	}

	//Free resources and close SDL
	close();

	return 0;
}
