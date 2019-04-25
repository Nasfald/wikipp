#ifndef PAGE_CONTENT_H
#define PAGE_CONTENT_H

#include "master_content.h"

namespace content {

typedef std::list<std::pair<int,std::string> > diff_t;

struct page_form : public cppcms::form {
	apps::wiki *w;
	cppcms::widgets::text title;
	cppcms::widgets::textarea content;
	cppcms::widgets::textarea sidebar;
	cppcms::widgets::submit save;
	cppcms::widgets::submit save_cont;
	cppcms::widgets::submit preview;
	cppcms::widgets::checkbox users_only;
	cppcms::form fields;
	cppcms::form buttons;
	page_form(apps::wiki *w);
	bool virtual validate();
};

struct page : public master {
	std::string title,content;
	std::string sidebar;
	std::string edit_link;
	std::string history_link;
};

struct page_hist: public page {
	int version;
	std::string rollback;
	time_t date;
};

struct edit_page: public page {
	page_form form;
	bool new_page;
	std::string back;
	std::string submit;
	edit_page(apps::wiki *w) : form(w),new_page(false) {}
};

struct history : public master {
	struct item {
		time_t update;
		std::string show_url;
		std::string edit_url;
		std::string diff_url;
		int version;
		std::string author;
	};
	std::vector<item> hist;
	std::string page;
	std::string title;
	std::string page_link;
};

struct diff: public master {
	std::string edit_v1,edit_v2;
	int v1,v2;
	diff_t content_diff_content;
	diff_t sidebar_diff_content;
	std::string title,title_1,title_2;
	bool title_diff,content_diff,sidebar_diff,no_versions,no_diff;
	diff() : 
		title_diff(false),content_diff(false),
		sidebar_diff(false),no_versions(false),
		no_diff(false)
	{
	}
};


} // namespace content

#endif
