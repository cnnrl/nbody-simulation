#include "arena-cpp/arena.hpp"
#include "clmath/clmath.hpp"
#include <new>
#include <raylib.h>

#define N 50
#define G 100
#define MAX_SCREEN 800
#define MIN_RAD 5
#define MAX_RAD 10
#define MIN_MASS 5
#define MAX_MASS 100


struct Body 
{
  Vector<double, 2> velocity; // m/s
  Vector<double, 2> position; // (m, m)
  Vector <double, 2> force;   // N
  double mass;                // kg
  double radius;                 // m
};

double randDouble(double min, double max) 
{
  std::random_device rd{};
  std::mt19937 gen{rd()};
  std::uniform_real_distribution<double> dis(min, max);

  return dis(gen);
}

void accumulateForce(Body* bodies, double dt) 
{
  constexpr double softness {1.0};
  for (std::size_t i {0}; i < N; ++i) 
  {
    Vector<double, 2> accel {zero_vector<double, 2>};

    for (std::size_t j {i + 1}; j < N; ++j) 
    {

      if (i == j) 
      {
        continue;
      }

      Vector<double, 2> difference = bodies[j].position - bodies[i].position;
      double distanceSquared {magnitudeSquared(difference)};

      Vector<double, 2> force {(G * bodies[i].mass * bodies[j].mass / ( distanceSquared + softness * softness)) * normalize(difference)};
      
      bodies[i].force += force;
      bodies[j].force -= force;
    }
  }
}

void updateBodies(Body* bodies, double dt) 
{
  for (std::size_t i {0}; i < N; ++i) {
    Vector<double, 2> accel = bodies[i].force * (1 / bodies[i].mass);

    bodies[i].velocity += accel * dt;
    bodies[i].position += bodies[i].velocity * dt;
    bodies[i].force = zero_vector<double, 2>;
  }
}

int main() 
{
  constexpr size_t ARENA_SIZE = N * sizeof(Body) + alignof(Body) * N;
  Arena arena(ARENA_SIZE);

  Body* bodies = static_cast<Body*>(arena.allocate(sizeof(Body) * N, alignof(Body)));
  for (int i {0}; i < N; ++i) 
  {
    new (&bodies[i]) Body 
    {
      randVec<double, 2>(-20.0, 20.0),  // velocity
      randVec<double, 2>(0.0, (double)MAX_SCREEN), // postion 
      zero_vector<double, 2>, // force
      randDouble(MIN_MASS, MAX_MASS), // mass
      randDouble(MIN_RAD, MAX_RAD) // radius
    };
  }


  InitWindow(MAX_SCREEN, MAX_SCREEN, "Hello raylib");

  while (!WindowShouldClose()) 
  {
    BeginDrawing();
    ClearBackground(BLACK);

    for (int i {0}; i < N; ++i) 
    {
      Vector2 pos = {(float)bodies[i].position[0], (float)bodies[i].position[1]};
      DrawCircleV(pos, bodies[i].radius, WHITE);
    }

    double dt {GetFrameTime()};

    accumulateForce(bodies, dt);
    updateBodies(bodies, dt);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
