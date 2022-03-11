#include <stdio.h>
#include <fstream>
#include <math.h>
#include "Angel.h"
#include "MeshViewer.hpp"

int
main( int argc, char **argv )
{
    argCount = argc;
    argArr = argv;

    setObject( *(argv+1) );

    glutInit( &argc, argv );
    #ifdef __APPLE__ 
        glutInitDisplayMode( GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    #else
        glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    #endif
    glutInitWindowSize( 1024, 1024 );
    glutCreateWindow( "Mesh Viewer" );
	glewExperimental = GL_TRUE;
    glewInit();

    init();

    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutMouseFunc( mouse );
    glutMotionFunc( passiveMouse );
    glutSpecialFunc( specialKeyboard );
    glutIdleFunc( idle );
    glEnable(GL_CULL_FACE);

    glutMainLoop();
    return 0;
}

void
setObject( std::string object ) {

    std::ifstream file(object);
    float minX = __FLT_MAX__, minY = __FLT_MAX__, minZ = __FLT_MAX__;
    float maxX = -__FLT_MAX__, maxY = -__FLT_MAX__, maxZ = -__FLT_MAX__;
    
    int none;
    std::string off;
    getline(file, off);

    // Read objects vertice, face and edge count
    file >> verticeCount;
    file >> faceCount;
    file >> edgeCount;

    // Read all vertices and save them
    vertices = new point4[verticeCount];
    for ( int i=0; i<verticeCount; i++ ) {
        file >> vertices[i].x;
        file >> vertices[i].y;
        file >> vertices[i].z;
        vertices[i].w = 1;
        minX = std::min( (vertices+i)->x, minX );
        minY = std::min( (vertices+i)->y, minY );
        minZ = std::min( (vertices+i)->z, minZ );
        maxX = std::max( (vertices+i)->x, maxX );
        maxY = std::max( (vertices+i)->y, maxY );
        maxZ = std::max( (vertices+i)->z, maxZ );
    }

    // Calculate coordinate changes to recenter object.
    float xChange = ( minX + maxX ) / 2;
    float yChange = ( minY + maxY ) / 2;
    float zChange = ( minZ + maxZ ) / 2;

    // Apply changes to repoisition object to center.
    for ( int i=0; i<verticeCount; i++ ) {
        vertices[i].x -= xChange;
        vertices[i].y -= yChange;
        vertices[i].z -= zChange;
    }

    // Find maximum and minimum coordinates to resize object properly.
    float maxOfAll = std::max( std::max( maxX, maxY), maxZ );
    float minOfAll = std::min( std::min( minX, minY), minZ );

    // Set resize to fit object properly to screen.
    Size[0] = Size[1] = Size[2] = 1.5 / ( 
        std::max ( std::max( std::abs( maxX - minX ),
                             std::abs( maxY - minY ) ),
                             std::abs( maxZ - minZ ) ) ); 

    // Set move unit by checking object's coordinates.
    moveUnit = 0.6 / (( maxOfAll - minOfAll ) * Size[0]);

    Rotation[0] = Rotation[1] = Rotation[2] = 0;
    Place[0] = Place[1] = Place[2] = 0;

    int index = 0;
    int one, two, three;
    points = new point4[faceCount*3];
    normals = new vec3[faceCount*3];

    // Read face coordinates and save them.
    for ( int i=0; i<faceCount; i++ ) {
        file >> none;
        file >> one;
        file >> two;
        file >> three;
        points[i*3]   = vertices[one];
        points[i*3+1] = vertices[two];
        points[i*3+2] = vertices[three];
        vec4 u = points[i*3+1] - points[i*3];
        vec4 v = points[i*3+2] - points[i*3+1];
        vec3 normal = normalize( cross(u,v) );
        normals[i*3] = normals[i*3+1] = normals[i*3+2] = normal;
    }

    colors = new point4[faceCount*3];
    srand( (unsigned)time( NULL ) );
    
    // Assign random colors to each vertice.
    for ( int i=0; i<faceCount*3; i++ ) {
        colors[i].x = (float) rand()/RAND_MAX;
        colors[i].y = (float) rand()/RAND_MAX;
        colors[i].z = (float) rand()/RAND_MAX;
        colors[i].w = 1;
    }

    // Set camera rotation and radius parameters to default
    theta = 90.0;
    phi = 0.0;
    radius = 1.3;
}

void
init()
{
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, 96*faceCount + 36*faceCount, NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, 48*faceCount, points );
    glBufferSubData( GL_ARRAY_BUFFER, 48*faceCount, 48*faceCount, colors );
    glBufferSubData( GL_ARRAY_BUFFER, 96*faceCount, 36*faceCount, normals );

    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram( program );

    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET( 0 ) );

    GLuint vColor = glGetAttribLocation( program, "vColor" ); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET( (GLintptr) 48*faceCount ) );

    GLuint vNormal = glGetAttribLocation( program, "vNormal" ); 
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET( (GLintptr) 96*faceCount ) );

    point4 light_position( 0.0, 0.0, 1.3, 0 );
    color4 light_ambient( 0.2, 0.2, 0.2, 1.0 );
    color4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
    color4 light_specular( 1.0, 1.0, 1.0, 1.0 );

    color4 material_ambient( 0.633, 0.727811, 0.633, 1.0);
    color4 material_diffuse( 0.07568, 0.61424, 0.07568, 1.0 );
    color4 material_specular(0.0415, 0.3745, 0.0415 , 1.0 ); 
    float  material_shininess = 76.8;

    color4 ambient_product = light_ambient * material_ambient;
    color4 diffuse_product = light_diffuse * material_diffuse;
    color4 specular_product = light_specular * material_specular;

    glUniform4fv( glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product );
    glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product );
    glUniform4fv( glGetUniformLocation(program, "SpecularProduct"), 1, specular_product );
    glUniform4fv( glGetUniformLocation(program, "LightPosition"), 1, light_position );
    glUniform1f( glGetUniformLocation(program, "Shininess"), material_shininess );

    glShadeModel(GL_SMOOTH);

    rotation = glGetUniformLocation( program, "rotation" );
    place = glGetUniformLocation( program, "place" );
    size = glGetUniformLocation( program, "size" );
    origin = glGetUniformLocation( program, "origin" );
    projection = glGetUniformLocation( program, "projection" );
    model_view = glGetUniformLocation( program, "model_view" );
    shade = glGetUniformLocation( program, "shade" );

    glEnable( GL_DEPTH_TEST );
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

point4  at( 0.0, 0.0, 0.0, 1.0 );
vec4    up( 0.0, 1.0, 0.0, 0.0 );
mat4 perspectiveMatrix = Perspective( 90, 1, 0.00001, 50 );

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glUniform3fv( rotation, 1, Rotation );
    glUniform3fv( place, 1 , Place );
    glUniform3fv( size, 1, Size );
    glUniform3fv( origin, 1, Origin);
    glUniform1i( shade, renderShader);

    point4 eye( radius*cos(phi*DegreesToRadians)*cos(theta*DegreesToRadians), radius*sin(phi*DegreesToRadians),
                radius*cos(phi*DegreesToRadians)*sin(theta*DegreesToRadians), 1.0 );

    mat4  mv = LookAt( eye, at, up );
    glUniformMatrix4fv( model_view, 1, GL_TRUE, mv );
    glUniformMatrix4fv( projection, 1, GL_TRUE, perspectiveMatrix );

    glDrawArrays( GL_TRIANGLES, 0, faceCount*3 );

    glutSwapBuffers();
}

void 
specialKeyboard( int key, int x, int y ) 
{
    switch (key)
    {
    case GLUT_KEY_RIGHT:
        Rotation[1] += 2;        
        if ( Rotation[1] > 360.0 ) 
            Rotation[1] -= 360.0;
        break;
    case GLUT_KEY_LEFT:
        Rotation[1] -= 2;
        if ( Rotation[1] < 0.0 ) 
            Rotation[1] += 360.0;
        break;
    }
}

void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
	    case 033: // Escape Key
	    case 'q': case 'Q':
	        exit( EXIT_SUCCESS );
	        break;
        case 's':
            Size[0] /= 2;
            Size[1] /= 2;
            Size[2] /= 2;
            break;
        case 'S':
            Size[0] *= 2;
            Size[1] *= 2;
            Size[2] *= 2;
            break;
        case 'x':
            Place[0] += moveUnit;
            break;
        case 'X':
            Place[0] -= moveUnit;
            break;
        case 'y':
            Place[1] += moveUnit;
            break;
        case 'Y':
            Place[1] -= moveUnit;
            break;
        case 'z':
            Place[2] += moveUnit;
            break;
        case 'Z':
            Place[2] -= moveUnit;
            break;
        case 'f': case 'F':     // Face mode
            renderShader = false;
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            break;
        case 'e': case 'E':    // Edge mode
            renderShader = false;
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            break;
        case 'v': case 'V':   // Vertice mode
            renderShader = false;
            glPointSize(5);
            glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );
            break;
        case 'm': case 'M':  // Shading mode
            renderShader = true;
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            break;
        case 'n': case 'N':
            delete vertices, points, colors, normals;
            currentObject = (currentObject + 1) % (argCount-1);
            setObject( argArr[ currentObject + 1 ] );
            init();
            break;
        case 'p': case 'P':
            delete vertices, points, colors, normals;
            currentObject = ( argCount + currentObject - 2 ) % (argCount-1);
            setObject( argArr[  currentObject + 1 ] );
            init();
            break;
        
    }
}

// prevX and prevY are previous mouse coordinates to compare with current mouse coordinates
int prevX = -1;
int prevY = -1;
bool canRotate = false;

void
mouse( int button, int state, int x, int y )
{
    if ( state == GLUT_DOWN) {
	    switch( button ) {
	        case GLUT_LEFT_BUTTON:   canRotate = true;  break;
	    }
    }
    if ( state == GLUT_UP) {
	    switch( button ) {
	        case GLUT_LEFT_BUTTON:   prevX = -1; prevY = -1; canRotate = false; break;
	    }
    }
    if  ( (button == 3 && radius - 0.1 > 0 ) || ( button == 4 ) ) {
        if ( button == 3 ) // Mouse wheel down
            radius -= 0.1;
        else               // Mouse wheel up
            radius += 0.1;
    }
}

void
passiveMouse( int x, int y )
{
    if ( canRotate ) {
    
        if ( prevX == -1 && prevY == -1 ) {
            prevX = x;
            prevY = y;
        }
        else {
            phi -= (prevY - y)*mouseRotationSpeed*0.1;
            theta -= (prevX - x)*mouseRotationSpeed*0.1;

            prevX = x;
            prevY = y;

            if ( phi > 90.0 ) 
                phi = 89.99;
            else if ( phi < -90 )
                phi = -89.99;
        }
    }
}

void idle( void ) {
    glutPostRedisplay();
}
