attribute vec4 a_position;
attribute vec4 a_color;
uniform mat4 u_projectionView;
uniform vec3 u_cameraPos;
uniform mat4 u_modelWorld;
varying vec4 v_color;

void main()
{
    mat4 modelWorld = u_modelWorld;
    modelWorld[3].xyz = u_modelWorld[3].xyz - u_cameraPos;

    v_color = a_color;
    gl_Position = u_projectionView * modelWorld * a_position;
}