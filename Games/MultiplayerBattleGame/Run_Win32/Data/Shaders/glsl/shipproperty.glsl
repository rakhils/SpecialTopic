struct shipobject_t 
{
	vec4	tint;
}; 
layout (binding=7, std140) uniform shipPropBlock
{
   shipobject_t  shipObj;
};