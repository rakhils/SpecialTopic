#version 420 core

uniform mat4 PROJECTION;
uniform mat4 VIEW;
uniform mat4 MODEL;

in vec3 POSITION;
in vec4 COLOR;
in vec2 UV;


out vec3 passWorldPosition;
out vec4 passColor; 
out vec2 passUV;

void main( void )
{



vec4 local_pos = vec4( POSITION, 0.0f );	

   vec4 world_pos = MODEL * local_pos ; // assume local is world for now; 
   passWorldPosition = world_pos.xyz;

   vec4 camera_pos = VIEW *local_pos ; 

   vec4 clip_pos = PROJECTION * vec4(camera_pos.xyz, 1); 

   clip_pos.z = clip_pos.w; 
	passUV = UV;
    passColor = COLOR; 
	gl_Position = clip_pos; 


}
