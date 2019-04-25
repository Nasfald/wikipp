#include "index.h"
#include "index_content.h"
#include "wiki.h"

#include <cppcms/cache_interface.h>
#include <cppcms/url_dispatcher.h>

namespace apps {

index::index(wiki &w):
	master(w)
{
	wi.dispatcher().assign("^/index/?",&index::display_index,this);
	wi.dispatcher().assign("^/changes(/?|/(\\d+))$",&index::changes,this,2);
}

std::string index::index_url()
{
	return wi.root()+"/index/";
}

std::string index::changes_url(int p)
{
	if(p==0)
		return wi.root()+"/changes/";
	return wi.root()+(booster::locale::format("/changes/{1}") % p).str();
}

void index::changes(std::string page_no)
{
	std::string key=locale_name+"_changes_"+page_no;
	if(cache().fetch_page(key))
		return;
	int p;
	const unsigned window=30;
	if(page_no.empty())
		p=0;
	else
		p=atoi(page_no.c_str());
	cppdb::result rs;
	cppdb::session sql(conn);
	rs = sql<<
		"SELECT history.title,history.version,history.created,"
		"	history.author,pages.lang,pages.slug "
		"FROM history "
		"JOIN pages ON history.id=pages.id "
		"ORDER BY created DESC "
		"LIMIT ? "
		"OFFSET ?"
		<< window << p*window;
	content::recent_changes c;	
	c.content;
	int n;
	for(n=0;rs.next();n++) {
		c.content.push_back(content::recent_changes::element());
		content::recent_changes::element &d=c.content.back();
		std::string lang,slug;
		std::tm created;
		rs>>d.title>>d.version>>created>>d.author>>lang>>slug;
		d.created = mktime(&created);
		d.url=wi.page.page_version_url(d.version,lang,slug);
		if(d.version>1)
			d.diff_url=wi.page.diff_url(d.version-1,d.version,lang,slug);
	}
	if(c.content.size()==window)
		c.next=changes_url(p+1);
	ini(c);
	render("recent_changes",c);
	cache().store_page(key,60);
	// Cache changes for at most 30 sec
	// Generally -- prevent cache drop with frequent updates
}

void index::display_index()
{
	std::string key=locale_name+"_toc_index";
	if(cache().fetch_page(key))
		return;
	content::toc c;
	ini(c);
	cppdb::result r;
	cppdb::session sql(conn);
	r=sql<<	"SELECT slug,title FROM pages "
		"WHERE lang=? "
		"ORDER BY title ASC" << locale_name;
	std::string letter="";
	typedef std::multimap<std::string,std::string,std::locale> mapping_type;
	mapping_type mapping(context().locale());
	while(r.next()) {
		std::string slug,t;
		r >> slug >> t;
		mapping.insert(std::pair<std::string,std::string>(t,slug));
	}
	unsigned items=mapping.size();
	unsigned items_left=items/3;
	unsigned items_mid=items*2/3;

	mapping_type::iterator p=mapping.begin();
	int rows_no = (mapping.size() +2)/3;
	c.table.resize(rows_no,std::vector<content::toc::element>(3));
	for(unsigned i=0;p!=mapping.end();i++,++p) {

		int col = i / rows_no;
		int row = i % rows_no;

		content::toc::element &e = c.table.at(row).at(col);
		e.title = p->first;
		e.url=wi.page.page_url(locale_name,p->second);
	}
	render("toc",c);
	cache().store_page(key,30);
	// Cache TOC for at most 30 seconds
}
}
