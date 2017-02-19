
#define GL_PI 3.14159265

#ifdef SHADER_SECTION
// Returns vec2( min_distance, material_id )
vec2 map( vec3 pos );
#endif

// Finite difference normal
vec3 getNormal( vec3 pos )
{
	vec3 eps = vec3( c_normal_distance, 0.0, 0.0 );
	vec3 nor = vec3(
		map(pos+eps.xyy).x - map(pos-eps.xyy).x,
		map(pos+eps.yxy).x - map(pos-eps.yxy).x,
		map(pos+eps.yyx).x - map(pos-eps.yyx).x );
	return normalize(nor);
}

// The "Chamfer" flavour makes a 45-degree chamfered edge (the diagonal of a square of size <r>):
float fOpUnionChamfer(float a, float b, float r) {
	return min(min(a, b), (a - r + b)*sqrt(0.5f));
}

// Intersection has to deal with what is normally the inside of the resulting object
// when using union, which we normally don't care about too much. Thus, intersection
// implementations sometimes differ from union implementations.
float fOpIntersectionChamfer(float a, float b, float r) {
	return max(max(a, b), (a + r + b)*sqrt(0.5f));
}

// Difference can be built from Intersection or Union:
float fOpDifferenceChamfer (float a, float b, float r) {
	return fOpIntersectionChamfer(a, -b, r);
}

// The "Round" variant uses a quarter-circle to join the two objects smoothly:
float fOpUnionRound(float a, float b, float r) {
	vec2 u = max(vec2(r - a,r - b), vec2(0));
	return max(r, min (a, b)) - length(u);
}

float fOpIntersectionRound(float a, float b, float r) {
	vec2 u = max(vec2(r + a,r + b), vec2(0));
	return min(-r, max (a, b)) + length(u);
}

float fOpDifferenceRound (float a, float b, float r) {
	return fOpIntersectionRound(a, -b, r);
}

// Similar to fOpUnionRound, but more lipschitz-y at acute angles
// (and less so at 90 degrees). Useful when fudging around too much
// by MediaMolecule, from Alex Evans' siggraph slides
float fOpUnionSoft(float a, float b, float r) {
	float e = max(r - abs(a - b), 0.f);
	return min(a, b) - e*e*0.25f/r;
}


// produces a cylindical pipe that runs along the intersection.
// No objects remain, only the pipe. This is not a boolean operator.
float fOpPipe(float a, float b, float r) {
	return length(vec2(a, b)) - r;
}

// first object gets a v-shaped engraving where it intersect the second
float fOpEngrave(float a, float b, float r) {
	return max(a, (a + r - abs(b))*sqrt(0.5f));
}

// first object gets a capenter-style groove cut out
float fOpGroove(float a, float b, float ra, float rb) {
	return max(a, min(a + ra, rb - abs(b)));
}

// first object gets a capenter-style tongue attached
float fOpTongue(float a, float b, float ra, float rb) {
	return min(a, max(a - ra, abs(b) - rb));
}

float fCylinderZ(vec3 p, float r, float height) {
	float d = length(p.xy) - r;
	d = max(d, abs(p.z) - height);
	return d;
}

// Rotate around a coordinate axis (i.e. in a plreturn vec3( p.x, pyz );ane perpendicular to that axis) by angle <a>.
// Read like this: R(p.xz, a) rotates "x towards z".
// This is fast if <a> is a compile-time constant and slower (but still practical) if not.
vec3 pRX( vec3 p, float a ) {
	
    vec2 pyz = p.yz * cos(a) + vec2(p.z, -p.y) * sin(a);
    return vec3( p.x, pyz );
    
}

vec3 pRY( vec3 p, float a) {
    
	vec2 pxz = cos(a)*p.xz + sin(a)*vec2(p.z, -p.x);
    return vec3( pxz.x, p.y, pxz.y );
}