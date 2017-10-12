#version 330 core
layout (location = 0) in vec3 vert;
uniform mat4 MVP;
uniform float global_t; //time since simulation start
const float PIx2 = 6.283185307179586476925286766559f;

uniform int arrayLength;
out vec4 vertexColor;
out float colorDensity;

struct Wave {
	vec2 source;
	float start_t; //time since start
	float w; //angular speed
	float l; //wavelength
	float A; //amplitude
	float sdecay; //decay in space
	int is_packet;
	float packet_l; //packet length
	float phase; //phase
	vec2 velocity;
};

layout (std140) uniform data
{
	Wave ona[1000];
};


void main()
{
	float height = 0.0f;
	for(int i = 0; i < arrayLength; i++) {
		float Dt = (global_t - ona[i].start_t);
		float v = (ona[i].w/PIx2)*ona[i].l;
		vec2 sourcePos = ona[i].source + Dt*ona[i].velocity;
		float u = length(ona[i].velocity);
		float originalTime;
		float distanceToOriginal;
		float d = distance(vert.xz, sourcePos);
		if(u != 0){
			float costh = dot(normalize(ona[i].source-sourcePos), normalize(vert.xz-sourcePos));
				originalTime = (sqrt( d*d*(u*u*costh*costh-u*u+v*v) )-d*u*costh+Dt*u*u-Dt*v*v)/(u*u-v*v);
				distanceToOriginal = v*(Dt-originalTime);
		}
		else {
			originalTime = Dt;
			distanceToOriginal = d;
		}
		height += 
		- exp( - ona[i].is_packet * pow(-ona[i].w * originalTime + PIx2 * distanceToOriginal/ona[i].l, 2) / (ona[i].packet_l*ona[i].packet_l) ) * 
		exp(-distanceToOriginal*ona[i].sdecay/ona[i].A) * 
		ona[i].A*sin(-ona[i].w * originalTime + PIx2 * distanceToOriginal/ona[i].l + ona[i].phase);
	}
	colorDensity = (height+1)/2;
	gl_Position = MVP * vec4(vert.x, height + vert.y, vert.z, 1);
}