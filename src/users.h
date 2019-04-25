#ifndef USERS_H
#define USERS_H

#include "master.h"

namespace apps {

class users : public master {
	bool auth_done;
	bool auth_ok;
	bool disable_reg;

	void login();
	void do_auth();
	void new_user();
public:
	void reset();
	bool user_exists(std::string);
	bool check_login(std::string,std::string);
	bool auth();
	void error_forbidden();
	std::string username;
	std::string login_url();
	users(wiki &);
	
};


}
#endif


