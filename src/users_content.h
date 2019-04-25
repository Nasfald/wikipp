#ifndef USERS_CONTENT_H
#define USERS_CONTENT_H

#include "master_content.h"

namespace content {

struct login_form : public cppcms::form {
	apps::wiki *w;
	cppcms::widgets::text username;
	cppcms::widgets::password password;
	cppcms::widgets::submit login;
	login_form(apps::wiki *);
	virtual bool validate();
};

struct new_user_form : public cppcms::form {
	apps::wiki *w;
	cppcms::widgets::text username;
	cppcms::widgets::password password1;
	cppcms::widgets::password password2;
	cppcms::widgets::text captcha;
	cppcms::widgets::submit submit;
	new_user_form(apps::wiki *w);
	void generate_captcha();
	bool virtual validate(); 
};

struct new_user : public master {
	new_user_form form;
	new_user(apps::wiki *w) : form(w){};
};

struct login : public master {
	login_form form;
	std::string new_user;
	login(apps::wiki *w) : form(w){};
};

} // namespace content

#endif
