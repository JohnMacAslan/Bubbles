// Computes fragment colors for the model triangles.

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

void main()
{	
	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
