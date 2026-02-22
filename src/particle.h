#ifndef PARTICLE_H
#define PARTICLE_H

struct particle {
	float x, y;
	float width, height;
	float vx, vy;
	float ax, ay;
	float mass;
};

void apply_gravity_pair(struct particle *a, struct particle *b);
void particle_collision(struct particle *p);
void resolve_pair_collision(struct particle *a, struct particle *b,
			    float restitution);

#endif
