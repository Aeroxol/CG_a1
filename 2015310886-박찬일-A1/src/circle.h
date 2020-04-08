#pragma once
#include <random>
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

struct circle_t
{
	vec2	center = vec2(0);		// 2D position for translation
	float	radius = 0.1f;		// radius
	float	theta = 0.0f;			// rotation angle
	vec4	color;				// RGBA color in [0,1]
	mat4	model_matrix;		// modeling transformation
	vec2	velocity;			// 속도

	// public functions
	void	update(float t, std::vector<circle_t>& circles);
	bool	is_colliding(circle_t& other);
	bool	circle_t::is_out();
	void	ec(circle_t& c2);
	void	ec_border();
};

/*Elastic collision*/
void circle_t::ec(circle_t& c2) {
	vec2 x1 = center;
	vec2 x2 = c2.center;
	float m1 = radius * radius;
	float m2 = c2.radius * c2.radius;
	vec2 v1 = velocity;
	vec2 v2 = c2.velocity;
	
	float d;
	d = radius + c2.radius - (center - c2.center).length();
	center += (center - c2.center).normalize() * 0.5f * d;
	c2.center += (c2.center - center).normalize() * 0.5f * d;
	//velocity = (v1 * (m1 - m2) + 2.0f * m2 * v2) / (m1 + m2);
	//c2.velocity = (v2 * (m2- m1) + 2.0f * m1 * v1) / (m1 + m2);
	
	velocity = v1
		- ((2.0f * m2) / (m1 + m2))
		* (dot(v1 - v2, x1 - x2) / (x1 - x2).length2())
		* (x1 - x2);

	c2.velocity = v2
		- ((2.0f * m1) / (m1 + m2))
		* (dot(v2 - v1, x2 - x1) / (x2 - x1).length2())
		* (x2 - x1);
	return;
};

void circle_t::ec_border() {
	if (center.x + radius > 1 ) {
		velocity.x = abs(velocity.x) * -1;
	}
	else if (center.x - radius < -1) {
		velocity.x = abs(velocity.x);
	}
	else if (center.y + radius > 1 ) {
		velocity.y = abs(velocity.y) * -1;
	}
	else if (center.y - radius < -1) {
		velocity.y = abs(velocity.y);
	}
}

std::vector<circle_t> create_circles(uint num)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> rand_one(0.0f, 1.0f);
	std::vector<circle_t> circles;
	circle_t c = { vec2(0.0f,0.0f),0.15f,0.0f,vec4(0.5f,0.5f,0.5f,1.0f) };

	num = clamp(num, 20, 30);
	for (uint i = 0; i < num; i++) {	// Poisson Disk Sampling ★★★
		bool near_circle = true;
		
		while (near_circle) {
			float max_radius = 0.2f;
			near_circle = false;
			c.center = vec2(rand_one(gen) * 1.8f - 0.9f, rand_one(gen) * 1.8f - 0.9f);	// make position
			max_radius = min(max_radius, min(min(1.0f - c.center.x, c.center.x + 1.0f), min(1.0f - c.center.y, c.center.y + 1.0f)));		// radius should not exceed d
			
			for (auto& ob : circles) {
				if ((c.center - ob.center).length() - ob.radius < 0.05f) {
					// if too near to another circle => reposition
					near_circle = true;
					break;
				}
				max_radius = min(max_radius, (c.center - ob.center).length() - ob.radius);
			}
			float d = rand_one(gen) * 0.15f + 0.05f;
			c.radius = min(max_radius, d);
			c.color = vec4(rand_one(gen)*0.8f + 0.2f, rand_one(gen) * 0.8f + 0.2f, rand_one(gen) * 0.8f + 0.2f, 1.0f);
			//c.radius = 0.05f;
			c.velocity = vec2(rand_one(gen) - 0.5f, rand_one(gen) - 0.5f) * 0.05f;
		}
		circles.emplace_back(c);
	}
	return circles;
}

inline void circle_t::update( float t , std::vector<circle_t>& circles)
{
	//radius	= 0.15f+cos(t)*0.05f;		// simple animation
	theta	= t;
	//float c	= cos(theta), s=sin(theta);
	bool compare = false;
	for (auto& c : circles) {
		if (compare) {
			if (is_colliding(c)) {
				// 탄성충돌
				ec(c);
			}
		}
		if (&c == this) {
			compare = true;
		}
	}
	if (is_out()) {
		ec_border();
	}
	center += velocity;


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

inline bool circle_t::is_out() {
	return (center.x + velocity.x + radius > 1 || center.x + velocity.x - radius < -1 || center.y + velocity.y + radius > 1 || center.y + velocity.y - radius < -1);
}
inline bool circle_t::is_colliding(circle_t& other) {
	return (other.center - (center+velocity)).length2() < (other.radius + radius) * (other.radius + radius);
}
#endif
