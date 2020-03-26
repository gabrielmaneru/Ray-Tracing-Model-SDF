#version 330 core
in vec2 attr_vtx;
out vec2 vtx;

void main()
{
	vtx = attr_vtx;

	gl_Position =vec4(attr_vtx, 0.0, 1.0);
}