#version 330 core                              
//in vec4 vertexColor;
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

struct Material{
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};


struct LightPoint{
    float constant;
    float linear;
    float quadratic;
     vec3 pos;
     vec3 color;
     vec3 dirToLight;
};

struct LightSpot{
    float constant;
    float linear;
    float quadratic;
     vec3 pos;
     vec3 color;
     vec3 dirToLight;
    float cosPhyInner;
    float cosPhyOuter;

};


struct LightDirectional{
     vec3 pos;
     vec3 color;
     vec3 dirToLight;
};

uniform Material material;
uniform LightDirectional lightD;
uniform LightPoint lightP0;
uniform LightPoint lightP1;
uniform LightPoint lightP2;
uniform LightPoint lightP3;
uniform LightSpot lightS;

//uniform sampler2D ourTexture;
//uniform sampler2D ourFace;
uniform vec3 objColor;
uniform vec3 ambientColor;
uniform vec3 cameraPos;

out vec4 FragColor; 

vec3 CalcLightDirectional(LightDirectional light ,vec3 uNormal,vec3 dirToCamera){

    //diffuse
    float diffIntensity = max(dot(light.dirToLight , uNormal),0);
    vec3 diffColor = diffIntensity * light.color * texture(material.diffuse,TexCoord).rgb;
    //specular
    vec3 R = normalize(reflect(-light.dirToLight,uNormal));
    float specIntensity = pow(max(dot(R,dirToCamera),0),material.shininess);
    vec3 specColor = specIntensity * light.color * texture(material.specular,TexCoord).rgb;

    vec3 result= diffColor + specColor;
    return result;
}

vec3 CalcLightPoint(LightPoint light ,vec3 uNormal,vec3 dirToCamera){
    //attenuation
    float dist = length(light.pos - FragPos);
    float attenuation = 1.0/(light.constant + light.linear * dist + light.quadratic*(dist*dist));

    //diffuse
    float diffIntensity = max(dot(normalize(light.pos - FragPos) , uNormal),0) * attenuation;
    vec3 diffColor = diffIntensity * light.color * texture(material.diffuse,TexCoord).rgb;
    
    //specular
    vec3 R = normalize(reflect(-normalize(light.pos - FragPos),uNormal));
    float specIntensity = pow(max(dot(R,dirToCamera),0),material.shininess) * attenuation;
    vec3 specColor = specIntensity * light.color * texture(material.specular,TexCoord).rgb;

    vec3 result = diffColor + specColor;
    return result; 
}

vec3 CalcLightSpot(LightSpot light ,vec3 uNormal,vec3 dirToCamera){
    //attenuation
    float dist = length(light.pos - FragPos);
    float attenuation = 1.0/(light.constant + light.linear * dist + light.quadratic*(dist*dist));
    
    float cosTheta = dot(normalize(FragPos - light.pos) , -1 * light.dirToLight);
    float spotRatio;
    if(cosTheta > lightS.cosPhyInner){
    spotRatio = 1.0f;
    }
    else if(cosTheta > lightS.cosPhyOuter){
    spotRatio = (cosTheta - lightS.cosPhyOuter)/(lightS.cosPhyInner - lightS.cosPhyOuter);
    }
    else{
    spotRatio = 0;
    }

    //diffuse
    float diffIntensity = max(dot(normalize(light.pos - FragPos) , uNormal),0) * attenuation *spotRatio;
    vec3 diffColor = diffIntensity * light.color * texture(material.diffuse,TexCoord).rgb;
    
    //specular
    vec3 R = normalize(reflect(-normalize(light.pos - FragPos),uNormal));
    float specIntensity = pow(max(dot(R,dirToCamera),0),material.shininess) * attenuation * spotRatio;
    vec3 specColor = specIntensity * light.color * texture(material.specular,TexCoord).rgb;
   
    
    vec3 result = diffColor+specColor;
    // (ambientColor+(diffColor+specColor)*spotRatio)*objColor;
    return result;
}

void main(){       
    vec3 finalResult = vec3(0,0,0);
    vec3 uNormal = normalize(Normal);
    vec3 dirToCamera = normalize(cameraPos - FragPos);
    finalResult += CalcLightDirectional(lightD, uNormal, dirToCamera);
    finalResult += CalcLightPoint(lightP0,uNormal,dirToCamera);
    finalResult += CalcLightPoint(lightP1,uNormal,dirToCamera);
    finalResult += CalcLightPoint(lightP2,uNormal,dirToCamera);
    //finalResult += CalcLightPoint(lightP3,uNormal,dirToCamera);
    finalResult += CalcLightSpot(lightS,uNormal,dirToCamera);
    finalResult += ambientColor;
    finalResult *= objColor;

    FragColor = vec4(finalResult ,1.0);
}


//________
/*    float dist = length(lightPos-FragPos);
    float attenuation = 1.0 / (lightP.constant + lightP.linear * dist + 
                lightP.quadratic * (dist * dist));

    vec3 lightDir =normalize(lightPos-FragPos);
    vec3 reflectVec = reflect(-lightDir,Normal);
    vec3 cameraVec = normalize(cameraPos-FragPos);

    //specular
    float specularAmount = pow(max(dot(reflectVec,cameraVec),0),material.shininess);
    vec3 specular = texture(material.specular , TexCoord).rgb*specularAmount * lightColor;

    //diffuse
    vec3 diffuse = texture(material.diffuse , TexCoord).rgb* max(dot(lightDir,Normal),0)*lightColor;
    //vec3 diffuse = texture(material.diffuse, TexCoord).rgb ;

    //ambient
    vec3 ambient = texture(material.diffuse , TexCoord).rgb* ambientColor;

    //FragColor = vertexColor;   
    //FragColor = mix(texture(ourTexture,TexCoord) , texture(ourFace,TexCoord),0.2);   
    //FragColor =vec4(objColor*ambientColor,1.0)*texture(ourTexture,TexCoord) * texture(ourFace,TexCoord);
    //FragColor =vec4(objColor*ambientColor,1.0);

    float cosTheta = dot(normalize(FragPos - lightPos) , -1 * lightDirUniform);
//    if(cosTheta > lightS.cosPhy){
//    
//    FragColor =vec4((ambient+(diffuse+specular))*objColor,1.0);
//
//    }
//    else{
//    FragColor =vec4((ambient)*objColor,1.0);
//    
//    }
    float spotRatio;
    if(cosTheta > lightS.cosPhyInner){
    spotRatio = 1.0f;
    }
    else if(cosTheta > lightS.cosPhyOuter){
    spotRatio = (cosTheta - lightS.cosPhyOuter)/(lightS.cosPhyInner - lightS.cosPhyOuter);
    }
    else{
    spotRatio = 0;
    }
    
    FragColor =vec4((ambient+(diffuse+specular)*spotRatio)*objColor,1.0);
    */