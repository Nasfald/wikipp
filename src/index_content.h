#ifndef INDEX_CONTENT_H
#define INDEX_CONTENT_H

#include "master_content.h"

namespace content {

struct toc : public master {
	struct element {
		std::string title;
		std::string url;
	};
	typedef std::vector<std::vector<element> > table_type;
	table_type table;
};

struct recent_changes : public master {
	struct element {
		std::string title;
		int version;
		time_t created;
		std::string author;
		std::string url;
		std::string diff_url;
	};
	std::vector<element> content;
	std::string next;
};


} // namespace content


#endif
