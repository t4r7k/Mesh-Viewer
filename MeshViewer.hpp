#include "Angel.h"

#define mouseRotationSpeed 1 // Can change mouse rotation speed here

// Standart callback functions for controlling the program
void init();
void display( void );
void keyboard( unsigned char key, int x, int y );
void mouse( int button, int state, int x, int y );
void passiveMouse( int x, int y );
void idle( void );
void specialKeyboard( int key, int x, int y );

// Type definitions to make code more readable
typedef Angel::vec4 point4;
typedef Angel::vec4 vec4;
typedef Angel::vec3 vec3;
typedef Angel::vec4 color4;

/* Current displaying object's arrays, object's 
 properties loaded to these and sent to GPU */
vec4 *vertices;
point4 *points;
color4 *colors;
vec3 *normals;

// Specs for currently displayed object
int verticeCount, faceCount, edgeCount;

// Boolean to send shader to render shader or not
bool renderShader = false;

// Current object's number
int currentObject = 0;

// Program arguments
int argCount;
char **argArr;

// Function to set up object to draw
void setObject( std::string object );

// Arrays to make transformations
GLfloat  Rotation[3] = { 0.0, 0.0, 0.0 };  // To rotate object
GLfloat  Place[3] = { 0.0, 0.0, 0.0 };  // To change position of object from origin
GLfloat  Size[3] = { 1, 1, 1 };     // To change object's size.
GLfloat  Origin[3] = {0.0,0.0,0.0}; //To transform matrice to origin

// Locations of shader uniform variables
GLuint rotation; 
GLuint place;
GLuint size;
GLuint origin;

// Matrices
GLuint  projection; // projection matrix
GLuint  model_view;  // model-view matrix 
GLboolean shade;   // shade setting

// Camera properties
float radius = 1.3; // Radius of the camera
float phi = 0;     // Coordinates of
float theta = 90;    // the camera.

// Unit to move given object (changes depending on object's original size)
float moveUnit = 1;