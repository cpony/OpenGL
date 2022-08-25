#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;
uniform bool shadows;
uniform float W_LIGHT;

#define NUM_SAMPLES 50
#define EPS 1e-3
#define PI 3.141592653589793
#define PI2 6.283185307179586
#define NUM_RINGS 10

vec3 poissonDisk[NUM_SAMPLES];

highp float rand_1to1(highp float x ) { 
  // -1 -1
  return fract(sin(x)*10000.0);
}

highp float rand_2to1(vec2 uv ) { 
  // 0 - 1
	const highp float a = 12.9898, b = 78.233, c = 43758.5453;
	highp float dt = dot( uv.xy, vec2( a,b ) ), sn = mod( dt, PI );
	return fract(sin(sn) * c);
}

void poissonDiskSamples( const in vec2 randomSeed ) {

  float ANGLE_STEP = PI2 * float( NUM_RINGS ) / float( NUM_SAMPLES );
  float INV_NUM_SAMPLES = 1.0 / float( NUM_SAMPLES );

  float angle = rand_2to1( randomSeed ) * PI2;
  float radius = INV_NUM_SAMPLES;
  float radiusStep = radius;

  for( int i = 0; i < NUM_SAMPLES; i ++ ) {
    poissonDisk[i] = vec3( cos( angle ), sin( angle ) , sin( angle*1.5)) * pow( radius, 0.75 );
    radius += radiusStep;
    angle += ANGLE_STEP;
  }
}

void uniformDiskSamples( const in vec2 randomSeed ) {

  float randNum = rand_2to1(randomSeed);
  float sampleX = rand_1to1( randNum ) ;
  float sampleY = rand_1to1( sampleX ) ;

  float angle = sampleX * PI2;
  float radius = sqrt(sampleY);

  for( int i = 0; i < NUM_SAMPLES; i ++ ) {
    poissonDisk[i] = vec3( radius * cos(angle) , radius * sin(angle) ,radius * sin( angle*1.5) );

    sampleX = rand_1to1( sampleY ) ;
    sampleY = rand_1to1( sampleX ) ;

    angle = sampleX * PI2;
    radius = sqrt(sampleY);
  }
}

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos)//PCSS不用
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    // use the fragment to light vector to sample from the depth map    
    // float closestDepth = texture(depthMap, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    // closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    // float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    // float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
    // PCF
    // float shadow = 0.0;
    // float bias = 0.05; 
    // float samples = 4.0;
    // float offset = 0.1;
    // for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    // {
        // for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        // {
            // for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            // {
                // float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r; // use lightdir to lookup cubemap
                // closestDepth *= far_plane;   // Undo mapping [0;1]
                // if(currentDepth - bias > closestDepth)
                    // shadow += 1.0;
            // }
        // }
    // }
    // shadow /= (samples * samples * samples);
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        //float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        //poissonDisk[i]
        float closestDepth = texture(depthMap, fragToLight + poissonDisk[i] * diskRadius).r;
        //获取立方体贴图的深度，已经将深度储存为fragment和光位置之间的距离了,直接取
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}

float findBlocker(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);

    //float viewDistance = length(viewPos - fragPos);

    int shadowCount = 0;
    float blockDepth = 0.0;
    //float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    //float diskRadius = 1.0 / textureSize(depthMap,0).x * 20;
    float diskRadius = 1.0 / 400 *2* 5;

    float shadow = 0.0;
    float bias = 0.015;
    int samples = 20;


    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        //float closestDepth = texture(depthMap, fragToLight + poissonDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth){
            blockDepth += closestDepth;
            shadow += 1.0;
            }
   }
        

    // display closestDepth as debug (to visualize depth cubemap)
    return float(blockDepth) / float(shadow) ;    


    //return blockDepth/float(shadow);


}

float PCSS(vec3 fragPos){
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);
    float viewDistance = length(viewPos - fragPos);
    float zBlocker = findBlocker(fragPos);
    zBlocker = zBlocker;
    if(zBlocker < EPS) return 1.0;
    if(zBlocker > far_plane) return 0.0;
    //zBlocker/=far_plane;


    float wPenumbra = (currentDepth - zBlocker) * (W_LIGHT+1.0) / zBlocker;

    float shadow = 0.0;
    float bias = 0.015;
    int samples = 20;

    //float diskRadius = (1.0 + (viewDistance / far_plane)) / (25.0*2*wPenumbra);
    //float diskRadius = 1.0 / textureSize(depthMap,0).x * 5 * wPenumbra;
    float diskRadius = 1.0 / 400 * 4*2* wPenumbra;

    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        //float closestDepth = texture(depthMap, fragToLight + poissonDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    return shadow; 
    //return wPenumbra; //重要过程化显示 
}



void main()
{           
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = shadows ? ShadowCalculation(fs_in.FragPos) : 0.0;   //重要过程化显示   
    //float shadow = shadows ? findBlocker(fs_in.FragPos) : 0.0;   //重要过程化显示                    
    //float shadow = shadows ? PCSS(fs_in.FragPos) : 0.0;                      

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    //FragColor = vec4(lighting, 1.0);
    FragColor = vec4(shadow,shadow,shadow ,1.0);//重要过程化显示     
}