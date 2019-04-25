#ifndef MASTER_DATA_H
#define MASTER_DATA_H
#include <cppcms/view.h>
#include <cppcms/form.h>
#include <booster/function.h>
#include <string>

namespace apps { class wiki; }

namespace content {

struct master : public cppcms::base_content {
	std::string media;
	std::string syntax_highlighter;
	std::string cookie_prefix;
	std::string main_link;
	std::string main_local;
	std::string login_link;
	std::string toc;
	std::string changes;
	std::string edit_options;
	std::string contact;
	std::string wiki_title,about,copyright;
	std::map<std::string,std::string> languages;
	booster::function<std::string(std::string const &)> markdown;
};

}


#endif
