#include "wiki.h"
#include <booster/regex.h>

#include <cppcms/url_dispatcher.h>
#include <cppcms/http_context.h>

namespace apps {

wiki::wiki(cppcms::service &srv) :
	cppcms::application(srv),
 	conn(settings().get<std::string>("wikipp.connection_string")),
	page(*this),
	options(*this),
	users(*this),
	index(*this)
{
	add(page);
	add(options);
	add(users);
	add(index);

	script=settings().get<std::string>("wikipp.script");
	cppcms::json::object langs = settings().at("wikipp.languages").object();
	for(cppcms::json::object::const_iterator p=langs.begin();p!=langs.end();++p) {
		lang_map[p->first]=p->second.str();
	}
	this->language_default = settings().at("wikipp.language_default").str();
}

std::string wiki::root(std::string l)
{
	if(l.empty()) l=locale_name;
	return script+"/"+l;
}
static const booster::regex lang_regex("^/(\\w+)(/.*)?$");

void wiki::main(std::string url)
{
	booster::smatch res;
	options.reset();
	users.reset();
	if(booster::regex_match(url,res,lang_regex)) {
		std::map<std::string,std::string>::const_iterator p = lang_map.find(std::string(res[1]));
		if(p==lang_map.end()) {
			locale_name = this->language_default ;
			page.redirect( this->language_default );
		}
		else {
			locale_name = p->first;
			context().locale(p->second);
			if(!dispatcher().dispatch(res[2]))
				page.redirect(locale_name);
		}
	}
	else
		page.redirect( this->language_default );
}


} // apps
