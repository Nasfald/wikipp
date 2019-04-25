#ifndef WIKI_H
#define WIKI_H
#include <cppcms/application.h>
#include <cppdb/frontend.h>

#include "page.h"
#include "users.h"
#include "index.h"
#include "options.h"



namespace apps {

class wiki : public cppcms::application {
	friend class apps::page;
	friend class apps::options;
	friend class apps::users;
	friend class apps::index;
	friend class apps::master;

	std::string script;
public:
	std::string conn;
	// Data 
	std::string locale_name;

	// Applications 

	apps::page page;
	apps::options options;
	apps::users users;
	apps::index index;

	std::string root(std::string locale_name="");
	bool set_locale(std::string);
	void run(std::string lang,std::string url);
	//virtual void on_404();
	virtual void main(std::string url);
	wiki(cppcms::service &s);
private:
	std::map<std::string,std::string> lang_map;
};

}

#endif

