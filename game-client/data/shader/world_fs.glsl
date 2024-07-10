varying vec4 v_color;
varying float v_logz;

//////////////////////////////////////////////////////////////////////////////
// Wireframe
//////////////////////////////////////////////////////////////////////////////
#ifdef CHANNEL_MIXER
uniform vec3 u_cm_red;
uniform vec3 u_cm_green;
uniform vec3 u_cm_blue;

vec3 channelMix(in vec3 color) {
    vec3 mixed;
    vec3 r = 4.0 * u_cm_red - vec3(2.0,2.0,2.0);
    vec3 g = 4.0 * u_cm_green - vec3(2.0,2.0,2.0);
    vec3 b = 4.0 * u_cm_blue - vec3(2.0,2.0,2.0);
    mixed.x = dot(r, color);
    mixed.y = dot(g, color);
    mixed.z = dot(b, color);
    return mixed;
}
#endif



//////////////////////////////////////////////////////////////////////////////
// Wireframe
//////////////////////////////////////////////////////////////////////////////
#ifdef WIREFRAME
varying vec3 v_barycentric;
float edgeFactor(){
    vec3 d = fwidth(v_barycentric);
    vec3 a3 = smoothstep(vec3(0.0), d * 0.95, v_barycentric);
    return min(min(a3.x, a3.y), a3.z);
}
#endif



//////////////////////////////////////////////////////////////////////////////
// Tonemap
//////////////////////////////////////////////////////////////////////////////
#ifdef TONE_MAP
uniform float u_exposure;
uniform float u_darkness;
vec3 toneMap(in vec3 color) {
    float exposure = u_exposure;
    float darkness = u_darkness;
    float tm = exposure * (exposure / darkness + 1.0) / (exposure + 1.0);
    vec3 luminanceFactors = vec3(0.2126, 0.7152, 0.0722);
    float lum = dot(color.rgb, luminanceFactors);
    return color * (lum * tm);
}
#endif



//////////////////////////////////////////////////////////////////////////////
// Fresnel
//////////////////////////////////////////////////////////////////////////////
#ifdef FRESNEL
varying float v_fresnel;
vec3 fresnel(in vec3 color){
    return mix(color, vec3(1, 1, 1), v_fresnel);
}
#endif



//////////////////////////////////////////////////////////////////////////////
// Vignette
//////////////////////////////////////////////////////////////////////////////
#ifdef VIGNETTE
uniform vec2 u_resolution;
vec3 vignette(in vec3 color) {
    const float innerRadius = 0.3;
    const float outerRadius = 0.65;
    const float intensity = 0.2;

    vec2 pos = gl_FragCoord.xy / u_resolution - .5;
    float v = smoothstep(outerRadius, innerRadius, length(pos));
    return mix(color, color * v, intensity);
}
#endif



//////////////////////////////////////////////////////////////////////////////
// Ground Fog
//////////////////////////////////////////////////////////////////////////////
#ifdef GROUND_FOG
uniform float u_groundFogMin;
uniform float u_groundFogMax;
uniform float u_groundFogHeight;
uniform float u_groundFogStart;
uniform float u_planetRadius;
uniform vec3 u_groundFogColor;
uniform vec3 u_planetPos;
varying vec3 v_position;

float getSurfaceDist() {
    return distance(v_position, u_planetPos) - u_planetRadius;
}

vec3 groundFog(in vec3 color)
{
    float a = 1. / u_groundFogStart;
    float b = 1. / u_groundFogHeight;

    float vertexAlt = max(0.01, getSurfaceDist());
    float distance = max(0.01, length(v_position));
    float standardFog = 1.0 - exp(-a * distance);
    float heightFog = exp(-b * vertexAlt);
    float amount =  clamp(standardFog * heightFog, u_groundFogMin, u_groundFogMax);
    return mix(color, u_groundFogColor, amount);
}
#endif



//////////////////////////////////////////////////////////////////////////////
// Gamma Correction
//////////////////////////////////////////////////////////////////////////////
#ifdef GAMMA_CORRECTION
uniform float u_gammaCorrection;
vec3 gammaCorrect(vec3 color) {
    return pow(color, vec3(1.0/u_gammaCorrection));
}
#endif



//////////////////////////////////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////////////////////////////////
void main() {
    gl_FragDepth = v_logz;
    vec3 color = v_color.rgb;

#ifdef VIGNETTE
    color = vignette(color);
#endif

#ifdef TONE_MAP
    color = toneMap(color);
#endif

#ifdef FRESNEL
    color = fresnel(color);
#endif

#ifdef GROUND_FOG
    color = groundFog(color);
#endif

#ifdef CHANNEL_MIXER
    color = channelMix(color);
#endif

    gl_FragColor.rgb = color.rgb;

#ifdef WIREFRAME
    gl_FragColor.a = v_color.a * 0.95 * (1.0-edgeFactor());
#else
    gl_FragColor.a = v_color.a;
#endif

#ifdef GAMMA_CORRECTION
    gl_FragColor.rgb = gammaCorrect(gl_FragColor.rgb);
#endif
}