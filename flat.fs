
uniform vec4 u_color;
uniform sampler2D u_texture;
varying vec2 v_uv;

// variables coming from vertex shader
varying vec3 v_wPos;
varying vec3 v_wNormal;

// variables that we need in the main for the phong model
uniform vec3 light_pos;
uniform vec3 Ka;
uniform vec3 u_camera_position;
uniform float Kd;
uniform float alpha;
uniform float Ks;

void main()
{
	gl_FragColor = texture2D(u_texture, v_uv);
	
	// 1st term calculation
	vec3 L_vector = light_pos - v_wPos; // Light vector L
	L_vector = normalize(L_vector); // Normalize vector L
	vec3 first_term = Ka*L_vector; // First term of Phong model

	// 2nd term calculation
	float L_dot_N = max(0.0,dot(L_vector,normalize(v_wNormal));
	vec3 second_term = Kd*L_dot_N;

	// 3rd term calculation
	vec3 R_vector = reflect(L_vector,normalize(v_wNormal));
	vec3 V_vector = normalize(u_camera_position - v_wPos);
	float R_dot_V = max(0.0,dot(R_vector,V_vector));
	float RV_power = pow(R_dot_V,alpha);
	vec3 third_term = Ks*power_term;

	// Calculation of the final term
	vec3 phong = first_term + second_term + third_term;
	//gl_FragColor = vec4(phong, 1.0);
	
}
