// vec3 LEFT_NORMAL = vec3(-1, 0, 0);
// vec3 FRONT_NORMAL = vec3(0, 0, -1);
// vec3 RIGHT_NORMAL = vec3(1, 0, 0);
// vec3 BACK_NORMAL = vec3(0, 0, 1);
// vec3 TOP_NORMAL = vec3(0, 1, 0);
// vec3 BOTTOM_NORMAL = vec3(0, -1, 0);

// vec3 screenPos = vec3(-1, -1, 1);
// vec3 screenDimension = vec3(2, 2, 15000);

// vec4 splanesf[6] = vec4[6](
	// vec4(LEFT_NORMAL.x, LEFT_NORMAL.y, LEFT_NORMAL.z, Length(screenPos)),
	// vec4(RIGHT_NORMAL.x, RIGHT_NORMAL.y, RIGHT_NORMAL.z, Length(screenPos + vec3(screenDimension.x, 0, 0))),
	// vec4(FRONT_NORMAL.x, FRONT_NORMAL.y, FRONT_NORMAL.z, Length(screenPos + vec3(0, 0, screenDimension.z))),
	// vec4(BACK_NORMAL.x, BACK_NORMAL.y, BACK_NORMAL.z, Length(screenPos)),
	// vec4(TOP_NORMAL.x, TOP_NORMAL.y, TOP_NORMAL.z, Length(screenPos + vec3(0, screenDimension.y, 0))),
	// vec4(BOTTOM_NORMAL.x, BOTTOM_NORMAL.y, BOTTOM_NORMAL.z, Length(screenPos))
	// );

// vec4 splanesp[6] = vec4[6](
	// vec4(screenPos.x, screenPos.y, screenPos.z, 0),
	// vec4(screenPos.x + screenDimension.x, screenPos.y, screenPos.z, 0),
	// vec4(screenPos.x, screenPos.y, screenPos.z + screenDimension.z, 0),
	// vec4(screenPos.x, screenPos.y, screenPos.z, 0),
	// vec4(screenPos.x, screenPos.y + screenDimension.y, screenPos.z, 0),
	// vec4(screenPos.x, screenPos.y, screenPos.z, 0)
	// );

	
	
//PIERAN - CHANGED//	
	
//Builds a normalized plane from view projection matrix
vec4 BuildPlane(const mat4 mtx, int row, float sign)
{
	vec3 normal = vec3(
				sign * mtx[0][row] + mtx[0][3], //x
				sign * mtx[1][row] + mtx[1][3],	//y
				sign * mtx[2][row] + mtx[2][3]  //z
				); 
				
	float dist = sign * mtx[3][row] + mtx[3][3];

//catches exception but extra branching
//	float len = length(normal)
//	if (len <= 0.0) return vec4(0,0,0,0);

//direct calc - assumes normal is never of length zero!
	float invlen = 1.f / length(normal);
	return vec4(normal * invlen, dist * invlen);
}

//Builds a world space view frustum from viewProj matrix (Taken from Richard's nclgl library)
void FrustumFromMatrix(const mat4 mtx, out vec4 splanesp[6])
{
	splanesp[0] = BuildPlane(mtx, 0,  1.0); //X+
	splanesp[1] = BuildPlane(mtx, 0, -1.0); //X-
	splanesp[2] = BuildPlane(mtx, 1,  1.0); //Y+
	splanesp[3] = BuildPlane(mtx, 1, -1.0); //Y-
	splanesp[4] = BuildPlane(mtx, 2,  1.0); //Z+
	splanesp[5] = BuildPlane(mtx, 2, -1.0); //Z-
}	

//Checks if a sphere intersects or is inside a given frustum
bool QuickSphereColliding(vec4 frustum[6], vec4 sphere)
{
	//for (int i = 0; i < 6; i++)
	//{
	//	if (dot(sphere.xyz, frustum[i].xyz) + frustum[i].w <= -sphere.w)
	//		return false;
	//}
	//return true;

	//Expanded version with less branching - !!unprofiled!! 
	// - Possible the above compiles into a more optimised solution. Though I guess it's food
	//   for thought =]
	float c;
	c  = min(dot(sphere.xyz, frustum[0].xyz) + frustum[0].w + sphere.w, 0.0);
	c += min(dot(sphere.xyz, frustum[1].xyz) + frustum[1].w + sphere.w, 0.0);
	c += min(dot(sphere.xyz, frustum[2].xyz) + frustum[2].w + sphere.w, 0.0);
	c += min(dot(sphere.xyz, frustum[3].xyz) + frustum[3].w + sphere.w, 0.0);
	c += min(dot(sphere.xyz, frustum[4].xyz) + frustum[4].w + sphere.w, 0.0);
	c += min(dot(sphere.xyz, frustum[5].xyz) + frustum[5].w + sphere.w, 0.0);
	
	return c == 0.0;
}
	
//EOF CHANGE//	
	
	
	
	
	
	

