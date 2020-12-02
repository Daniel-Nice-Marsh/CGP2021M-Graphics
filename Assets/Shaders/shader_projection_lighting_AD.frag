#version 440 core
out vec4 vertColour;	//output colour of vertex
in vec2 textureCoordinate; //tex coords from vertex shader
in vec3 normals;
in vec3 fragmentPosition;
in vec3 lightColour;
in vec3 lightPosition;

uniform sampler2D uPerlinTex;
uniform sampler2D uLavaTex; //uniform holding texture info from main programme
uniform sampler2D uIceTex;
uniform float uTime;



void main()
{
	//===================================================================================
	// lighting
	//===================================================================================

	//ambient component
	//set the ambient coeff from material
	float lightAmbientStrength = 0.3f;
	vec3 objectAmbientReflectionCoeff = vec3(1.0f, 1.0f, 1.0f);
	vec3 ambient = (lightAmbientStrength * objectAmbientReflectionCoeff) * lightColour;
	
	//diffuse component
	//normalise normal vectors (reset them as unit vectors)
	vec3 nNormal = normalize(normals);

	//calculate the light direction from the light position and the fragment position
    vec3 lightDirection = normalize(lightPosition - fragmentPosition);
	
	//determine the dot product of normal direction and light direction
	float diffuseStrength = max(dot(nNormal, lightDirection), 0.0f);
	
	//combine this with the light colour
	//set the diffuse coeff from material
	vec3 objectDiffuseReflectionCoeff = vec3(1.0f, 1.0f, 1.0f);
    vec3 diffuse = (diffuseStrength * objectDiffuseReflectionCoeff) * lightColour;
	//===================================================================================

	//===================================================================================
	// texture
	//===================================================================================
	
	// store perlin tex.
	vec4 perlin = texture(uPerlinTex, textureCoordinate);

	// sin.
	float wave = smoothstep(-0.7,1.3,(sin(uTime/1000)+perlin.g));

	// doo da
	float barrier = step(0.5, wave);

	// store ice tex.
	vec4 ice = texture(uIceTex, textureCoordinate);

	// store lava tex.
	vec4 lava = texture(uLavaTex, textureCoordinate);

	vec4 effect = (lava*barrier) + (ice*(1.0-barrier));

	vertColour = (vec4((ambient+diffuse),1.0) * effect);
	
/*
	
	tex.r = fragmentPosition.x;
	tex.g = fragmentPosition.y;
	tex.b = fragmentPosition.z;
	tex.a = 0.5 + (ceil(sin(uTime/1000)/2))+floor(ceil(sin(uTime/1000)/2));
	
*/
	


	//apply no lighting, ambient and diffuse components with colour contributed by texture
	//vertColour = (vec4((lightColour), 1.0) * vertColour
	//vertColour = (vec4((ambient),1.0) * vertColour);
	//vertColour = (vec4((ambient+diffuse),1.0) * vertColour);
	//===================================================================================
}