#include "users.h"
#include "wiki.h"
#include "users_content.h"
#include <sys/time.h>
#include <time.h>

#include <cppcms/url_dispatcher.h>
#include <cppcms/cache_interface.h>
#include <cppcms/session_interface.h>

#define _(X) ::cppcms::locale::translate(X)

namespace content {
login_form::login_form(apps::wiki *_w) :
	w(_w)
{
	username.message(_("Username"));
	password.message(_("Password"));
	login.value(_("Login"));
	add(username);
	add(password);
	add(login);
	username.non_empty();
	password.non_empty();
}

bool login_form::validate()
{
	if(!form::validate())
		return false;
	if(w->users.check_login(username.value(),password.value()))
		return true;
	password.valid(false);
	return false;
}


new_user_form::new_user_form(apps::wiki *_w):
	w(_w)
{
	username.message(_("Username"));
	password1.message(_("Password"));
	password2.message(_("Confirm"));
	captcha.message(_("Solve"));
	submit.value(_("Submit"));
	add(username);
	add(password1);
	add(password2);
	add(captcha);
	add(submit);
	username.non_empty();
	password1.non_empty();
	password2.check_equal(password1);
}

void new_user_form::generate_captcha()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	unsigned seed=tv.tv_usec / 1000 % 100;
	int num1=rand_r(&seed) % 10+1;
	int num2=rand_r(&seed) % 10+1;
	int sol=num1+num2;
	captcha.help((booster::locale::format("{1} + {2}") % num1 % num2).str());
	w->session().set("captcha",sol);
	w->session().age(5*60); // at most 5 minutes
}

bool new_user_form::validate()
{
	if(!form::validate())
		return false;
	
	if(!w->session().is_set("captcha") || captcha.value()!=w->session()["captcha"]) {
		w->session().erase("captcha");
		return false;
	}
	if(w->users.user_exists(username.value())) {
		username.error_message(_("This user exists"));
		username.valid(false);
		return false;
	}
	return true;
}


} 

namespace apps {

users::users(wiki &w) :	master(w)
{
	wi.dispatcher().assign("^/login/?$",&users::login,this);
	disable_reg=settings().get("wikipp.disable_registration",true);
	if(!disable_reg){
		wi.dispatcher().assign("^/register/?$",&users::new_user,this);
	}
	reset();
}

void users::new_user()
{
	content::new_user c(&wi);
	if(request().request_method()=="POST") {
		c.form.load(context());
		cppdb::session sql(conn);
		cppdb::transaction tr(sql);
		if(c.form.validate()) {
			sql<<	"INSERT INTO users(username,password) "
				"VALUES(?,?)"
				<< c.form.username.value()
				<< c.form.password1.value()
				<< cppdb::exec;
			tr.commit();
			wi.page.redirect(locale_name);
			session()["username"]=c.form.username.value();
			session().expose("username");
			session().default_age(); // return to default
			return;
		}
		tr.commit();
	}
	c.form.generate_captcha();
	ini(c);
	render("new_user",c);
}

void users::reset()
{
	auth_done=auth_ok=false;
}

std::string users::login_url()
{
	return wi.root()+"/login/";
}

bool users::user_exists(std::string u)
{
	std::string key="user_exists_"+u;
	std::string tmp;
	if(cache().fetch_frame(key,tmp,true)) { // No triggers
		return true;
	}
	cppdb::result r;
	cppdb::session sql(conn);
	r=sql<<"SELECT id FROM users WHERE username=?" << u << cppdb::row;
	if(!r.empty()) {
		cache().store_frame(key,tmp);
		return true;
	}
	return false;
}

void users::login()
{
	content::login c(&wi);
	if(request().request_method()=="POST") {
		c.form.load(context());
		if(c.form.validate()) {
			wi.page.redirect(locale_name);
			session()["username"]=c.form.username.value();
			session().expose("username");
			return;
		}
	}
	else {
		if(auth()) {
			response().set_redirect_header(request().http_referer());
			session().clear();
			return;
		}
	}
	ini(c);
	if(!disable_reg)
		c.new_user=wi.root()+"/register/";
	render("login",c);
}

bool users::check_login(std::string u,std::string p)
{
	if(u.empty() || p.empty())
		return false;
	cppdb::result r;
	cppdb::session sql(conn);
	r=sql<<	"SELECT password FROM users "
		"WHERE username=?" << u << cppdb::row;
	if(r.empty()) {
		return false;
	}
	std::string pass;
	r>>pass;
	if(p!=pass)
		return false;
	return true;
}

bool users::auth()
{
	if(!auth_done)
		do_auth();
	return auth_ok;
}

void users::do_auth()
{
	if(session().is_set("username") && user_exists(session()["username"])) {
		auth_ok=true;
	}
	else {
		auth_ok=false;
	}
	if(auth_ok)
		username=session()["username"];
	else
		username=request().remote_addr();
	auth_done=true;
}

void users::error_forbidden()
{
	response().set_redirect_header(login_url());
}


}
