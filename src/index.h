#ifndef INDEX_H
#define INDEX_H

#include "master.h"

namespace apps {

class wiki;

class index : public master {
	void changes(std::string);
	void display_index();
public:
	index(wiki &);
	std::string index_url();
	std::string changes_url(int n=0);
};

}


#endif
