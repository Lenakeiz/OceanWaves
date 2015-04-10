//////////////////////////////////////////////////////////////////////////////////////////
//
// Default vertex shader for materials. Extend this to deal with bump mapping, defered rendering, shadows etc.
//

const float PI = 3.14159265358979323846264;

//parameters
uniform float time;
uniform int n_waves;

//gerstner parameters
uniform float[8] wavelength;
uniform float[8] amplitude;
uniform float[8] speed;
uniform float[8] dir_x;
uniform float[8] dir_z;
uniform float[8] steepness;

// matrices
uniform mat4 modelToProjection;
uniform mat4 modelToCamera;

// attributes from vertex buffer
attribute vec4 pos;
attribute vec2 uv;
attribute vec3 normal;
attribute vec4 color;

// outputs
varying vec3 normal_;
varying vec2 uv_;
varying vec4 color_;
varying vec3 model_pos_;
varying vec3 camera_pos_;

vec3 calcutateGerstnerPosition()
{
	vec3 contribute = vec3(0.0,0.0,0.0);
	float frequency;
	float phase;
	float displacement;
	
	for(int i=0; i < n_waves; ++i)
	{
		frequency = 2 * PI / wavelength[i];
	    phase = speed[i] * frequency;
		//vec2 dir = normalize(vec2(dir_x[i],dir_z[i]));
		//vec2 position = vec2(pos.x,pos.x)
		//displacement = dot(dir,pos.xyz);
		displacement = dot(vec3(dir_x[i],0.0,dir_z[i]),pos.xyz);
		contribute += vec3(
							steepness[i] * amplitude[i] * dir_x[i] * cos(displacement * frequency + phase * time),
							amplitude[i] * sin(displacement * frequency + phase * time),
							steepness[i] * amplitude[i] * dir_z[i] * cos(displacement * frequency + phase * time)
							);
	}
	return contribute;
}

vec3 calculateGersterNormal(vec3 finalPos)
{
	vec3 normal = vec3(0.0,1.0,0.0);
	float frequency;
	float phase;
	float displacement;

	for(int i=0; i < n_waves; ++i)
	{
		frequency = 2.0 * PI / wavelength[i];
	    phase = speed[i] * frequency;
		displacement = dot(vec3(dir_x[i],0.0,dir_z[i]),finalPos);
		normal += vec3(
							- dir_x[i] * amplitude[i] * frequency * cos(displacement * frequency + phase * time),
							- steepness[i] * amplitude[i] * frequency * sin(displacement * frequency + phase * time),
							- dir_z[i] * amplitude[i] * frequency * cos(displacement * frequency + phase * time)
							);
	}
	return normal;
}

void main() {
  vec3 gerstnercontribute = calcutateGerstnerPosition();
  vec3 finalPosition = vec3(pos.x + gerstnercontribute.x, gerstnercontribute.y, pos.z + gerstnercontribute.z);
  gl_Position = modelToProjection * vec4(finalPosition, pos.w);
  vec3 tnormal = (modelToCamera * vec4(calculateGersterNormal(finalPosition), 0.0)).xyz;
  //vec3 tnormal = (modelToCamera * vec4(normal_calculation(), 0.0)).xyz;
  vec3 tpos = (modelToCamera * vec4(finalPosition, pos.w)).xyz;
  normal_ = tnormal;
  uv_ = uv;
  color_ = color;
  camera_pos_ = tpos;
  model_pos_ = finalPosition;
}

