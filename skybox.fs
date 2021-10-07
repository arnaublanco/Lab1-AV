
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;
uniform samplerCube u_texture;
uniform vec3 u_camera_position;

void main()
{
	vec3 direction = v_world_position - u_camera_position;
	gl_FragColor = textureCube(u_texture, direction);
}
