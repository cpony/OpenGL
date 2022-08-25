#version 330 core

#define PI 3.1415926535
#define RAYCAST_MAX 100000.0
#define MAT_LAMBERTIAN 0
#define MAT_METALLIC 1
#define MAT_DIELECTRIC 2

in vec2 screenCoord;

out vec4 FragColor;

uniform vec2 screenSize;

//随机数
uint m_u = uint(521288629);
uint m_v = uint(362436069);

uint GetUintCore(inout uint u, inout uint v){
	v = uint(36969) * (v & uint(65535)) + (v >> 16);
	u = uint(18000) * (u & uint(65535)) + (u >> 16);
	return (v << 16) + u;
}

float GetUniformCore(inout uint u, inout uint v){
	uint z = GetUintCore(u, v);
	
	return float(z) / uint(4294967295);
}

float GetUniform(){
	return GetUniformCore(m_u, m_v);
}

uint GetUint(){
	return GetUintCore(m_u, m_v);
}

float rand(){
	return GetUniform();
}

vec2 rand2(){
	return vec2(rand(), rand());
}

vec3 rand3(){
	return vec3(rand(), rand(), rand());
}

vec4 rand4(){
	return vec4(rand(), rand(), rand(), rand());
}

vec3 random_in_unit_sphere(){
	vec3 p;
	
	float theta = rand() * 2.0 * PI;
	float phi = rand() * PI;
	p.y = cos(phi);
	p.x = sin(phi) * cos(theta);
	p.z = sin(phi) * sin(theta);
	
	return p;
}

//材质
struct Lambertian{
	vec3 albedo;
};

Lambertian CreateLambertian(vec3 albedo){
	Lambertian lambertian;

	lambertian.albedo = albedo;

	return lambertian;
}

struct Metallic{
	vec3 albedo;
	float fuzz;
};

Metallic CreateMetallic(vec3 albedo, float fuzz){
	Metallic metallic;

	metallic.albedo = albedo;
	metallic.fuzz = fuzz;

	return metallic;
}

struct Dielectric{
	vec3 albedo;
	float ior;
};

Dielectric CreateDielectric(vec3 albedo, float ior){
	Dielectric dielectric;

	dielectric.albedo = albedo;
	dielectric.ior = ior;

	return dielectric;
}

Lambertian lambertMaterials[4];
Metallic metallicMaterials[4];
Dielectric dielectricMaterials[4];

//光线
struct Ray{
    vec3 origin;
    vec3 direction;
};

Ray CreateRay(vec3 o, vec3 d){
    Ray ray;
    ray.origin = o;
    ray.direction = d;
    
    return ray;
}

vec3 GetRayLocation(Ray ray, float t){
    return ray.origin + t * ray.direction;
}

//摄像机
struct Camera{
    vec3 lower_left_corner;
    vec3 horizontal;
	vec3 vertical;
	vec3 origin;
    vec3 u;
	vec3 v;
	vec3 w;
	float lens_radius;
}; 

Camera camera;

Camera CameraConstructor(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist){
	Camera camera;

	camera.origin = lookfrom;
    camera.lens_radius = aperture / 2;

    float theta = radians(vfov);
    float half_height = tan(theta / 2);
    float half_width = aspect * half_height;

    camera.w = normalize(lookfrom - lookat);
    camera.u = normalize(cross(vup, camera.w));
    camera.v = cross(camera.w, camera.u);
    camera.lower_left_corner = camera.origin
                              - half_width * focus_dist * camera.u
                              - half_height * focus_dist * camera.v
                              - focus_dist * camera.w;

    camera.horizontal = 2 * half_width * focus_dist * camera.u;
    camera.vertical = 2 * half_height * focus_dist * camera.v;

	return camera;
}

Ray CameraGetRay(Camera camera, vec2 uv){
	Ray ray = CreateRay(camera.origin, 
		camera.lower_left_corner + 
		uv.x * camera.horizontal + 
		uv.y * camera.vertical - camera.origin);

	return ray;
}

//球体
struct Sphere{
    vec3 center;
    float radius;
	int materialPtr;//材质数组的下标
	int materialType;//记录材质的类型
}; 

Sphere CreateSphere(vec3 center, float radius, int type, int ptr){
	Sphere sphere;

	sphere.center = center;
	sphere.radius = radius;
	sphere.materialPtr = ptr;
	sphere.materialType = type;

	return sphere;
}

//记录世界中的物体
struct World{
    int objectCount;
	Sphere objects[100];
};

World CreateWorld(){
	World world;

	world.objectCount = 20;

	world.objects[0] = CreateSphere(vec3(0, -1000, 0), 1000, MAT_LAMBERTIAN, 0);
	world.objects[1] = CreateSphere(vec3(0, 1, 0), 1.0, MAT_DIELECTRIC, 0);
	world.objects[2] = CreateSphere(vec3(-4, 1, 0), 1.0, MAT_LAMBERTIAN, 1);
	world.objects[3] = CreateSphere(vec3(4, 1, 0), 1.0, MAT_METALLIC, 0);

	int i = 4;
	float a, b;
	for (a = -4; a < 4; a++) {
		for (b = -3; b < 3; b++) {
			vec3 center;
			if(i % 2 == 0){
			    center = vec3(a + 0.9 * rand(), 0.2, b * 1.5 + 0.9 * rand());
			}
			else{
			    center = vec3(a * -1.5 + 0.9 * rand(), 0.2, b * 1.5 + 0.9 * rand());
			}
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				if (i < 5) {
					// glass
					world.objects[i] = CreateSphere(center, 0.2, MAT_DIELECTRIC, i % 4);
					i++;
				}
			else if (i < 11) {
					// metal
					world.objects[i] = CreateSphere(center, 0.2, MAT_METALLIC, i % 4);
					i++;
				}
				else {
				    // diffuse
					world.objects[i] = CreateSphere(center, 0.2, MAT_LAMBERTIAN, i % 4);
					i++;
				}
			}
		}
	}

	return world;
}

void InitScene(){
	World world = CreateWorld();
	float aspect_ratio = screenSize.x / screenSize.y;
	vec3 lookfrom = vec3(13, 2, 3);
    vec3 lookat = vec3(0, 0, 0);
    vec3 vup = vec3(0, 1, 0);
    float dist_to_focus = 10.0;
    float aperture = 0.1;

    camera = CameraConstructor(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    lambertMaterials[0] = CreateLambertian(vec3(0.5, 0.5, 0.5));
	lambertMaterials[1] = CreateLambertian(vec3(0.7, 0.5, 0.2));
	lambertMaterials[2] = CreateLambertian(vec3(0.8, 0.3, 0.4));
	lambertMaterials[3] = CreateLambertian(vec3(0.0, 0.7, 0.7));

	metallicMaterials[0] = CreateMetallic(vec3(0.7, 0.6, 0.5), 0.0);
	metallicMaterials[1] = CreateMetallic(vec3(0.5, 0.7, 0.5), 0.1);
	metallicMaterials[2] = CreateMetallic(vec3(0.5, 0.5, 0.7), 0.2);
	metallicMaterials[3] = CreateMetallic(vec3(0.7, 0.7, 0.7), 0.3);

	dielectricMaterials[0] = CreateDielectric(vec3(1.0, 1.0, 1.0), 1.5);
	dielectricMaterials[1] = CreateDielectric(vec3(1.0, 1.0, 1.0), 1.5);
	dielectricMaterials[2] = CreateDielectric(vec3(1.0, 1.0, 1.0), 1.5);
	dielectricMaterials[3] = CreateDielectric(vec3(1.0, 1.0, 1.0), 1.5);
}

//记录击中点信息
struct HitRecord{
	float t;
	vec3 position;
	vec3 normal;
	int materialPtr;//材质数组的下标
	int materialType;//记录材质的类型
};

vec3 reflect(in vec3 v, in vec3 n){
	return v - 2 * dot(n, v) * n;
}

bool refract(vec3 v, vec3 n, float ni_over_nt, out vec3 refracted){
	vec3 uv = normalize(v);
	float dt = dot(uv, n);
	float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1.0 - dt * dt);
	if (discriminant > 0.0){
		refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
		return true;
	}

	return false;
}

float schlick(float cosine, float ior){
	float r0 = (1 - ior) / (1 + ior);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}

void LambertianScatter(in Lambertian lambertian, in Ray incident, in HitRecord hitRecord, out Ray scattered, out vec3 attenuation){
	attenuation = lambertian.albedo;

	scattered.origin = hitRecord.position;
	scattered.direction = hitRecord.normal + random_in_unit_sphere();
}

void MetallicScatter(in Metallic metallic, in Ray incident, in HitRecord hitRecord, out Ray scattered, out vec3 attenuation){
	attenuation = metallic.albedo;

	scattered.origin = hitRecord.position;
	scattered.direction = reflect(incident.direction, hitRecord.normal) + metallicMaterials[hitRecord.materialPtr].fuzz * random_in_unit_sphere();
}

void DielectricScatter(in Dielectric dielectric, in Ray incident, in HitRecord hitRecord, out Ray scattered, out vec3 attenuation){
	attenuation = dielectric.albedo;
	vec3 reflected = reflect(incident.direction, hitRecord.normal);

	vec3 outward_normal;
	float ni_over_nt;
	float cosine;
	if(dot(incident.direction, hitRecord.normal) > 0.0){//从内击中
		outward_normal = -hitRecord.normal;
		ni_over_nt = dielectric.ior;
		cosine = dot(incident.direction, hitRecord.normal) / length(incident.direction);//入射光线角度
	}
	else{//从外击中
		outward_normal = hitRecord.normal;
		ni_over_nt = 1.0 / dielectric.ior;
		cosine = -dot(incident.direction, hitRecord.normal) / length(incident.direction);//入射光线角度
	}

	float reflect_prob;
	vec3 refracted;
	if(refract(incident.direction, outward_normal, ni_over_nt, refracted)){
		reflect_prob = schlick(cosine, dielectric.ior);
	}
	else{
		reflect_prob = 1.0;
	}

	if(rand() < reflect_prob){
		scattered = Ray(hitRecord.position, refracted);
	}
	else{
		scattered = Ray(hitRecord.position, refracted);
	}
}


bool HitSphere(Sphere sphere, Ray ray, float t_min, float t_max, inout HitRecord hitRecord){
	vec3 oc = ray.origin - sphere.center;
	
	float a = dot(ray.direction, ray.direction);
	float b = 2.0 * dot(oc, ray.direction);
	float c = dot(oc, oc) - sphere.radius * sphere.radius;

	float delta = b * b - 4 * a * c;

	if(delta > 0.0){
		float temp = (-b - sqrt(delta)) / (2.0 * a);
		if(temp < t_max && temp> t_min){
			hitRecord.t = temp;
			hitRecord.position = GetRayLocation(ray, hitRecord.t);
			hitRecord.normal = (hitRecord.position - sphere.center) / sphere.radius;

			hitRecord.materialPtr = sphere.materialPtr;
			hitRecord.materialType = sphere.materialType;
			
			return true;
		}

		temp = (-b + sqrt(delta)) / (2.0 * a);
		if(temp < t_max && temp> t_min){
			hitRecord.t = temp;
			hitRecord.position = GetRayLocation(ray, hitRecord.t);
			hitRecord.normal = (hitRecord.position - sphere.center) / sphere.radius;

			hitRecord.materialPtr = sphere.materialPtr;
			hitRecord.materialType = sphere.materialType;

			return true;
		}
	}
	
	return false;
}

bool HitWorld(World world, Ray ray, float t_min, float t_max, inout HitRecord rec){
    HitRecord tempRec;
	bool hitanything = false;
	float cloestFar = t_max;//记录最近的击中点的位置

	//最终会获得这条光线击中的最近物体的信息
	for(int i = 0;i < world.objectCount; i++){
	    //如果击中物体，则记录击中点的信息，当击中点比之前的击中点更近时更新信息，更远则不做记录
	    if(HitSphere(world.objects[i], ray, t_min, cloestFar, tempRec)){
		    rec = tempRec;
			hitanything = true;
			cloestFar = rec.t;//更新最近的击中点
		}
	}

	return hitanything;
}

vec3 GetEnvironemntColor(Ray ray){
	vec3 normalizeDir = normalize(ray.direction);
	float t = 0.5 * (normalizeDir.y + 1.0);
	return vec3(1.0, 1.0, 1.0) * (1.0 - t) + vec3(0.5, 0.7, 1.0) * t;

}

vec3 RayTrace(Ray ray, int depth){
    World world = CreateWorld();

    HitRecord hitRecord;

	vec3 bgColor = vec3(0);//背景颜色
	vec3 objColor = vec3(1.0);//用于累成，记录光线击中的所有物体的叠加颜色

	//散射次数越多，光越暗，在物体与物体的接触面处容易发生多次散射，形成阴影
	//光线散射次数
	while(depth > 0){
	    depth--;
		
		//判断是否击中世界中的物体，如果击中，获得最近的那个物体的信息
		if(HitWorld(world, ray, 0.001, 100000.0, hitRecord)){
		    vec3 attenuation;//衰减值，也是物体的颜色
			Ray scatterRay;//散射光线

			//计算出散射光线方向以及获得该物体的颜色
			if(hitRecord.materialType == MAT_LAMBERTIAN){
			    LambertianScatter(lambertMaterials[hitRecord.materialPtr], ray, hitRecord, scatterRay, attenuation);
			}
			else if(hitRecord.materialType == MAT_METALLIC){
			    MetallicScatter(metallicMaterials[hitRecord.materialPtr], ray, hitRecord, scatterRay, attenuation);
			}
			else if(hitRecord.materialType == MAT_DIELECTRIC){
			    DielectricScatter(dielectricMaterials[hitRecord.materialPtr], ray, hitRecord, scatterRay, attenuation);
			}

			ray = scatterRay;//将散射光线作为新的光线
			objColor *= attenuation;//光源衰减，相当于是各个物体颜色的累乘
		}
		//如果没有击中任何物体则退出循环，并获得此时的环境光颜色
		else{
		    bgColor = GetEnvironemntColor(ray);
			break;
		}
	}

	//背景颜色相当于是光线，返回物体颜色与光源颜色的乘积
    return objColor * bgColor;
}

vec3 GammaCorrection(vec3 c){
	return pow(c, vec3(1.0 / 2.2));
}

void main(){
    InitScene();

    vec3 color = vec3(0.0, 0.0, 0.0);
	int sampleCount = 100;
	for(int i = 0; i < sampleCount; i++){
		Ray ray = CameraGetRay(camera, screenCoord + rand2() / screenSize);
		color += RayTrace(ray, 50);
	}
	color /= sampleCount;

	color = GammaCorrection(color);

	FragColor = vec4(color, 1.0);
}