begin;
drop table if exists history;
drop table if exists pages;
drop table if exists users;
drop table if exists options;

create table options (
    lang varchar(16) not null,
    name varchar(32) not null,
    value text not null,
    unique(lang,name)
);

create table users (
    id integer primary key autoincrement not null,
    username varchar(32) unique not null,
    password varchar(32) not null
) ;

create table pages (
    id integer primary key autoincrement not null,
    lang varchar(16) not null,
    slug varchar(128) not null,
    title varchar(256) not null,
    content text not null,
    sidebar text not null,
    users_only integer not null,
    unique (lang,slug)
);


create table history (
    id integer not null,
    version integer not null,
    created datetime not null,
    author varchar(32) not null,
    title varchar(256) not null,
    content text not null,
    sidebar text not null,
    unique(id,version),
    primary key(id,version)
);

create index history_timeline on history(created);

commit;
