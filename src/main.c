#include <SDL3/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "particle.h"

#define WIDTH 1280
#define HEIGHT 720

SDL_Renderer *rend = NULL;
struct particle *particles = NULL;
size_t particle_count = 0;
float particle_mass = 1.0f;
int quit_flag = 0;

double clamp(double d, double min, double max)
{
	const double t = d < min ? min : d;
	return t > max ? max : t;
}

void render_particles()
{
	for (size_t i = 0; i < particle_count; ++i) {
		struct particle p = particles[i];
		if (p.mass > 30.0f)
			SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
		else
			SDL_SetRenderDrawColor(rend, 0, 200, 255, 255);

		SDL_FRect rect = { p.x - p.width * 0.5f, p.y - p.height * 0.5f, p.width, p.height };
		SDL_RenderFillRect(rend, &rect);
	}
}

void add_particle(float x, float y)
{
	size_t new_count = particle_count + 1;
	struct particle *tmp = realloc(particles, new_count * sizeof(*particles));
	if (!tmp) {
		fprintf(stderr, "realloc failed\n");
		return;
	}

	particles = tmp;
	particle_count = new_count;

	struct particle p = { .x = x,
			      .y = y,
			      .width = particle_mass * 0.8f,
			      .height = particle_mass * 0.8f,
			      .vx = 0.0f,
			      .vy = 0.0f,
			      .ax = 0.0f,
			      .ay = 0.0f,
			      .mass = particle_mass };
	particles[particle_count - 1] = p;
}

void update_particles(float dt)
{
	if (particle_count == 0)
		return;

	for (size_t i = 0; i < particle_count; ++i) {
		particles[i].ax = 0.0f;
		particles[i].ay = 0.0f;
	}

	for (size_t i = 0; i < particle_count; ++i) {
		for (size_t j = i + 1; j < particle_count; ++j) {
			apply_gravity_pair(&particles[i], &particles[j]);
		}
	}

	for (size_t i = 0; i < particle_count; ++i) {
		particles[i].vx += particles[i].ax * dt;
		particles[i].vy += particles[i].ay * dt;
		particles[i].x += particles[i].vx * dt;
		particles[i].y += particles[i].vy * dt;
	}

	for (size_t i = 0; i < particle_count; ++i) {
		for (size_t j = i + 1; j < particle_count; ++j) {
			resolve_pair_collision(&particles[i], &particles[j], 0.6f);
		}
	}

	for (size_t i = 0; i < particle_count; ++i) {
		particle_collision(&particles[i]);
	}
}

void clear_screen()
{
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
	SDL_RenderClear(rend);
}

void handle_inputs(void)
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_EVENT_QUIT:
			quit_flag = 1;
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			if (event.button.button == SDL_BUTTON_LEFT) {
				float mx, my;
				SDL_GetMouseState(&mx, &my);
				add_particle(mx, my);
			}
			break;
		case SDL_EVENT_MOUSE_WHEEL:
			if (event.wheel.y > 0)
				++particle_mass;
			else if (event.wheel.y < 0) {
				particle_mass = clamp(--particle_mass, 1.0f, 500.0f);
			default:
				break;
			}
		}
	}
}

int main(void)
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *win = SDL_CreateWindow("Hyperspace", WIDTH, HEIGHT, 0);
	if (!win) {
		fprintf(stderr, "Window create failed: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	rend = SDL_CreateRenderer(win, NULL);
	if (!rend) {
		fprintf(stderr, "Renderer create failed: %s\n", SDL_GetError());
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 1;
	}

	Uint32 prev_ticks = SDL_GetTicks();

	while (!quit_flag) {
		Uint32 frame_start = SDL_GetTicks();
		float dt = (frame_start - prev_ticks) / 1000.0f;
		if (dt > 0.05f)
			dt = 0.05f;
		prev_ticks = frame_start;

		handle_inputs();
		update_particles(dt);

		clear_screen();
		render_particles();
		SDL_RenderPresent(rend);

		Uint32 frame_time = SDL_GetTicks() - frame_start;
		if (frame_time < (Uint32)FRAME_DELAY_MS)
			SDL_Delay((Uint32)(FRAME_DELAY_MS - frame_time));
	}

	free(particles);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
