#include <math.h>
#include "particle.h"
#include "main.h"

void apply_gravity_pair(struct particle *a, struct particle *b)
{
	// Euclidean distance
	float dx = b->x - a->x;
	float dy = b->y - a->y;
	float dist2 = dx * dx + dy * dy + EPS;
	float dist = sqrtf(dist2);
	if (dist < 1e-6f)
		return;

	float ux = dx / dist;
	float uy = dy / dist;
	float F = G * a->mass * b->mass / dist2;
	float Fx = F * ux;
	float Fy = F * uy;

	a->ax += Fx / a->mass;
	a->ay += Fy / a->mass;
	b->ax -= Fx / b->mass;
	b->ay -= Fy / b->mass;
}

void particle_collision(struct particle *p)
{
	float halfw = p->width * 0.5f;
	float halfh = p->height * 0.5f;

	/* floor */
	if (p->y + halfh > HEIGHT) {
		p->y = HEIGHT - halfh;
		p->vy = -p->vy * 0.6f;
		if (fabsf(p->vy) < 1.0f)
			p->vy = 0.0f;
	}
	/* ceiling */
	if (p->y - halfh < 0) {
		p->y = halfh;
		p->vy = -p->vy * 0.6f;
	}
	/* left wall */
	if (p->x - halfw < 0) {
		p->x = halfw;
		p->vx = -p->vx * 0.8f;
	}
	/* right wall */
	if (p->x + halfw > WIDTH) {
		p->x = WIDTH - halfw;
		p->vx = -p->vx * 0.8f;
	}
}

void resolve_pair_collision(struct particle *a, struct particle *b,
			    float restitution)
{
	float ra = a->width * 0.5f;
	float rb = b->width * 0.5f;
	float dx = b->x - a->x;
	float dy = b->y - a->y;
	float dist2 = dx * dx + dy * dy;
	float dist = sqrtf(dist2);
	if (dist <= 0.0f) {
		dx = 0.01f;
		dy = 0.0f;
		dist = 0.01f;
		dist2 = dist * dist;
	}
	float radii = ra + rb;
	if (dist >= radii)
		return;

	/* positional correction */
	const float percent = 0.8f;
	const float slop = 0.01f;
	float penetration = radii - dist;
	float invMassA = (a->mass > 0.0f) ? 1.0f / a->mass : 0.0f;
	float invMassB = (b->mass > 0.0f) ? 1.0f / b->mass : 0.0f;
	float invMassSum = invMassA + invMassB;
	if (invMassSum == 0.0f)
		return;
	float nx = dx / dist;
	float ny = dy / dist;
	float correction =
		percent * fmaxf(penetration - slop, 0.0f) / invMassSum;
	a->x -= nx * correction * invMassA;
	a->y -= ny * correction * invMassA;
	b->x += nx * correction * invMassB;
	b->y += ny * correction * invMassB;

	float rvx = b->vx - a->vx;
	float rvy = b->vy - a->vy;
	float velAlongNormal = rvx * nx + rvy * ny;
	if (velAlongNormal > 0)
		return; 

	float j = -(1.0f + restitution) * velAlongNormal;
	j /= invMassSum;

	float ix = j * nx;
	float iy = j * ny;
	if (a->mass > 0.0f) {
		a->vx -= ix * invMassA;
		a->vy -= iy * invMassA;
	}
	if (b->mass > 0.0f) {
		b->vx += ix * invMassB;
		b->vy += iy * invMassB;
	}
}
