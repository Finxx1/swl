#include <swl.h>

#include <assert.h>

int main(int argc, char** argv) {
	assert(swl_init(SWL_BACKEND_ANY));
	
	swl_window_t* window = swl_create_window();
	assert(window);
}
