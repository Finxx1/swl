#include <swl.h>

#include <stdio.h>

int main(int argc, char** argv) {
	if (swl_init(SWL_BACKEND_ANY) == false) {
		printf("%s\n", swl_get_error());
		return 1;
	}
	
	swl_window_t* window = swl_create_window();
	if (window == NULL) {
		printf("%s\n", swl_get_error());
		return 1;
	}

	
}
