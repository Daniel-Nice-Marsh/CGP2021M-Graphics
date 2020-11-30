#version 440 core
layout (location = 0) in vec3 Position; //vertex positions
//layout (location=1) in vec3 vertexColours;  //vertex colours
layout (location=1) in vec2 texCoord;	//tex coords
layout (location=2) in vec3 normal;	// vertex normals

 					
out vec2 textureCoordinate; 
out vec3 normals;
out vec3 fragmentPosition;
out vec3 lightColour;
out vec3 lightPosition;


uniform mat4 uNormalMatrix; // get normal matrix.
uniform mat4 uModel;		// get model.
uniform mat4 uView;		// get view.
uniform mat4 uProjection;	// get projection.
uniform vec3 lightCol;		// get light colour.
uniform vec3 lightPos;		// get light position.


void main()
{
	
	// update model position.
	gl_Position = uProjection * uView * uModel * vec4(Position.x, Position.y, Position.z, 1.0); 
	
	// apply texture to model coordinates.
	textureCoordinate = vec2(texCoord.x, 1 - texCoord.y);
	
	
	// not sure how this works.
	fragmentPosition = vec3(uModel * vec4(Position, 1.0f));
	
	// not sure how this works.
	//calculate a 'normal matrix' and multiply by the unmodified normal
	normals = mat3(uNormalMatrix) * normal;
	
	lightColour = lightCol;
	lightPosition = lightPos;

}