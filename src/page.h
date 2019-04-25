#ifndef PAGE_H
#define PAGE_H

#include "master.h"
#include <string>

namespace content { 
	class page;
	class edit_page;
	class page_form;
}

namespace apps {

class page : public master {
	void save(int id,content::page_form &,cppdb::session &sql);
	bool load(content::page_form &);
	bool edit_on_post(content::edit_page &);
	bool load_history(int,content::page_form &);
protected:
	std::string slug;
	void ini(content::page &);
	void display(std::string slug);
	void history(std::string slug,std::string page);
	void display_ver(std::string slug,std::string sid);
	void edit(std::string slug,std::string version);
	void diff(std::string slug,std::string v1,std::string v2);

	std::string edit_url();
	std::string edit_version_url(int ver);
	std::string history_url(int n=0);
public:
	page(wiki &w);
	std::string diff_url(int v1,int v2,std::string lang="",std::string s="");
	std::string page_url(std::string l="",std::string s="");
	std::string page_version_url(int ver,std::string l="",std::string s="");
	std::string default_page_url(std::string l="en",std::string s="main")
		{ return page_url(l,s); }
	void redirect(std::string locale="en",std::string slug="main");
};

}


#endif
