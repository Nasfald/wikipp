#include "wiki.h"
#include "master.h"
#include "master_content.h"
#include "markdown.h"
#include <cppcms/localization.h>
#include <cppcms/service.h>
#include <cppcms/xss.h>

#define _(X) ::cppcms::locale::translate(X)
#define N_(S,P,N)  ::cppcms::locale::translate(S,P,N)

namespace {
	cppcms::xss::rules const &xss_filter()
	{
		static cppcms::xss::rules r;
		static bool initialized = false;
		if(initialized)
			return r;
		using namespace cppcms::xss;

		r.html(rules::xhtml_input);
		r.add_tag("ol",rules::opening_and_closing);
		r.add_tag("ul",rules::opening_and_closing);
		r.add_tag("li",rules::opening_and_closing);
		r.add_tag("p",rules::opening_and_closing);
		r.add_property("p","style",booster::regex("\\s*text-align\\s*:\\s*(center|left|right)\\s*;?"));
		r.add_tag("b",rules::opening_and_closing);
		r.add_tag("i",rules::opening_and_closing);
		r.add_tag("tt",rules::opening_and_closing);
		r.add_tag("sub",rules::opening_and_closing);
		r.add_tag("sup",rules::opening_and_closing);
		r.add_tag("blockquote",rules::opening_and_closing);
		r.add_tag("strong",rules::opening_and_closing);
		r.add_tag("em",rules::opening_and_closing);
		r.add_tag("h1",rules::opening_and_closing);
		r.add_tag("h2",rules::opening_and_closing);
		r.add_tag("h3",rules::opening_and_closing);
		r.add_tag("h4",rules::opening_and_closing);
		r.add_tag("h5",rules::opening_and_closing);
		r.add_tag("h6",rules::opening_and_closing);
		booster::regex cl_id(".*");
		r.add_property("h1","id",cl_id);
		r.add_property("h2","id",cl_id);
		r.add_property("h3","id",cl_id);
		r.add_property("h4","id",cl_id);
		r.add_property("h5","id",cl_id);
		r.add_property("h6","id",cl_id);
		r.add_tag("span",rules::opening_and_closing);
		r.add_property("span","id",cl_id);
		r.add_tag("code",rules::opening_and_closing);
		r.add_tag("pre",rules::opening_and_closing);
		r.add_property("pre","name",booster::regex("\\w+"));
		r.add_property("pre","class",booster::regex("\\w+"));
		r.add_tag("a",rules::opening_and_closing);
		r.add_uri_property("a","href");
		r.add_tag("hr",rules::stand_alone);
		r.add_tag("br",rules::stand_alone);
		r.add_tag("img",rules::stand_alone);
		r.add_uri_property("img","src");
		r.add_integer_property("img","width");
		r.add_integer_property("img","height");
		r.add_integer_property("img","border");
		r.add_property("img","alt",booster::regex(".*"));
		r.add_tag("table",rules::opening_and_closing);
		r.add_tag("tr",rules::opening_and_closing);
		r.add_tag("th",rules::opening_and_closing);
		r.add_tag("td",rules::opening_and_closing);
		r.add_integer_property("table","cellpadding");
		r.add_integer_property("table","cellspacing");
		r.add_integer_property("table","border");
		r.add_tag("center",rules::opening_and_closing);
		r.add_entity("nbsp");
		r.encoding("UTF-8");
		r.comments_allowed(true);

		initialized = true;
		return r;
	}

	struct init_it { init_it() { xss_filter(); } } instance;
}


const int K_markdown_flags =
0x00000004|		//MKD_NOPANTS
0x01000000| 	//MKD_DLEXTRA
0x02000000| 	//MKD_FENCEDCODE
0x08000000| 	//MKD_GITHUBTAGS
0x40000000| 	//MKD_LATEX
0;

std::string mymarkdown(std::string const &s)
{
	int flags = K_markdown_flags;
	if(s.compare(0,10,"<!--toc-->")==0) {
		flags |= mkd::toc;
	}
	std::string html = markdown_format_for_highlighting(markdown_to_html(s.c_str(),s.size(),flags),"cpp");
	return cppcms::xss::filter(html,xss_filter(),cppcms::xss::escape_invalid);
}

namespace apps {

master::master(wiki &_w) : 
	application(_w.service()),
	wi(_w),
	locale_name(wi.locale_name)
{
	conn = wi.conn;
	cppcms::json::object langs=settings().get("wikipp.languages",cppcms::json::object());
	for(cppcms::json::object::const_iterator p=langs.begin(),e=langs.end();p!=e;++p) {
		std::string lname;
		if(p->first=="en")
			lname="English";
		else {
			/// Translate as the target language
			/// for fr gettext("LANG")="Francis"
			lname=_("LANG").str(service().generator().generate(p->second.str()));
			if(lname=="LANG") {
				lname=p->first;
			}
		}
		languages[lname]=settings().get<std::string>("wikipp.script") +"/"+ p->first.str();
	}
	media=settings().get<std::string>("wikipp.media");
	syntax_highlighter=settings().get("wikipp.syntax_highlighter","");
	cookie_prefix=settings().get("session.cookies.prefix","cppcms_session")+"_";
}

void master::ini(content::master &c)
{
	wi.options.load();
	c.markdown = mymarkdown;
	c.media=media;
	c.syntax_highlighter=syntax_highlighter;
	c.cookie_prefix=cookie_prefix;
	c.main_link=wi.page.default_page_url();
	c.main_local=wi.page.default_page_url(locale_name);
	c.toc=wi.index.index_url();
	c.changes=wi.index.changes_url();
	c.login_link=wi.users.login_url();
	c.wiki_title=wi.options.local.title;
	c.about=wi.options.local.about;
	c.copyright=wi.options.local.copyright;
	c.contact=wi.options.global.contact;
	c.edit_options=wi.options.edit_url();
	c.languages=languages;
}


}
