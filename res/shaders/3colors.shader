#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
uniform float hOffset;
out vec3 ourColor;
out vec4 position;
void main()
{

    gl_Position = vec4(aPos.x + hOffset, aPos.y, aPos.z, 1.0);
    ourColor = aColor;
    position = vec4(aPos.x + hOffset, aPos.y, aPos.z, 1.0);
}
#shader fragment
#version 330 core
in vec3 ourColor;
in vec4 position;
out vec4 FragColor;
uniform float greenColor;
void main() {
   // FragColor = vec4(ourColor.x,greenColor, ourColor.z , 1.0f);
    FragColor = position;
}
