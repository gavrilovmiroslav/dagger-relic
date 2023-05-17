#include "Scene.h"
#include <string.h>

struct Scene scene;

void Scene::Reset(void)
{
    U32 i;

    boxcount            = 0;
    boxcount_current    = 0;
    pushplatecount      = 0;
    pushplate_activenow = 0;
    dooropen            = false;
}
