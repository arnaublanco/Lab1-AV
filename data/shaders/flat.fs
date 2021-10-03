
uniform vec4 u_color;
uniform sampler2D u_texture;
varying vec2 v_uv;
varying vec3 v_world_position;

// Variables coming from the vertex shader
varying vec3 v_normal;
varying vec3 v_position;

// Uniform variables coming from CPU
uniform vec3 u_camera_position;
uniform vec3 light_pos;

uniform vec3 ambientLight;
uniform vec3 diffuseLight;
uniform vec3 specularLight;

uniform vec3 ambientMaterial;
uniform vec3 diffuseMaterial;
uniform vec3 specularMaterial;
uniform float alpha;

void main()
{
	// 1st term
	vec3 first_term = ambientMaterial*ambientLight;

	// 2nd term
	vec3 lightVector = normalize(light_pos - v_world_position);
	vec3 N = normalize(v_normal);
	float L_dot_N = max(0.0,dot(lightVector,N));
	vec3 second_term = diffuseMaterial*L_dot_N*diffuseLight;

	// 3rd term
	vec3 R = reflect(-lightVector,N);
	vec3 V = normalize(u_camera_position - v_world_position);
	float R_dot_V = max(0.0,dot(R,V));
	float power = pow(R_dot_V,alpha);
	vec3 third_term = specularMaterial*power*specularLight;
	
	gl_FragColor = vec4(second_term + third_term,1.0)*(vec4(first_term,1.0)+texture(u_texture,v_uv));
}
