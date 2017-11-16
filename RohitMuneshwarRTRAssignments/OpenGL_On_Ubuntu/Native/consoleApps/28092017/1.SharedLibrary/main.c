 #include <stdlib.h>
#include<stdio.h>
#include <dlfcn.h>
#include "geometry.h"

int main(void)
{
	void *handle;
	void (*drawGeometry)(char*);
	char *error;
	printf("Inside main...");
 	handle = dlopen ("libgeometry.so", RTLD_LAZY);
        if (!handle) {
		printf("Unable to load library");
            exit(1);
        }
	drawGeometry = dlsym(handle, "drawGeometry");
        if ((error = dlerror()) != NULL)  {
            printf("Unable to get proc address from library");
            exit(1);
        }

	char name[40] = "Square";
        (*drawGeometry)(name);
        dlclose(handle);
	
	//drawGeometry(name);
	return 0;
}
