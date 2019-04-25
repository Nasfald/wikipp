#include "page.h"
#include "page_content.h"
#include "wiki.h"
#include "diff.h"

#include <booster/posix_time.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/cache_interface.h>

#define _(X) ::cppcms::locale::translate(X)

namespace content { 
// Page content
page_form::page_form(apps::wiki *_w):
	w(_w)
{
	title.message(_("Title"));
	content.message(_("Content"));
	sidebar.message(_("Sidebar"));
	save.value(_("Save"));
	save_cont.value(_("Save and Continue"));
	preview.value(_("Preview"));
	fields.add(title);
	fields.add(content);
	fields.add(sidebar);
	buttons.add(save);
	buttons.add(save_cont);
	buttons.add(preview);
	buttons.add(users_only);
	add(fields);
	add(buttons);
	users_only.help(_("Disable editing by visitors"));
	users_only.error_message(_("Please Login"));
	title.non_empty();
	content.non_empty();
	content.rows(25);
	content.cols(60);
	sidebar.rows(10);
	sidebar.cols(60);
}

bool page_form::validate()
{
	bool res=form::validate();
	if(users_only.value() && !w->users.auth()) {
		users_only.valid(false);
		users_only.value(false);
		return false;
	}
	return res;
}

} // namespace content

namespace apps {

page::page(wiki &w):
	master(w)
{
	wi.dispatcher().assign("^/page/(\\w+)/version/(\\d+)$",&page::display_ver,this,1,2);
	wi.dispatcher().assign("^/page/(\\w+)/?$",&page::display,this,1);
	wi.dispatcher().assign("^/page/(\\w+)/edit(/version/(\\d+))?$",&page::edit,this,1,3);
	wi.dispatcher().assign("^/page/(\\w+)/history(/|/(\\d+))?$",&page::history,this,1,3);
	wi.dispatcher().assign("^/page/(\\w+)/diff/(\\d+)vs(\\d+)/?$",&page::diff,this,1,2,3);
}

std::string page::diff_url(int v1,int v2,std::string l,std::string s)
{
	if(l.empty()) l=locale_name;
	if(s.empty()) s=slug;
	return wi.root(l) + 
		(booster::locale::format("/page/{1}/diff/{2}vs{3}") % s % v1 % v2).str();
}

std::string page::page_url(std::string l,std::string s)
{
	if(l.empty()) l=locale_name;
	if(s.empty()) s=slug;
	return wi.root(l)+"/page/"+s;
}

std::string page::page_version_url(int ver,std::string l,std::string s)
{
	if(l.empty()) l=locale_name;
	if(s.empty()) s=slug;
	return wi.root(l)+
		(booster::locale::format("/page/{1}/version/{2}") % s % ver).str();
}
std::string page::edit_url()
{
	return wi.root()+"/page/"+slug+"/edit";
}
std::string page::edit_version_url(int ver)
{
	return (booster::locale::format(edit_url()+"/version/{1}") % ver).str();
}
std::string page::history_url(int n)
{
	std::string u=wi.root()+"/page/"+slug+"/history/";
	if(n)
		u+=(booster::locale::format("{1}") % n).str();
	return u;
}

void page::diff(std::string slug,std::string sv1,std::string sv2)
{
	int v1=atoi(sv1.c_str()), v2=atoi(sv2.c_str());
	this->slug=slug;
	cppdb::session sql(conn);
	cppdb::result r;
	content::diff c;
	c.v1=v1;
	c.v2=v2;
	c.edit_v1=edit_version_url(v1);
	c.edit_v2=edit_version_url(v2);
	r=sql<<	"SELECT version,history.title,history.content,history.sidebar,pages.title FROM pages "
		"JOIN history ON pages.id=history.id "
		"WHERE lang=? AND slug=? AND version IN (?,?) " << locale_name << slug << v1 << v2;
	
	std::string t1,c1,s1,t2,c2,s2;
	int count=0;
	while(r.next()) {
		int ver;
		r>>ver;
		if(ver==v1) {
			r>>t1>>c1>>s1>>c.title;
		}
		else {
			r>>t2>>c2>>s2;
		}
		count++;
	}
	if(count != 2) {
		c.no_versions=true;
		master::ini(c);
		render("diff",c);
		return;
	}
	if(t1!=t2) {
		c.title_diff=true;
		c.title_1=t1;
		c.title_2=t2;
	}
	else {
		c.title=t1;
	}
	if(c1!=c2) {
		c.content_diff=true;
		std::vector<std::string> X=split(c1);
		std::vector<std::string> Y=split(c2);
		diff::diff(X,Y,c.content_diff_content);
	}
	if(s1!=s2) {
		c.sidebar_diff=true;
		std::vector<std::string> X=split(s1);
		std::vector<std::string> Y=split(s2);
		diff::diff(X,Y,c.sidebar_diff_content);
	}
	if(t1==t2 && c1==c2 && s1==s2) 
		c.no_diff=true;
	master::ini(c);
	render("diff",c);
}

void page::history(std::string slug,std::string page)
{
	this->slug=slug;
	unsigned const vers=10;
	int offset;
	content::history c;
	master::ini(c);
	if(page.empty())
		offset=0;
	else
		offset=atoi(page.c_str());

	cppdb::session sql(conn);
	cppdb::result r;
	r=sql<<	"SELECT title,id FROM pages "
		"WHERE pages.lang=? AND pages.slug=? " << locale_name << slug << cppdb::row;
	if(r.empty()) {
		redirect(locale_name);
		return;
	}
	int id;
	r>>c.title>>id;
	r=sql<<	"SELECT created,version,author FROM history "
		"WHERE id=? "
		"ORDER BY version DESC "
		"LIMIT ? "
		"OFFSET ?"
		<<id << vers+1 << offset*vers;

	c.hist.reserve(vers);
	for(unsigned i=0;r.next() && i<vers;i++) {
		int ver;
		c.hist.resize(i+1);
		std::tm update = std::tm();
		r>> update >> ver >> c.hist[i].author ;
		c.hist[i].update = mktime(&update);
		c.hist[i].version=ver;
		c.hist[i].show_url=page_version_url(ver);
		c.hist[i].edit_url=edit_version_url(ver);
		if(ver>1)
			c.hist[i].diff_url=diff_url(ver-1,ver);
	}
	
	if(r.next()) {
		c.page=history_url(offset+1);
	}

	c.page_link=page_url();
	render("history",c);
}


void page::display(std::string slug)
{
	this->slug=slug;
	std::string key="article_"+locale_name+":"+slug;
	if(cache().fetch_page(key))
		return;
	content::page c;

	cppdb::session sql(conn);
	cppdb::result r;
	r=sql<<	"SELECT title,content,sidebar FROM pages WHERE lang=? AND slug=?"
		<<locale_name << slug << cppdb::row;
	if(r.empty()) {
		std::string redirect=edit_url();
		std::cerr << " Page " << redirect << std::endl;
		response().set_redirect_header(redirect);
		return;
	}
	ini(c);
	r >> c.title >> c.content >> c.sidebar;
	render("page",c);
	cache().store_page(key);
}

void page::ini(content::page &c)
{
	master::ini(c);
	c.edit_link=edit_url();
	c.history_link=history_url();
}

void page::edit(std::string slug,std::string version)
{
	this->slug=slug;
	content::edit_page c(&wi);
	if(request().request_method()=="POST") {
		if(!edit_on_post(c))
			return;
	}
	else {
		if(version.empty()) {
			c.new_page=!load(c.form);
			if(c.new_page) {
				wi.options.load();
				c.form.users_only.value(wi.options.global.users_only_edit);
			}
		}
		else {
			int ver=atoi(version.c_str());
			if(!load_history(ver,c.form)) {
				redirect(locale_name,slug);
				return;
			}
		}
		if(c.form.users_only.value() && !wi.users.auth()) {
			wi.users.error_forbidden();
		}
	}
	ini(c);
	c.back=page_url();
	c.submit=edit_url();
	render("edit_page",c);
}

bool page::load(content::page_form &form)
{
	cppdb::session sql(conn);
	cppdb::result r;
	r=sql<<	"SELECT title,content,sidebar,users_only "
		"FROM pages WHERE lang=? AND slug=?" << locale_name << slug << cppdb::row;
	if(!r.empty()) {
		std::string title,content,sidebar;
		int users_only;
		r >> title >> content >> sidebar >> users_only;
		form.title.value(title);
		form.content.value(content);
		form.sidebar.value(sidebar);
		form.users_only.value(users_only);
		return true;
	}
	wi.options.load();
	form.users_only.set(wi.options.global.users_only_edit);
	return false;
}

void page::redirect(std::string loc,std::string slug)
{
	std::string redirect=page_url(loc,slug);
	response().set_redirect_header(redirect);
}

void page::save(int id,content::page_form &form,cppdb::session &sql)
{
	std::tm t = booster::ptime::local_time(booster::ptime::now());
	wi.users.auth();
	if(id!=-1) {
		sql<<	"UPDATE pages SET content=?,title=?,sidebar=?,users_only=? "
			"WHERE lang=? AND slug=?"
				<< form.content.value() << form.title.value() 
				<< form.sidebar.value() << form.users_only.value() 
				<< locale_name << slug << cppdb::exec;
	}
	else {
		cppdb::statement s;
		s=sql<<	"INSERT INTO pages(lang,slug,title,content,sidebar,users_only) "
			"VALUES(?,?,?,?,?,?)"
			<< locale_name << slug
			<< form.title.value()
			<< form.content.value()
			<< form.sidebar.value()
			<< form.users_only.value();
		s.exec();
		id=s.sequence_last("pages_id_seq");
	}
	sql<<	"INSERT INTO history(id,version,created,title,content,sidebar,author) "
		"SELECT ?,"
		"	(SELECT COALESCE(MAX(version),0)+1 FROM history WHERE id=?),"
		"	?,?,?,?,?"
			<< id << id << t 
			<< form.title.value()
			<< form.content.value()
			<< form.sidebar.value()
			<< wi.users.username 
			<< cppdb::exec;
}


bool page::edit_on_post(content::edit_page &c)
{
	wi.options.load();
	
	cppdb::session sql(conn);
	cppdb::transaction tr(sql);
	cppdb::result r;
	r=sql<<	"SELECT id,users_only FROM pages WHERE lang=? and slug=?" << locale_name << slug << cppdb::row;
	int id=-1,users_only=wi.options.global.users_only_edit;
	if(!r.empty()) {
		r>>id>>users_only;
	}
	r.clear();

	if(users_only && !wi.users.auth()) {
		wi.users.error_forbidden();
		return false;
	}
	c.form.load(context());
	if(c.form.validate()) {
		if(c.form.save.value() || c.form.save_cont.value()) {
			save(id,c.form,sql);
			cache().rise("article_"+locale_name+":"+slug);
		}
		if(c.form.save.value()) {
			redirect(locale_name,slug);
			tr.commit();
			return false;
		}
		if(c.form.preview.value()) {
			c.title=c.form.title.value();
			c.content=c.form.content.value();
			c.sidebar=c.form.sidebar.value();
		}
	}
	tr.commit();
	return true;
}

bool page::load_history(int ver,content::page_form &form)
{
	cppdb::session sql(conn);
	cppdb::result r;
	r=sql<<	"SELECT history.title,history.content,history.sidebar,pages.users_only "
		"FROM pages "
		"JOIN history ON pages.id=history.id "
		"WHERE pages.lang=? AND pages.slug=? AND history.version=?"
		<<locale_name<<slug<<ver<<cppdb::row;
	if(!r.empty()) {
		std::string title,content,sidebar;
		int uonly;
		r >> title >> content >> sidebar >> uonly;
		form.title.value(title);
		form.content.value(content);
		form.sidebar.value(sidebar);
		form.users_only.value(uonly);
		return true;
	}
	return false;
}

void page::display_ver(std::string slug,std::string sid)
{
	this->slug=slug;
	content::page_hist c;
	int id=atoi(sid.c_str());
	cppdb::result r;
	cppdb::session sql(conn);
	r=sql<<	"SELECT history.title,history.content,history.sidebar,history.created "
		"FROM pages "
		"JOIN history ON pages.id=history.id "
		"WHERE pages.lang=? AND pages.slug=? AND history.version=?"
			<<locale_name << slug << id << cppdb::row;
	if(!r.empty()) {
		redirect(locale_name,slug);
		return;
	}
	std::tm date;
	r>>c.title>>c.content>>c.sidebar>>date;
	c.date = mktime(&date);
	c.version=id;
	c.rollback=edit_version_url(id);
	ini(c);
	render("page_hist",c);
}

}


