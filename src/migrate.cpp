#include <cppdb/frontend.h>
#include <iostream>

int main(int argc,char **argv)
{
	if(argc!=3) {
		std::cerr <<	"Wikipp database migration utility - allows to convert database from one\n"
				"engine to another. It supports: postgresql, mysql, sqlite3\n"
				"\n"
				"To use, create a new database using provided sql scripts and then run\n"
				"$ wikipp_migrate source_connection_string target_connection_string\n" 
				<< std::endl;
		return 1;
	}

	std::string cs_src = argv[1];
	std::string cs_tgt = argv[2];
	try {
		using namespace cppdb;
		session src(cs_src);
		session tgt(cs_tgt);
		transaction tr_src(src);
		transaction tr_tgt(tgt);

		result r;
		statement st;
		r = src << "SELECT lang,name,value FROM options";
		st = tgt<<"INSERT into options(lang,name,value) values(?,?,?)";
		while(r.next()) {
			std::string l,n,v;
			r>>l>>n>>v;
			st << l << n << v;
			st.exec();
			st.reset();
		}

		r = src << "SELECT id, username, password FROM users";
		st = tgt << "INSERT into users(id,username,password) values(?,?,?)";
		while(r.next()) {
			std::string id,user,pass;
			r >> id >> user >> pass;
			st << id << user << pass;
			st.exec();
			st.reset();
		}

		r = src << "SELECT id, lang, slug, title, content, sidebar, users_only FROM pages";
		st = tgt << "INSERT into pages(id, lang, slug, title, content, sidebar, users_only) values(?,?,?,?,?,?,?)";
		while(r.next()) {
			long long id;
			int users_only;
			std::string lang, slug, title, content, sidebar;
			r >> id >> lang >> slug >> title >> content >> sidebar >> users_only;
			st << id << lang << slug << title << content << sidebar << users_only;
			st.exec();
			st.reset();
		}
		r = src << "SELECT id, version, created, author, title, content, sidebar FROM history ";
		st = tgt << "INSERT INTO history(id, version, created, author, title, content, sidebar) VALUES(?,?,?,?,?,?,?)";
		while(r.next()) {
			long long id,version;
			std::tm created;
			std::string a,t,c,side;
			r >> id >> version >> created >> a >> t >> c >> side;
			st << id << version << created << a << t << c << side;
			st.exec();
			st.reset();
		}
		if(tgt.engine()=="postgresql") {
			tgt<<"SELECT setval('users_id_seq',(SELECT max(id) FROM users))" << row;
			tgt<<"SELECT setval('pages_id_seq',(SELECT max(id) FROM pages))" << row;
		}

		tr_src.commit();
		tr_tgt.commit();

	}
	catch(std::exception const &e) {
		std::cerr <<"Failed " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
