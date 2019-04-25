#include <cppcms/service.h>
#include <cppcms/applications_pool.h>
#include <booster/log.h>
#include "wiki.h"

int main(int argc,char **argv)
{
	try {
		cppcms::service app(argc,argv);
		app.applications_pool().mount(cppcms::applications_factory<apps::wiki>());
		app.run();
	}
	catch(std::exception const &e) {
		BOOSTER_ERROR("wikipp") << e.what() ;
		std::cerr<<e.what()<<std::endl;
		return 1;
	}
	return 0;
}
