vec3 LEFT_NORMAL = vec3(-1, 0, 0);
vec3 FRONT_NORMAL = vec3(0, 0, -1);
vec3 RIGHT_NORMAL = vec3(1, 0, 0);
vec3 BACK_NORMAL = vec3(0, 0, 1);
vec3 TOP_NORMAL = vec3(0, 1, 0);
vec3 BOTTOM_NORMAL = vec3(0, -1, 0);

vec3 screenPos = vec3(-1, -1, 1);
vec3 screenDimension = vec3(2, 2, 15000);

vec4 splanesf[6] = vec4[6](
	vec4(LEFT_NORMAL.x, LEFT_NORMAL.y, LEFT_NORMAL.z, Length(screenPos)),
	vec4(RIGHT_NORMAL.x, RIGHT_NORMAL.y, RIGHT_NORMAL.z, Length(screenPos + vec3(screenDimension.x, 0, 0))),
	vec4(FRONT_NORMAL.x, FRONT_NORMAL.y, FRONT_NORMAL.z, Length(screenPos + vec3(0, 0, screenDimension.z))),
	vec4(BACK_NORMAL.x, BACK_NORMAL.y, BACK_NORMAL.z, Length(screenPos)),
	vec4(TOP_NORMAL.x, TOP_NORMAL.y, TOP_NORMAL.z, Length(screenPos + vec3(0, screenDimension.y, 0))),
	vec4(BOTTOM_NORMAL.x, BOTTOM_NORMAL.y, BOTTOM_NORMAL.z, Length(screenPos))
	);

vec4 splanesp[6] = vec4[6](
	vec4(screenPos.x, screenPos.y, screenPos.z, 0),
	vec4(screenPos.x + screenDimension.x, screenPos.y, screenPos.z, 0),
	vec4(screenPos.x, screenPos.y, screenPos.z + screenDimension.z, 0),
	vec4(screenPos.x, screenPos.y, screenPos.z, 0),
	vec4(screenPos.x, screenPos.y + screenDimension.y, screenPos.z, 0),
	vec4(screenPos.x, screenPos.y, screenPos.z, 0)
	);
