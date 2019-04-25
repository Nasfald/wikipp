#ifndef OPTIONS_H
#define OPTIONS_H
#include "master.h"
#include <cppcms/json.h>
#include <cppcms/serialization_classes.h>

namespace apps {

struct global_options : public cppcms::serializable {
	int users_only_edit;
	std::string contact;
	void serialize(cppcms::archive &a);
};

struct locale_options : public cppcms::serializable{
	std::string title;
	std::string about;
	std::string copyright;
	void serialize(cppcms::archive &a);
};




class options : public master {
	bool loaded;
protected:
	void edit();
public:
	void reset();
	options(apps::wiki &);
	void load();
	void save();
	std::string edit_url();
	global_options global;
	locale_options  local;
};

} // apps;

#endif
