#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

struct circle_t
{
	vec2	center=vec2(0);		// 2D position for translation
	float	radius=1.0f;		// radius
	float	theta=0.0f;			// rotation angle
	vec4	color;				// RGBA color in [0,1]
	mat4	model_matrix;		// modeling transformation
	vec2	velocity;			// 속도

	// public functions
	void	update( float t , std::vector<circle_t>& circles );	
	bool	is_collide(circle_t& other);
};

float rand_norm() {
	return ((float)std::rand() / RAND_MAX);
}

/*Elastic collision*/
vec2 ec(circle_t& c1, circle_t& c2) {
	vec2 x1 = c1.center;
	vec2 x2 = c2.center;
	float m1 = c1.radius * c1.radius;
	float m2 = c2.radius * c2.radius;
	vec2 v1 = c1.velocity;
	vec2 v2 = c2.velocity;

	c1.velocity = v1 
		- (2.0f * m2 / (m1 + m2)) 
		* (dot(v1 - v2, x1 - x2) / (x1 - x2).length2())
		* (x1 - x2);
	c2.velocity = v2
		- (2.0f * m1 / (m1 + m2))
		* (dot(v2 - v1, x2 - x1) / (x2 - x1).length2())
		* (x2 - x1);
};

inline std::vector<circle_t> create_circles()
{
	std::srand((uint)time(NULL));
	std::vector<circle_t> circles;
	circle_t c = { vec2(0.0f,0.0f),0.25f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f) };;

	for (int i = 0; i < 20; i++) {	// Poisson Disk Sampling ★★★
		c.center = vec2(rand_norm() * 2.0f - 1.0f, rand_norm() * 2.0f - 1.0f);
		c.radius = rand_norm() * 0.15f + 0.05f;
		c.velocity = vec2(rand_norm() - 0.5f, rand_norm() - 0.5f);
		circles.emplace_back(c);
	}

	return circles;
}



inline void circle_t::update( float t , std::vector<circle_t>& circles)
{
	//radius	= 0.15f+cos(t)*0.05f;		// simple animation
	theta	= t;
	//float c	= cos(theta), s=sin(theta);
	center += velocity;
	
	

	for (auto& c : circles) {
		if (&c == this)		continue; 
		if (is_collide(c)) {
			// 방향전환
			

		}
	}

	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	/*
	mat4 rotation_matrix =
	{
		c,-s, 0, 0,
		s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	*/

	mat4 rotation_matrix =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	
	model_matrix = translate_matrix*rotation_matrix*scale_matrix;
}

inline bool circle_t::is_collide(circle_t& other) {
	return (other.center - center).length2() < (other.radius + radius) * (other.radius + radius);
}
#endif
