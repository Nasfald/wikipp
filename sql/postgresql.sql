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
    id serial primary key not null,
    username varchar(32) unique not null,
    password varchar(32) not null
) ;

create table pages (
    id serial primary key not null,
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
    created timestamp not null,
    author varchar(32) not null,
    title varchar(256) not null,
    content text not null,
    sidebar text not null,
    unique(id,version),
    foreign key(id) references pages(id),
    primary key(id,version)
);

create index history_timeline on history(created);

commit;


