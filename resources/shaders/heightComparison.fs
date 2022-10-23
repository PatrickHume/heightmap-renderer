// A shader used in the tessellation levels evaluation.
// Returns the per-pixel difference between the heightmap
// and the tessellated mesh height. 

#version 410 core
// Pass in the heightmap texture.
uniform sampler2D heightMap;
// The texture coordinate and the mesh height to compare.
in vec2 fragTexCoord;
in float height;

// Output the fragColor.
layout (location = 0) out float FragColor;

void main()
{
   // Output the difference between the heightmap and the tessellated mesh height. 
   FragColor = abs((texture(heightMap, fragTexCoord).r - height)*255.0);
}
