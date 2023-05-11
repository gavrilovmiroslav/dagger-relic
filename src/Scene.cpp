#include "Scene.h"
#include <string.h>

struct Scene scene;

void Scene::Reset(void)
{
    memset(this, 0, sizeof(Scene));
}
