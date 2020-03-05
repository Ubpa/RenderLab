#include <Basic/Math.h>
#include <random>

#include <ctime>

using namespace Ubpa;

using namespace std;

static uniform_int_distribution<unsigned> uiMap;
static uniform_int_distribution<signed> iMap;
static uniform_real_distribution<float> fMap(0.0f,1.0f);
static uniform_real_distribution<float> fMap_exclude1(0.0f, 0.99999999f);
static uniform_real_distribution<double> dMap(0.0,1.0);
static default_random_engine engine;

int Math::Rand_I() {
	return iMap(engine);
}

unsigned int Math::Rand_UI() {
	return uiMap(engine);
}

float Math::Rand_F() {
	return fMap(engine);
}

float Math::Rand_F_exclude1() {
	return fMap_exclude1(engine);
}

double Math::Rand_D() {
	return dMap(engine);
}

void Math::RandSetSeedByCurTime() {
	engine.seed(clock());
}
