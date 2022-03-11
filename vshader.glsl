#version 150

in  vec4 vPosition;
in  vec4 vColor;
in  vec3 vNormal;
out vec4 color;

uniform vec3 rotation;
uniform vec3 place;
uniform vec3 size;
uniform mat4 projection;
uniform mat4 model_view;
uniform bool shade;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 LightPosition;
uniform float Shininess;

void main() 
{
    vec3 angles = radians( rotation );
    vec3 c = cos( angles );
    vec3 s = sin( angles );

    mat4 ry = mat4( c.y, 0.0, -s.y, 0.0,
		    0.0, 1.0,  0.0, 0.0,
		    s.y, 0.0,  c.y, 0.0,
		    0.0, 0.0,  0.0, 1.0 );

    mat4 placeMatrix = mat4( 1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            place.x, place.y, place.z, 1.0 );

            
    mat4 sizeMatrix = mat4( size.x, 0.0, 0.0, 0.0,
            0.0, size.y, 0.0, 0.0,
            0.0, 0.0, size.z, 0.0,
            0.0, 0.0, 0.0, 1.0 );
    
    mat4 viewMatrix = model_view * placeMatrix * ry * sizeMatrix;

    if ( shade ) {
        vec3 pos = (viewMatrix * vPosition).xyz;
	
        vec3 L = normalize( (LightPosition).xyz - pos );
        vec3 E = normalize( -pos );
        vec3 H = normalize( L + E );

        vec3 N = normalize( viewMatrix *vec4(vNormal, 0.0) ).xyz;

        vec4 ambient = AmbientProduct;

        float Kd = max( dot(L, N), 0.0 );
        vec4  diffuse = Kd*DiffuseProduct;

        float Ks = pow( max(dot(N, H), 0.0), Shininess );
        vec4  specular = Ks * SpecularProduct;
    
        if( dot(L, N) < 0.0 )
	        specular = vec4(0.0, 0.0, 0.0, 1.0);

        color = ambient + diffuse + specular;
        color.a = 1.0;
    }
    else
        color = vColor;

    gl_Position =  projection * viewMatrix * vPosition;
} 
