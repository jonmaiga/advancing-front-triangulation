attribute vec4 a_position;
#ifdef NORMAL_ATTRIBUTE
attribute vec3 a_normal;
#endif

#ifdef COLOR_ATTRIBUTE
attribute vec4 a_color;
#else
uniform vec4 u_materialDiffuse;
#endif

#ifdef INSTANCED
attribute mat4 a_modelWorld;
#else
uniform mat4 u_modelWorld;
#endif
uniform mat4 u_projectionView;

varying vec4 v_color;
varying float v_logz;

//////////////////////////////////////////////////////////////////////////////
// Model world
//////////////////////////////////////////////////////////////////////////////
mat4 getModelWorld() {
#ifdef INSTANCED
    mat4 modelWorld = a_modelWorld;
#else
    mat4 modelWorld = u_modelWorld;
#endif
    // this might be tempting - but we need to do it on cpu in doubles!
    //modelWorld[3].xyz = modelWorld[3].xyz - u_cameraPos;
    return modelWorld;
}

//////////////////////////////////////////////////////////////////////////////
// Ground Fog
//////////////////////////////////////////////////////////////////////////////
#ifdef GROUND_FOG
varying vec3 v_position;
#endif


//////////////////////////////////////////////////////////////////////////////
// Wireframe
//////////////////////////////////////////////////////////////////////////////
#ifdef WIREFRAME
const vec3 c_barycentric[3] = vec3[3](vec3(1.0,0,0), vec3(0,1.0,0), vec3(0,0,1.0));
varying vec3 v_barycentric;
#endif



//////////////////////////////////////////////////////////////////////////////
// Skydome
//////////////////////////////////////////////////////////////////////////////
#ifdef SKYDOME
uniform vec4 u_skyColor1;
uniform vec4 u_skyColor2;
uniform vec4 u_skyColor3;
uniform vec3 u_zenithDir;
vec4 getSkyDomeDiffuse(vec3 vertexPos) {
    vec3 camDir = normalize(vertexPos);
    float zenitiness = 0.5 * (1.0 + dot(u_zenithDir, camDir));
    float skyAlpha = 1.0 - zenitiness;
    if (skyAlpha < 0.5) {
        return mix(u_skyColor1, u_skyColor2, 2.0 * skyAlpha);
    } else {
        return mix(u_skyColor2, u_skyColor3, 2.0 * (skyAlpha-0.5));
    }
}
#endif



//////////////////////////////////////////////////////////////////////////////
// Noise
//////////////////////////////////////////////////////////////////////////////
#ifdef DIFFUSE_NORMAL_NOISE
float hash(float n) { return fract(sin(n)*753.5453123); }
float noise(in vec3 x)
{
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);

    float n = p.x + p.y*157.0 + 113.0*p.z;
    return mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                   mix( hash(n+157.0), hash(n+158.0),f.x),f.y),
               mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                   mix( hash(n+270.0), hash(n+271.0),f.x),f.y),f.z);
}
#endif

float noiseMultiplier(in vec3 x) {
#ifdef DIFFUSE_NORMAL_NOISE
    return 0.85 + 0.15 * noise(20. * x);
#else
    return 1.0;
#endif
}



//////////////////////////////////////////////////////////////////////////////
// Light
//////////////////////////////////////////////////////////////////////////////
#ifdef POINT_LIGHT
#ifdef DIFFUSE_SQUEEZE
uniform float u_squeezeFrom;
#endif
float lightClamp(float dot) {
#ifdef LIGHT_INVERT_NORMAL
    dot = -dot;
#endif
    dot = 0.5 * (dot + 1.0);
    dot *= dot;
    dot = clamp(dot, 0.0, 1.0);
#ifdef DIFFUSE_SQUEEZE
    return mix(u_squeezeFrom, 1, dot);
#else
    return dot;
#endif
}

#ifdef FRESNEL
uniform float u_fresnelScale;
uniform float u_fresnelPow;
varying float v_fresnel;
void fresnel(vec3 vertexPos, vec3 normal)
{
    vec3 camVertexDir = normalize(-vertexPos);
    v_fresnel = u_fresnelScale * pow((1.0 + dot(camVertexDir, normal)), u_fresnelPow);
}
#endif

struct PointLight {
    vec3 pos;
    vec4 diffuse;
};
uniform int u_pointLightCount;
uniform PointLight u_pointLights[2];

vec4 calculateLightColor(vec4 materialDiffuse, vec3 vertexPos, vec3 normal) {
    float noiseMul = noiseMultiplier(normal);
    vec4 color = vec4(0);
    for (int i = 0; i < u_pointLightCount; i++) {
        vec3 lightDir = normalize(vertexPos - u_pointLights[i].pos);
        float diffuseReflection = lightClamp(dot(normal, -lightDir)) * noiseMul;
        color += materialDiffuse * u_pointLights[i].diffuse * diffuseReflection;
    }
    color.a = materialDiffuse.a;
#ifdef FRESNEL
    fresnel(vertexPos, normal);
#endif
    return color;
}
#endif



//////////////////////////////////////////////////////////////////////////////
// Distort
//////////////////////////////////////////////////////////////////////////////
#ifdef DISTORT
uniform float u_distortTime;
uniform float u_distortAmplitude;
#endif

vec4 distort(vec4 vertexPos) {
#ifdef DISTORT
    vec3 offset = vertexPos.xyz + vertexPos.zxy;
    vertexPos.x += u_distortAmplitude * sin(u_distortTime + offset.x);
    vertexPos.y += u_distortAmplitude * sin(u_distortTime + offset.y);
    vertexPos.z += u_distortAmplitude * sin(u_distortTime + offset.z);
    return vertexPos;
#else
    return vertexPos;
#endif
}



//////////////////////////////////////////////////////////////////////////////
// Logarithmic depth buffer
//////////////////////////////////////////////////////////////////////////////
void logarithmicDepthBuffer() {
    const float far = 10000000000.0;
    const float C = 0.01;
    const float FC = 1.0 / log(far * C + 1.0);
    v_logz = log(gl_Position.w * C + 1.0) * FC;
    gl_Position.z = (2.0 * v_logz - 1.0) * gl_Position.w;
}



//////////////////////////////////////////////////////////////////////////////
// Get material diffuse
//////////////////////////////////////////////////////////////////////////////
vec4 materialDiffuse(vec3 vertexPos) {
#ifdef SKYDOME
    return getSkyDomeDiffuse(vertexPos);
#elif defined(COLOR_ATTRIBUTE)
    return a_color;
#else
    return u_materialDiffuse;
#endif
}



//////////////////////////////////////////////////////////////////////////////
// Get color
//////////////////////////////////////////////////////////////////////////////
vec4 getColor(mat4 modelWorld, vec3 vertexPos) {
#ifdef POINT_LIGHT
    vec3 normal = normalize(mat3(modelWorld) * a_normal);
    return calculateLightColor(materialDiffuse(vertexPos), vertexPos, normal);
#else
    return materialDiffuse(vertexPos);
#endif
}

//////////////////////////////////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////////////////////////////////
void main() {
    vec4 position = distort(a_position);
    mat4 modelWorld = getModelWorld();
    vec4 vertexPos = modelWorld * position;

    v_color = getColor(modelWorld, vertexPos.xyz);

#ifdef GROUND_FOG
    v_position = vertexPos.xyz;
#endif

#ifdef WIREFRAME
    int idx = int(mod(gl_VertexID, 3));
    v_barycentric = c_barycentric[idx];
#endif

    gl_Position = u_projectionView * vertexPos;

    logarithmicDepthBuffer();
}