#define PI 3.14159265358979323846

uniform int flag;

//uniform vec4 fvAmbient; //gl_FrontLightProduct[0].ambient
//uniform vec4 fvSpecular; //gl_FrontLightProduct[0].specular
//uniform vec4 fvDiffuse; //gl_FrontLightProduct[0].diffuse

// used for phong
uniform vec3 pAmbientMat;
uniform vec3 specularMat;
uniform vec3 diffuseMat;
uniform float specularPower;

// used for cook-torrence
uniform vec3 cAmbientMat;
uniform vec3 F0;
uniform float d;
uniform float s;
uniform float m1;
uniform float w1;
uniform float m2;
uniform float w2;


varying vec3 V;
varying vec3 L;
varying vec3 N;
varying vec4 color;

void main(void)
{
   // cook torrence lighting with phong shading
   if (flag == 0) {
      vec3  H  = normalize(L + V);
      
      float NdotV = dot(N, V);
      float NdotH = dot(N, H);
      float NdotL = dot(N, L);
      float VdotH = dot(V, H);
      
      vec3 n = (1.0 + sqrt(F0)) / (1.0 - sqrt(F0));
      vec3 F = (1.0 - pow((1.0 - NdotV), 5.0)) + (n * pow((1.0 - NdotV), 5.0));
      
      float alpha = acos(NdotH / (length(N) * length(H)));
      float D1 = w1 * (exp(-pow((tan(alpha)/m1), 2.0)) / (pow(m1, 2.0) * pow(cos(alpha), 4.0)));
      float D2 = w2 * (exp(-pow((tan(alpha)/m2), 2.0)) / (pow(m2, 2.0) * pow(cos(alpha), 4.0)));
      float D = D1 + D2;
   
      float G = min(1.0, min(((2.0 * NdotH * NdotV) / VdotH), ((2.0 * NdotH * NdotL) / VdotH)));
      
      vec3 Rs = (F * D * G) / (PI * NdotL * NdotV);
      vec3 Ra = PI * Rd;
      
      float dwi = 0.0001;
      
      vec4 ret = vec4(gl_FrontLightProduct[0].ambient.xyz * Ra + gl_FrontLightProduct[0].diffuse.xyz * NdotL /* * dwi*/ * (s * Rs + d * Rd), 1.0);
      gl_FragColor = ret;
      
   // Phong lighting with phong shading
   } else if (flag == 1) {
      float NDotL = max(0.0, dot(N, L)); 
   
      //vec3 R = normalize(((2.0 * N) * NDotL) - L);
      vec3 R = normalize(-reflect(L,N)); 
      float RDotV = max(0.0, dot(R, V));
      
      vec3 H = normalize(L + V);
      float NDotH = dot(N, H);
      
      vec3 Ia = pAmbientMat * gl_FrontLightProduct[0].ambient.xyz;
      vec3 Id = diffuseMat * gl_FrontLightProduct[0].diffuse.xyz * NDotL;
      Id = clamp(Id, 0.0, 1.0);
      vec3 Is = specularMat * gl_FrontLightProduct[0].specular.xyz * pow(RDotV, specularPower);
      gl_FragColor = vec4((Ia + Id + Is), 1.0);
      
   // we are using Gourard shading, just use the color given by the vertex shader
   } else {
      gl_FragColor = color;
   }
   
}