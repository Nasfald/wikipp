#ifndef OPTIONS_CONTENT_H
#define OPTIONS_CONTENT_H

#include "master_content.h"

namespace content {

struct options_form : cppcms::form {
	cppcms::widgets::checkbox users_only;
	cppcms::widgets::text contact_mail;
	cppcms::widgets::text wiki_title;
	cppcms::widgets::textarea about;
	cppcms::widgets::text copyright;
	cppcms::widgets::submit submit;
	options_form();
};


struct edit_options:  public master {
	options_form form;
};


} // namespace content

#endif

