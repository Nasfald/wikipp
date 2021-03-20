{
	"wikipp" : {
		"script" : "/wikipp",
		"media" :"/media",
		//"syntax_highlighter" : "/media/sh",
		"disable_registration" : false,

		"languages" : {
			"en" : "en_US.UTF-8" ,
			"fr" : "fr_FR.UTF-8" ,
			"he" : "he_IL.UTF-8" ,
			"ru" : "ru_RU.UTF-8" ,
			"pl" : "pl_PL.UTF-8"
		},

		// Sqlite3 Sample Connection String
		"connection_string" : "sqlite3:db=./db/wikipp.db;@pool_size=16",
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
		"api" : "http",
		"port" : 8080,
		// "api" : "fastcgi",
		"socket" : "/tmp/wikipp.sock"
		// "socket" : "stdin"
	},
	"session" : {
		"expire" : "renew",
		"location" : "client",
		"timeout" : 2592000, // One month 24*3600*30
		"cookies" :  {
			"prefix" : "wikipp"
		},
		"server" : {
			"storage" : "files" 
		},
		"client" : {
			"encryptor" : "aes",
			"key" : "9bc6dbda707cb72ea1205dd5b1c90464"
		}
	},
	"views" : {
		"paths" : [ "./build"] ,
		"skins" : [ "view" ] ,
		//"auto_reload" : true
	},
	"file_server" : {
		"enable": true,
		"doument_root" : "."
	},
	"localization" : {
		// "backend" : "std", you may switch if for performance enhanecements 
		"messages" : { 
			"paths" : [ "./build/locale"],
			"domains" :  [ "wikipp" ]
		},
	    "locales" : [ "fr_FR.UTF-8", "he_IL.UTF-8" , "en_US.UTF-8", "ru_RU.UTF-8", "pl_PL.UTF-8" ]
	},
	"http" : {
		"script_names" : [ "/wikipp" ]
	},
	"logging" : {
		"level" : "info",
		"syslog" : {
			"enable": true,
			"id" : "WikiPP"
		},
		"file" : {
			"name" : "./wikipp.log",
			"append" : true
		}
	},
	"cache" : {
		"backend" : "thread_shared", 
		"limit" : 100, // items - thread cache
	},
	"security" : {
		"csrf" : { "enable" : true }
		// "multipart_form_data_limit" : 65536, // KB
		// "content_length_limit" : 1024, // KB
		// "uploads_path" : "" // temporary directory
		//
		// You may change to true for debugging only
		// "display_error_message" : false
	}

}



