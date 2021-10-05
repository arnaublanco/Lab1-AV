
varying vec3 v_world_position;
varying vec3 v_normal;


uniform vec3 u_camera_pos;
uniform samplerCube u_texture;

void main()
{
	vec3 v = normalize(u_camera_position-v_world_position);
	vec3 reflected = normalize(reflect(-v,v_normal));

	gl_FragColor  = texture(u_texture,reflected);
}
