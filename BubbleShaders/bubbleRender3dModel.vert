// All model vertices and normals are passed to this shader.
// It must transform the vertices as appropriate for the view.
// The view and projection matrices are provided, if you need
// the normal matrix, you must construct it in the shader.

uniform float elapsedTime;  //time in seconds since simulation began
uniform float near; //near srendering plane
uniform float far; //far rendering plane
uniform float fov; //field of view
uniform float cursorScrollAmount; //delta scroll amount
uniform vec2 resolution;  //view resolution in pixels
uniform vec3 modelCenter;  //center of the model (might be useful for rotations)
uniform vec3 cameraPos;  //camera position
uniform vec3 lookAtPos;  //point in 3D space the camera is point towards
uniform vec3 cameraUp;  //camera up vector
uniform vec3 lightPos;  //light position
uniform mat4 projMatrix;  //projection matrix
uniform mat4 viewMatrix;  //view matrix (camera)
uniform vec2 cursorAbsolutePos;  //absolute cursor position in pixels
uniform vec2 cursorDragAmount;  //tracks amount of x,y change while the mouse button is down (i.e. amount of drag)
uniform vec2 lastClickPos;  //where the last mouse click happened
uniform vec2 lastFrameDragPos;  //where the cursor was on the previous frame while the mouse button is down
uniform int mouseButtonDown;  //tracks if the mouse button is down

varying vec3 normal;  //normal that will be interpolated for the fragment shader


void main()
{	
	vec4 vertex = projMatrix * viewMatrix * gl_Vertex;  //use the uploaded matrix data
	gl_Position = vertex;  //output the transformed vertex
	normal = gl_Normal;
}
