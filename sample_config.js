{
	"wikipp" : {

		"script" : "/wikipp",
		"media" :"/media/",
		
		//
		// Set a path to hightligher to enable it
		//
		// "syntax_highlighter" : "/media/sh",
		//

		"disable_registration" : false,

		"languages" : {
			"en" : "en_US.UTF-8" ,
			"fr" : "fr_FR.UTF-8" ,
			"he" : "he_IL.UTF-8" ,
			"ru" : "ru_RU.UTF-8" ,
			"pl" : "pl_PL.UTF-8"
		},

		//
		// Setup connection string for DB
		//

		// Sqlite3 Sample Connection String
		//"connection_string" : "sqlite3:db=/var/wikipp/db/wikipp.db;@pool_size=16",
		//
		// PostgreSQL Sample Connection String
		// "connection_string" : "postgresql:dbname=wikipp;@pool_size=16",
		//
		// MySQL Sample Connection String
		//
		// "connection_string" : "mysql:database=wikipp;user=root;password=root;@pool_size=16",
		//
		// In Some cases mysql works faster without prepared statements as it uses query cache, so you
		// may change this string to:
		//
		// "connection_string" : "mysql:database=wikipp;user=root;password=root;@pool_size=16;@use_prepared=off",
		//	
	},
	"service" : {
		"api" : "fastcgi",
		"socket" : "stdin"
	},
	"session" : {
		"expire" : "renew",
		"location" : "client",
		"timeout" : 2592000, // One month 24*3600*30
		"cookies" :  {
			"prefix" : "wikipp"
		},
		"client" : {
			"cbc" : "aes",
			"hmac" :        "sha1",
			// setup these values using 
			//
			// cppcms_make_key --hmac sha1 --cbc aes
			//
			// "cbc_key" :     "Generate your own!",
			// "hmac_key" :    "Generate your own"
		}
	},
	"views" : {
		"paths" : [ "/usr/lib/wikipp"] ,
		"skins" : [ "view" ] ,
	},
	"localization" : {
		"messages" : { 
			"paths" : [ "/usr/share/locale"],
			"domains" :  [ "wikipp" ]
		},
		"locales" : [ 
			"fr_FR.UTF-8" ,
			"he_IL.UTF-8" ,
			"en_US.UTF-8",
			"ru_RU.UTF-8",
			"pl_PL.UTF-8" 
		]
	},
	"logging" : {
		"level" : "info",
		
		//"syslog" : {
		//	"enable": true,
		//	"id" : "WikiPP"
		//},

		//"file" : {
		//	"name" : "/var/log/wikipp/wikipp.log",
		//	"append" : true
		//}
	},
	"cache" : {
		"backend" : "thread_shared", 
		"limit" : 100, // items - thread cache
	},
	"security" : {
		"csrf" : { "enable" : true }
	}
}



