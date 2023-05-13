#include "Scene.h"
#include <string.h>

struct Scene scene;

void Scene::Reset(void)
{
    U32 i;

    for (i = 0; i < entity.size(); i++)
    {
        registry->destroy(entity[i]);
    }
    entity.clear();

    boxcount            = 0;
    boxcount_current    = 0;
    pushplatecount      = 0;
    pushplate_activenow = 0;
}
